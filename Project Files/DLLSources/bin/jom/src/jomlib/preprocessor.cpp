/****************************************************************************
 **
 ** Copyright (C) 2008-2014 Digia Plc and/or its subsidiary(-ies).
 ** Contact: http://www.qt-project.org/legal
 **
 ** This file is part of the jom project on Trolltech Labs.
 **
 ** This file may be used under the terms of the GNU General Public
 ** License version 2.0 or 3.0 as published by the Free Software Foundation
 ** and appearing in the file LICENSE.GPL included in the packaging of
 ** this file.  Please review the following information to ensure GNU
 ** General Public Licensing requirements will be met:
 ** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
 ** http://www.gnu.org/copyleft/gpl.html.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ****************************************************************************/

#include "preprocessor.h"
#include "ppexprparser.h"
#include "macrotable.h"
#include "exception.h"
#include "makefilelinereader.h"
#include "helperfunctions.h"
#include "fastfileinfo.h"

#include <QDir>
#include <QDebug>

namespace NMakeFile {

Preprocessor::Preprocessor()
:   m_macroTable(0),
    m_expressionParser(0),
    m_bInlineFileMode(false)
{
    m_rexPreprocessingDirective.setPattern(QLatin1String("^!\\s*(\\S+)(.*)"));
}

Preprocessor::~Preprocessor()
{
    delete m_expressionParser;
}

void Preprocessor::setMacroTable(MacroTable* macroTable)
{
    m_macroTable = macroTable;
    if (m_expressionParser)
        m_expressionParser->setMacroTable(m_macroTable);
}

bool Preprocessor::openFile(const QString& fileName)
{
    m_conditionalStack.clear();
    if (!m_fileStack.isEmpty())
        m_fileStack.clear();

    return internalOpenFile(fileName);
}

bool Preprocessor::internalOpenFile(QString fileName)
{
    if (fileName.startsWith(QLatin1Char('"')) && fileName.endsWith(QLatin1Char('"')))
        fileName = fileName.mid(1, fileName.length() - 2);
    else if (fileName.startsWith(QLatin1Char('<')) && fileName.endsWith(QLatin1Char('>'))) {
        fileName = fileName.mid(1, fileName.length() - 2);
        QString includeVar = m_macroTable->macroValue(QLatin1String("INCLUDE")).replace(QLatin1Char('\t'), QLatin1Char(' '));
        QStringList includeDirs = includeVar.split(QLatin1Char(';'), QString::SkipEmptyParts);
        QString fullFileName;
        foreach (const QString& includeDir, includeDirs) {
            fullFileName = includeDir + QDir::separator() + fileName;
            if (QFile::exists(fullFileName)) {
                fileName = fullFileName;
                break;
            }
        }
    } else {
        QString fullFileName = fileName;
        QStack<TextFile> tmpStack;
        while (m_fileStack.count() >= 1) {
            if (QFile::exists(fullFileName)) {
                fileName = fullFileName;
                break;
            }
            TextFile textFile = m_fileStack.pop();
            tmpStack.push(textFile);
            fullFileName = textFile.fileDirectory + QDir::separator() + fileName;
        }
        while (!tmpStack.isEmpty())
            m_fileStack.push(tmpStack.pop());
    }

    // make file name absolute for safe cycle detection
    const QString origFileName = fileName;
    QFileInfo fileInfo(fileName);
    if (!fileInfo.exists()) {
        QString msg = QLatin1String("File %1 doesn't exist.");
        error(msg.arg(origFileName));
    }
    fileName = fileInfo.absoluteFilePath();

    // detect include cycles
    foreach (const TextFile& tf, m_fileStack)
        if (tf.reader->fileName() == fileName)
            error(QLatin1String("cycle in include files: ") + fileInfo.fileName());

    MakefileLineReader* reader = new MakefileLineReader(fileName);
    if (!reader->open()) {
        delete reader;
        error(QLatin1Literal("Can't open ") + origFileName);
    }

    m_fileStack.push(TextFile());
    TextFile& textFile = m_fileStack.top();
    textFile.reader = reader;
    textFile.fileDirectory = fileInfo.absolutePath();
    return true;
}

QString Preprocessor::readLine()
{
    QString line;
    for (;;) {
        basicReadLine(line);
        if (!m_bInlineFileMode && parseMacro(line))
            continue;
        if (parsePreprocessingDirective(line))
            continue;
        break;
    }
    return line;
}

uint Preprocessor::lineNumber() const
{
    if (m_fileStack.isEmpty())
        return 0;
    return m_fileStack.top().reader->lineNumber();
}

QString Preprocessor::currentFileName() const
{
    if (m_fileStack.isEmpty())
        return QString();
    return m_fileStack.top().reader->fileName();
}

void Preprocessor::basicReadLine(QString& line)
{
    if (!m_linesPutBack.isEmpty()) {
        line = m_linesPutBack.takeFirst();
        return;
    }

    if (m_fileStack.isEmpty()) {
        line = QString();
        return;
    }

    line = m_fileStack.top().reader->readLine(m_bInlineFileMode);
    while (line.isNull()) {
        delete m_fileStack.top().reader;
        m_fileStack.pop();
        if (m_fileStack.isEmpty())
            return;
        line = m_fileStack.top().reader->readLine(m_bInlineFileMode);
    }
}

bool Preprocessor::parseMacro(const QString& line)
{
    if (line.isEmpty())
        return false;

    static const QRegExp rex(QLatin1String("^(?:_|[a-z]|[0-9]|\\$)(?:[a-z]|[0-9]|\\$|=|\\()?"),
                             Qt::CaseInsensitive, QRegExp::RegExp2);
    if (rex.indexIn(line) != 0)
        return false;

    int equalsSignPos = -1;
    int parenthesisDepth = 0;
    for (int i=1; i < line.count(); ++i) {
        const QChar &ch = line.at(i);
        if (ch == QLatin1Char('(')) {
            ++parenthesisDepth;
        } else if (ch == QLatin1Char(')')) {
            --parenthesisDepth;
        } else if (parenthesisDepth == 0) {
            if (ch == QLatin1Char('=')) {
                equalsSignPos = i;
                break;
            } else if (ch == QLatin1Char(':')) {
                // A colon (outside parenthesis) to the left of an equals sign is not a valid
                // macro assignment. This is likely to be a description block.
                break;
            }
        }
    }

    if (equalsSignPos < 0)
        return false;

    QString name = line.left(equalsSignPos).trimmed();
    QString value = line.mid(equalsSignPos + 1).trimmed();
    removeInlineComments(value);
    //qDebug() << "parseMacro" << name << value;
    m_macroTable->setMacroValue(name, value);
    return true;
}

bool Preprocessor::parsePreprocessingDirective(const QString& line)
{
    QString directive, value;
    QString expandedLine = m_macroTable->expandMacros(line);
    if (!isPreprocessingDirective(expandedLine, directive, value))
        return false;

    if (directive == QLatin1String("CMDSWITCHES")) {
    } else if (directive == QLatin1String("ERROR")) {
        error(QLatin1Literal("ERROR: ") + value);
    } else if (directive == QLatin1String("MESSAGE")) {
        puts(qPrintable(value));
    } else if (directive == QLatin1String("INCLUDE")) {
        internalOpenFile(value);
    } else if (directive == QLatin1String("IF")) {
        bool followElseBranch = evaluateExpression(value) == 0;
        enterConditional(followElseBranch);
        if (followElseBranch) {
            skipUntilNextMatchingConditional();
        }
    } else if (directive == QLatin1String("IFDEF")) {
        bool followElseBranch = !m_macroTable->isMacroDefined(value);
        enterConditional(followElseBranch);
        if (followElseBranch) {
            skipUntilNextMatchingConditional();
        }
    } else if (directive == QLatin1String("IFNDEF")) {
        bool followElseBranch = m_macroTable->isMacroDefined(value);
        enterConditional(followElseBranch);
        if (followElseBranch) {
            skipUntilNextMatchingConditional();
        }
    } else if (directive == QLatin1String("ELSE")) {
        if (conditionalDepth() == 0)
            error(QLatin1String("unexpected ELSE"));
        if (!m_conditionalStack.top()) {
            skipUntilNextMatchingConditional();
        }
    } else if (directive == QLatin1String("ELSEIF")) {
        if (conditionalDepth() == 0)
            error(QLatin1String("unexpected ELSE"));
        if (!m_conditionalStack.top() || evaluateExpression(value) == 0) {
            skipUntilNextMatchingConditional();
        } else {
            m_conditionalStack.pop();
            m_conditionalStack.push(false);
        }
    } else if (directive == QLatin1String("ELSEIFDEF")) {
        if (conditionalDepth() == 0)
            error(QLatin1String("unexpected ELSE"));
        if (!m_conditionalStack.top() || !m_macroTable->isMacroDefined(value)) {
            skipUntilNextMatchingConditional();
        } else {
            m_conditionalStack.pop();
            m_conditionalStack.push(false);
        }
    } else if (directive == QLatin1String("ELSEIFNDEF")) {
        if (conditionalDepth() == 0)
            error(QLatin1String("unexpected ELSE"));
        if (!m_conditionalStack.top() || m_macroTable->isMacroDefined(value)) {
            skipUntilNextMatchingConditional();
        } else {
            m_conditionalStack.pop();
            m_conditionalStack.push(false);
        }
    } else if (directive == QLatin1String("ENDIF")) {
        exitConditional();
    } else if (directive == QLatin1String("UNDEF")) {
        m_macroTable->undefineMacro(value);
    }

    return true;
}

bool Preprocessor::isPreprocessingDirective(const QString& line, QString& directive, QString& value)
{
    if (line.isEmpty())
        return false;

    const QChar firstChar = line.at(0);
    if (isSpaceOrTab(firstChar))
        return false;

    bool oldStyleIncludeDirectiveFound = false;
    if (firstChar != QLatin1Char('!') && line.length() > 8) {
        const char ch = line.at(7).toLatin1();
        if (!isSpaceOrTab(QLatin1Char(ch)))
            return false;

        if (line.left(7).toLower() == QLatin1String("include"))
            oldStyleIncludeDirectiveFound = true;
        else
            return false;
    }

    bool result = true;
    if (oldStyleIncludeDirectiveFound) {
        directive = QLatin1String("INCLUDE");
        value = line.mid(8);
    } else {
        result = m_rexPreprocessingDirective.exactMatch(line);
        if (result) {
            directive = m_rexPreprocessingDirective.cap(1).toUpper();
            value = m_rexPreprocessingDirective.cap(2).trimmed();
        }
    }

    value = m_macroTable->expandMacros(value);
    removeInlineComments(value);
    return result;
}

void Preprocessor::skipUntilNextMatchingConditional()
{
    uint depth = 0;
    QString line, directive, value;

    enum DirectiveToken { TOK_IF, TOK_ENDIF, TOK_ELSE, TOK_UNINTERESTING };
    DirectiveToken token;
    do {
        basicReadLine(line);
        if (line.isNull())
            return;

        QString expandedLine = m_macroTable->expandMacros(line);
        if (!isPreprocessingDirective(expandedLine, directive, value))
            continue;

        if (directive == QLatin1String("ENDIF"))
            token = TOK_ENDIF;
        else if (directive.startsWith(QLatin1String("IF")))
            token = TOK_IF;
        else if (directive.startsWith(QLatin1String("ELSE")))
            token = TOK_ELSE;
        else
            token = TOK_UNINTERESTING;

        if (token == TOK_UNINTERESTING)
            continue;

        if (depth == 0) {
            if (token == TOK_ELSE) {
                m_linesPutBack.append(expandedLine);
                return;  // found the next matching ELSE
            }
            if (token == TOK_ENDIF) {
                exitConditional();
                return;  // found the next matching ENDIF
            }
        }

        if (token == TOK_ENDIF)
            --depth;
        else if (token == TOK_IF)
            ++depth;

    } while (!line.isNull());
}

void Preprocessor::enterConditional(bool followElseBranch)
{
    m_conditionalStack.push(followElseBranch);
}

void Preprocessor::exitConditional()
{
    if (m_conditionalStack.isEmpty())
        error(QLatin1String("unexpected ENDIF"));
    m_conditionalStack.pop();
}

int Preprocessor::evaluateExpression(const QString& expr)
{
    if (!m_expressionParser) {
        m_expressionParser = new PPExprParser;
        m_expressionParser->setMacroTable(m_macroTable);
    }

    if (!m_expressionParser->parse(qPrintable(m_macroTable->expandMacros(expr)))) {
        QString msg = QLatin1String("Can't evaluate preprocessor expression.");
        msg += QLatin1String("\nerror: ");
        msg += QString::fromLatin1(m_expressionParser->errorMessage());
        msg += QLatin1String("\nexpression: ");
        msg += expr;
        error(msg);
    }

    return m_expressionParser->expressionValue();
}

void Preprocessor::error(const QString& msg)
{
    throw FileException(msg, currentFileName(), lineNumber());
}

void Preprocessor::removeInlineComments(QString& line)
{
    int idx = -1;
    while (true) {
        idx = line.indexOf(QLatin1Char('#'), idx + 1);
        if (idx > 0 && line.at(idx - 1) == QLatin1Char('^')) {
            line.remove(idx - 1, 1);
            continue;
        }
        break;
    }
    if (idx >= 0) {
        line.truncate(idx);
        line = line.trimmed();
    }
}

} // namespace NMakeFile
