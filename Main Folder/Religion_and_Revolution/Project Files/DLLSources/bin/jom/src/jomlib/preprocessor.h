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

#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <QRegExp>
#include <QStack>
#include <QStringList>

class PPExprParser;

namespace NMakeFile {

class MacroTable;
class MakefileLineReader;

class Preprocessor
{
public:
    Preprocessor();
    ~Preprocessor();

    void setMacroTable(MacroTable* macroTable);
    MacroTable* macroTable() { return m_macroTable; }
    bool openFile(const QString& filename);
    QString readLine();
    uint lineNumber() const;
    QString currentFileName() const;
    int evaluateExpression(const QString& expr);
    bool isInlineFileMode() const { return m_bInlineFileMode; }
    void setInlineFileModeEnabled(bool enabled) { m_bInlineFileMode = enabled; }

    static void removeInlineComments(QString& line);

private:
    bool internalOpenFile(QString fileName);
    void basicReadLine(QString& line);
    bool parseMacro(const QString& line);
    bool parsePreprocessingDirective(const QString& line);
    bool isPreprocessingDirective(const QString& line, QString& directive, QString& value);
    void skipUntilNextMatchingConditional();
    void error(const QString& msg);
    void enterConditional(bool followElseBranch);
    void exitConditional();
    int conditionalDepth() { return m_conditionalStack.count(); }

private:
    struct TextFile
    {
        MakefileLineReader* reader;
        QString fileDirectory;

        TextFile()
            : reader(0)
        {}

        TextFile(const TextFile& rhs)
            : reader(rhs.reader), fileDirectory(rhs.fileDirectory)
        {}

        TextFile& operator=(const TextFile& rhs)
        {
            reader = rhs.reader;
            fileDirectory = rhs.fileDirectory;
            return *this;
        }
    };

    QStack<TextFile>    m_fileStack;
    MacroTable*         m_macroTable;
    QRegExp             m_rexPreprocessingDirective;
    QStack<bool>        m_conditionalStack;
    PPExprParser*       m_expressionParser;
    QStringList         m_linesPutBack;
    bool                m_bInlineFileMode;
};

} //namespace NMakeFile

#endif // PREPROCESSOR_H
