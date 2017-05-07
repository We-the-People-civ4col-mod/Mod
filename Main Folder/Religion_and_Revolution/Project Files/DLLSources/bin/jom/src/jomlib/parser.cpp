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

#include "parser.h"
#include "preprocessor.h"
#include "options.h"
#include "exception.h"
#include "helperfunctions.h"

#include <QDebug>
#include <QDir>
#include <QDirIterator>

namespace NMakeFile {

Parser::Parser()
:   m_preprocessor(0)
{
    m_rexDotDirective.setPattern(QLatin1String("^\\.(IGNORE|PRECIOUS|SILENT|SUFFIXES)\\s*:(.*)"));
    m_rexInferenceRule.setPattern(QLatin1String("^(\\{.*\\})?(\\.\\w+)(\\{.*\\})?(\\.\\w+)(:{1,2})"));
    m_rexSingleWhiteSpace.setPattern(QLatin1String("\\s"));
}

Parser::~Parser()
{
}

/**
 * Parses the content, provided by the Preprocessor object and
 * creates a new Makefile object.
 */
void Parser::apply(Preprocessor* pp,
                   Makefile* mkfile,
                   const QStringList& activeTargets)
{
    m_makefile = mkfile;
    m_activeTargets = activeTargets;
    m_preprocessor = pp;
    const Options* options = mkfile->options();
    m_silentCommands = options->suppressOutputMessages;
    m_ignoreExitCodes = !options->stopOnErrors;
    m_suffixes.clear();
    m_suffixes << QLatin1String(".exe")
               << QLatin1String(".obj")
               << QLatin1String(".asm")
               << QLatin1String(".c")
               << QLatin1String(".cpp")
               << QLatin1String(".cxx")
               << QLatin1String(".bas")
               << QLatin1String(".cbl")
               << QLatin1String(".for")
               << QLatin1String(".pas")
               << QLatin1String(".res")
               << QLatin1String(".rc");
    m_syncPoints.clear();
    m_ruleIdxByToExtension.clear();
    int dbSeparatorPos, dbSeparatorLength, dbCommandSeparatorPos;

    try {
        readLine();
        while (!m_line.isNull()) {
            QString expandedLine = m_preprocessor->macroTable()->expandMacros(m_line);
            if (isEmptyLine(expandedLine)) {
                readLine();
            } else if (isDotDirective(expandedLine)) {
                m_line = expandedLine;
                Preprocessor::removeInlineComments(m_line);
                parseDotDirective();
            } else if (isInferenceRule(expandedLine)) {
                m_line = expandedLine;
                Preprocessor::removeInlineComments(m_line);
                parseInferenceRule();
            } else if (isDescriptionBlock(dbSeparatorPos, dbSeparatorLength, dbCommandSeparatorPos)) {
                parseDescriptionBlock(dbSeparatorPos, dbSeparatorLength, dbCommandSeparatorPos);
            } else {
                error(QLatin1String("syntax error"));
            }
        }
    } catch (FileException&) {
        throw;
    } catch (Exception &e) {
        // Enrich the error message with filename and line number.
        error(e.message());
    }

    // if the makefile doesn't contain target, we can stop here
    if (m_makefile->targets().isEmpty())
        return;

    // make sure that all active targets exist
    foreach (const QString& targetName, m_activeTargets) {
        if (!m_makefile->target(targetName)) {
            QString msg = QLatin1String("Target %1 doesn't exist.");
            throw Exception(msg.arg(targetName));
        }
    }

    // if no active target is defined, use the first one
    if (m_activeTargets.isEmpty()) {
        m_activeTargets.append(m_makefile->firstTarget()->targetName());
    }

    m_makefile->calculateInferenceRulePriorities(m_suffixes);

    // translate sync points from .SYNC targets into real dependencies
    for (QHash<QString, QStringList>::const_iterator it = m_syncPoints.constBegin();
        it != m_syncPoints.constEnd(); ++it)
    {
        DescriptionBlock *target = m_makefile->target(it.key());
        if (!target)
            break;
        target->m_dependents += it.value();
        target->m_dependents.removeDuplicates();
    }

    // build rule suffix cache
    for (int i = m_makefile->inferenceRules().count(); --i >= 0;) {
        InferenceRule *ir = m_makefile->inferenceRules().at(i);
        m_ruleIdxByToExtension[ir->m_toExtension].prepend(ir);
    }

    // check for cycles in active targets
    foreach (const QString& targetName, m_activeTargets) {
        DescriptionBlock *target = m_makefile->target(targetName);
        checkForCycles(target);
        preselectInferenceRules(target);
    }
}

MacroTable* Parser::macroTable()
{
    return m_preprocessor->macroTable();
}

void Parser::readLine()
{
    m_line = m_preprocessor->readLine();
}

bool Parser::isEmptyLine(const QString& line)
{
    return line.trimmed().isEmpty();
}

/**
 * Returns the index of the command separator or -1 if its non-existent.
 */
static int removeCommentsAndFindCommandSeparator(QString& line)
{
    if (line.isEmpty())
        return -1;

    QVector<bool> quotedVector(line.length());
    quotedVector[0] = (line.at(0) == QLatin1Char('"'));
    for (int i=1; i < line.length(); ++i) {
        bool isInDoubleQuote = quotedVector[i-1];
        if (line.at(i) == QLatin1Char('"'))
            isInDoubleQuote = !isInDoubleQuote;
        quotedVector[i] = isInDoubleQuote;
    }

    int commentIdx = -1;
    QList<int> toRemove;
    while (true) {
        commentIdx = line.indexOf(QLatin1Char('#'), commentIdx + 1);
        if (commentIdx > 0 && (line.at(commentIdx - 1) == QLatin1Char('^'))) {
            toRemove.append(commentIdx - 1);  // remove the ^ characters later
            continue;
        }
        if (commentIdx < 0 || !quotedVector[commentIdx])
            break;
    }

    int cmdSepIdx = -1;
    do {
        cmdSepIdx = line.indexOf(QLatin1Char(';'), cmdSepIdx + 1);
    } while (cmdSepIdx >= 0 && quotedVector[cmdSepIdx]);

    if (commentIdx == cmdSepIdx)  // equality means, they are both -1
        return -1;

    if ((0 <= commentIdx && commentIdx < cmdSepIdx) || cmdSepIdx == -1) {
        // The # indicates a comment.
        line.truncate(commentIdx);
        for (int i=toRemove.count() - 1; i >= 0; i--)
            line.remove(toRemove.at(i), 1);
        cmdSepIdx = -1;
    } else {
        // The # is inside the command and not a comment.
    }
    return cmdSepIdx;
}

/**
 * Test if this a description block and determine the separator position and length.
 *
 * The colon is in only one case not a separator:
 * If there's a single character before the colon.
 * In this case its interpreted as a drive letter.
 */
bool Parser::isDescriptionBlock(int& separatorPos, int& separatorLength, int& commandSeparatorPos)
{
    commandSeparatorPos = removeCommentsAndFindCommandSeparator(m_line);

    const int lineLength = m_line.length();
    if (lineLength == 0)
        return false;

    const QChar firstChar = m_line.at(0);
    if (isSpaceOrTab(firstChar))
        return false;

    separatorPos = -1;
    int i = -1;
    int parenthesisLevel = 0;
    for (i=0; i < m_line.count(); ++i) {
        const QChar &ch = m_line.at(i);
        if (ch == QLatin1Char('(')) {
            ++parenthesisLevel;
        } else if (ch == QLatin1Char(')')) {
            --parenthesisLevel;
        } else if (parenthesisLevel == 0 && ch == QLatin1Char(':')) {
            // Is it a drive letter at the beginning of the line?
            if (i == 1 && firstChar.isLetter())
                continue;
            // Is it a drive letter somewhere in the line?
            if (i > 1 && m_line.at(i-1).isLetter()) {
                const QChar &ch2 = m_line.at(i-2);
                if (ch2.isSpace() || ch2 == QLatin1Char('"'))
                    continue;
            }
            // No further objections. We've found the separator.
            separatorPos = i;
            break;
        }
    }

    if (separatorPos < 0)
        return false;

    const int idx = separatorPos + 1;
    if (idx < lineLength && m_line.at(idx) == QLatin1Char(':'))
        separatorLength = 2;
    else
        separatorLength = 1;

    return true;
}

bool Parser::isInferenceRule(const QString& line)
{
    return m_rexInferenceRule.exactMatch(line);
}

bool Parser::isDotDirective(const QString& line)
{
    return m_rexDotDirective.exactMatch(line);
}

DescriptionBlock* Parser::createTarget(const QString& targetName)
{
    DescriptionBlock* target = new DescriptionBlock(m_makefile);
    target->setTargetName(targetName);
    m_makefile->append(target);
    return target;
}

static void split_append_helper(QStringList& lst, const QString& str, int from, int to)
{
    QString entry;
    entry = str.mid(from, to - from);
    entry = entry.trimmed();
    removeDoubleQuotes(entry);
    if (!entry.isEmpty())
        lst.append(entry);
}

static QStringList splitTargetNames(const QString& str)
{
    QStringList lst;
    bool inDoubleQuotes = false;
    int from = 0, to = 0;
    for (int i=0; i < str.length(); ++i) {
        const QChar ch = str.at(i);
        if (ch == QLatin1Char('\"'))
            inDoubleQuotes = !inDoubleQuotes;
        else if (!inDoubleQuotes && (ch == QLatin1Char(' ') || ch == QLatin1Char('\t')))
            to = i;
        if (from < to) {
            split_append_helper(lst, str, from, to);
            to++;
            from = to;
        }
    }

    if (from < str.length()) {
        split_append_helper(lst, str, from, str.length());
    }
    return lst;
}

static bool containsWildcard(const QString &str)
{
    // Scan for * and ? but ignore patterns like $* and $?
    enum State {
        InitialState,
        AfterDollarState,
        AfterDollarParenState
    };
    State s = InitialState;
    for (int i = 0; i < str.length(); ++i) {
        const QChar ch = str.at(i);
        if (ch == MacroTable::fileNameMacroMagicEscape) {
            s = AfterDollarState;
            continue;
        }
        if (s == AfterDollarState) {
            s = (ch.unicode() == '(') ? AfterDollarParenState : InitialState;
        } else if (s == AfterDollarParenState) {
            s = InitialState;
        } else {
            const ushort c = ch.unicode();
            if (c == '*' || c == '?')
                return true;
        }
    }
    return false;
}

static QStringList expandWildcards(const QString &dirPath, const QStringList &lst)
{
    QStringList result;
    result.reserve(lst.count());
    foreach (const QString &str, lst) {
        if (containsWildcard(str)) {
            QDirIterator dit(dirPath, QStringList(str));
            while (dit.hasNext()) {
                QString filePath = dit.next();
                if (filePath.startsWith(dirPath, Qt::CaseInsensitive)) {
                    filePath.remove(0, dirPath.length());
                    if (filePath.startsWith(QLatin1Char('/')))
                       filePath.remove(0, 1);
                }
                result.append(filePath);
            }
        } else {
            result.append(str);
        }
    }
    return result;
}

void Parser::parseDescriptionBlock(int separatorPos, int separatorLength, int commandSeparatorPos)
{
    QString target = m_line.left(separatorPos).trimmed();
    target = m_preprocessor->macroTable()->expandMacros(target);
    QString value = m_line;
    if (commandSeparatorPos >= 0) value.truncate(commandSeparatorPos);
    value.remove(0, separatorPos + separatorLength);
    value = m_preprocessor->macroTable()->expandMacros(value.trimmed(), true);
    target = QDir::toNativeSeparators(target);
    value = QDir::toNativeSeparators(value);

    // extract command from description block line
    QList<Command> commands;
    if (commandSeparatorPos >= 0) {
        QString inPlaceCommand;
        inPlaceCommand = m_line;
        inPlaceCommand.remove(0, commandSeparatorPos + 1);
        inPlaceCommand = inPlaceCommand.trimmed();
        parseCommandLine(inPlaceCommand, commands, false);
    }

    readLine();
    if (m_line.trimmed().isEmpty()) {
        readLine();
    } else {
        while (parseCommand(commands, false))
            readLine();
    }

    const QStringList targets = splitTargetNames(target);
    QStringList dependents = splitTargetNames(value);
    dependents = expandWildcards(m_makefile->dirPath(), dependents);

    // handle the special .SYNC dependents
    {
        QStringList syncDeps;
        int k = 0;
        for (int i=0; i < dependents.count();) {
            if (dependents.at(i) == QLatin1String(".SYNC")) {
                dependents.removeAt(i);
                if (dependents.count() == i)
                    break;
                syncDeps = dependents.mid(k, i - k);
                k = i;
            } else {
                m_syncPoints[dependents.at(i)] += syncDeps;
                ++i;
            }
        }
    }

    foreach (const QString& t, targets) {
        DescriptionBlock* descblock = m_makefile->target(t);
        DescriptionBlock::AddCommandsState canAddCommands = separatorLength > 1 ? DescriptionBlock::ACSEnabled : DescriptionBlock::ACSDisabled;
        if (descblock) {
            if (canAddCommands != descblock->m_canAddCommands &&
                descblock->m_canAddCommands != DescriptionBlock::ACSUnknown)
            {
                error(QLatin1String("cannot have : and :: dependents for same target"));
                return;
            }
            descblock->m_canAddCommands = canAddCommands;
        } else {
            descblock = createTarget(t);
            descblock->m_canAddCommands = canAddCommands;
            canAddCommands = DescriptionBlock::ACSEnabled;
        }
        descblock->m_dependents.append(dependents);
        descblock->expandFileNameMacrosForDependents();

        if (!commands.isEmpty()) {
            if (canAddCommands == DescriptionBlock::ACSEnabled || descblock->m_commands.isEmpty())
                descblock->m_commands.append(commands);
            else
                qWarning("Cannot add commands to previously defined target %s.", qPrintable(t));
        }

        //qDebug() << "parseDescriptionBlock" << descblock->m_targetName << descblock->m_dependents;
    }
}

bool Parser::parseCommand(QList<Command>& commands, bool inferenceRule)
{
    // eat empty lines
    while (m_line.trimmed().isEmpty()) {
        readLine();
        if (m_line.isNull())
            return false;
    }

    // check if we have a command line
    if (!startsWithSpaceOrTab(m_line))
        return false;

    parseCommandLine(m_line, commands, inferenceRule);
    return true;
}

void Parser::parseCommandLine(const QString& cmdLine, QList<Command>& commands, bool inferenceRule)
{
    commands.append(Command());
    Command& cmd = commands.last();
    if (m_ignoreExitCodes) cmd.m_maxExitCode = 255;
    cmd.m_silent = m_silentCommands;

    if (inferenceRule) {
        cmd.m_commandLine = cmdLine.trimmed();
    } else {
        cmd.m_commandLine = m_preprocessor->macroTable()->expandMacros(cmdLine.trimmed());
        cmd.evaluateModifiers();
    }

    parseInlineFiles(cmd, inferenceRule);
}

void Parser::parseInlineFiles(Command& cmd, bool inferenceRule)
{
    // First, create the InlineFile objects from the command line.
    int fileNamePos = 0;
    while ((fileNamePos = cmd.m_commandLine.indexOf(QLatin1String("<<"), fileNamePos)) >= 0) {
        fileNamePos += 2;
        InlineFile* inlineFile = new InlineFile();
        cmd.m_inlineFiles.append(inlineFile);

        // Determine the inline file name, if given.
        if (cmd.m_commandLine.count() > fileNamePos && !cmd.m_commandLine.at(fileNamePos).isSpace()) {
            if (cmd.m_commandLine.at(fileNamePos) == QLatin1Char('"')) {
                // quoted file name
                int idx = cmd.m_commandLine.indexOf(QLatin1Char('"'), fileNamePos + 1);
                if (idx == -1) {
                    QString msg = QLatin1String("missing quote for inline file name in %0, line %1");
                    qWarning("%s", qPrintable(msg.arg(m_preprocessor->currentFileName()).arg(m_preprocessor->lineNumber())));
                } else {
                    inlineFile->m_filename = cmd.m_commandLine.mid(fileNamePos + 1, idx - fileNamePos - 1);
                    cmd.m_commandLine.remove(fileNamePos, idx - fileNamePos + 1);
                }
            } else {
                // unquoted file name
                int idx = fileNamePos + 1;
                while (idx < cmd.m_commandLine.count() && !cmd.m_commandLine.at(idx).isSpace())
                    ++idx;
                inlineFile->m_filename = cmd.m_commandLine.mid(fileNamePos, idx - fileNamePos);
                cmd.m_commandLine.remove(fileNamePos, idx - fileNamePos);
            }
        }
    }

    // Read the content for each inline file.
    m_preprocessor->setInlineFileModeEnabled(true);
    foreach (InlineFile* inlineFile, cmd.m_inlineFiles) {
        readLine();
        while (!m_line.isNull()) {
            if (m_line.startsWith(QLatin1String("<<"))) {
                QStringList options = m_line.right(m_line.length() - 2).split(m_rexSingleWhiteSpace);
                if (options.contains(QLatin1String("KEEP")))
                    inlineFile->m_keep = true;
                if (options.contains(QLatin1String("UNICODE")))
                    inlineFile->m_unicode = true;
                break;
            }

            QString contentLine;
            if (inferenceRule)
                contentLine = m_line;
            else
                contentLine = m_preprocessor->macroTable()->expandMacros(m_line);

            if (!contentLine.endsWith(QLatin1String("\r\n"))) {
                if (contentLine.endsWith(QLatin1Char('\n')))
                    contentLine.insert(contentLine.length() - 1, QLatin1Char('\r'));
                else
                    contentLine.append(QLatin1String("\r\n"));
            }

            inlineFile->m_content.append(contentLine);
            readLine();
        }
    }
    m_preprocessor->setInlineFileModeEnabled(false);
}

void Parser::parseInferenceRule()
{
    QString fromPath = m_rexInferenceRule.cap(1);
    QString fromExt  = m_rexInferenceRule.cap(2);
    QString toPath   = m_rexInferenceRule.cap(3);
    QString toExt    = m_rexInferenceRule.cap(4);
    bool batchMode   = m_rexInferenceRule.cap(5).length() > 1;

    if (fromPath.length() >= 2)
        fromPath = fromPath.mid(1, fromPath.length() - 2);
    if (toPath.length() >= 2)
        toPath = toPath.mid(1, toPath.length() - 2);
    if (fromPath.isEmpty())
        fromPath = QLatin1String(".");
    if (toPath.isEmpty())
        toPath = QLatin1String(".");

    //qDebug() << fromPath << fromExt
    //         << toPath << toExt << batchMode;

    removeDirSeparatorAtEnd(fromPath);
    removeDirSeparatorAtEnd(toPath);

    InferenceRule *rule = new InferenceRule();
    rule->m_batchMode = batchMode;
    rule->m_fromSearchPath = fromPath;
    rule->m_fromExtension = fromExt;
    rule->m_toSearchPath = toPath;
    rule->m_toExtension = toExt;

    readLine();
    while (parseCommand(rule->m_commands, true))
        readLine();

    m_makefile->addInferenceRule(rule);
}

void Parser::parseDotDirective()
{
    QString directive = m_rexDotDirective.cap(1);
    QString value = m_rexDotDirective.cap(2);
    //qDebug() << m_rexDotDirective.cap(1) << m_rexDotDirective.cap(2);

    if (directive == QLatin1String("SUFFIXES")) {
        QStringList splitvalues = value.simplified().split(m_rexSingleWhiteSpace, QString::SkipEmptyParts);
        //qDebug() << "splitvalues" << splitvalues;
        if (splitvalues.isEmpty())
            m_suffixes.clear();
        else
            m_suffixes.append(splitvalues);
        //qDebug() << ".SUFFIXES" << m_suffixes;
    } else if (directive == QLatin1String("IGNORE")) {
        m_ignoreExitCodes = true;
    } else if (directive == QLatin1String("PRECIOUS")) {
        const QStringList& splitvalues = value.split(m_rexSingleWhiteSpace);
        foreach (QString str, splitvalues)
            if (!str.isEmpty())
                m_makefile->addPreciousTarget(str);
    } else if (directive == QLatin1String("SILENT")) {
        m_silentCommands = true;
    }

    readLine();
}

void Parser::checkForCycles(DescriptionBlock* target)
{
    if (!target)
        return;

    if (target->m_bVisitedByCycleCheck) {
        QString msg = QLatin1String("cycle in targets detected: %1");
        throw Exception(msg.arg(target->targetName()));
    }

    target->m_bVisitedByCycleCheck = true;
    for (int i = target->m_dependents.count(); --i >= 0;) {
        DescriptionBlock *const dep = m_makefile->target(target->m_dependents.at(i));
        checkForCycles(dep);
    }
    target->m_bVisitedByCycleCheck = false;
}

QVector<InferenceRule*> Parser::findRulesByTargetName(const QString& targetFilePath)
{
    QVector<InferenceRule *> rules;

    const QHash<QString, QVector<InferenceRule *> >::const_iterator itEnd = m_ruleIdxByToExtension.constEnd();
    for (QHash<QString, QVector<InferenceRule *> >::const_iterator it = m_ruleIdxByToExtension.constBegin(); it != itEnd; ++it) {
        const QString &toExtension = it.key();
        if (!targetFilePath.endsWith(toExtension))
            continue;

        foreach (InferenceRule *rule, it.value()) {
            if (rule->m_priority < 0)
                continue;

            QString fileName = fileNameFromFilePath(targetFilePath);
            QString directory = targetFilePath.left(targetFilePath.length() - fileName.length());
            removeDirSeparatorAtEnd(directory);
            if (directory.isEmpty())
                directory = QLatin1Char('.');
            if (directory != rule->m_toSearchPath)
                continue;

            rules.append(rule);
        }
    }

    return rules;
}

void Parser::preselectInferenceRules(DescriptionBlock *target)
{
    if (target->m_commands.isEmpty()) {
        QVector<InferenceRule *> rules = findRulesByTargetName(target->targetName());
        if (!rules.isEmpty())
            target->m_inferenceRules = rules;
    }
    foreach (const QString &dependentName, target->m_dependents) {
        DescriptionBlock *dependent = m_makefile->target(dependentName);
        if (dependent) {
            preselectInferenceRules(dependent);
        } else {
            QString dependentFileName = dependentName;
            removeDoubleQuotes(dependentFileName);
            QVector<InferenceRule *> rules = findRulesByTargetName(dependentFileName);
            if (!rules.isEmpty()) {
                dependent = createTarget(dependentFileName);
                dependent->m_inferenceRules = rules;
            }
        }
    }
}

void Parser::error(const QString& msg)
{
    throw FileException(msg, m_preprocessor->currentFileName(), m_preprocessor->lineNumber());
}

} // namespace NMakeFile
