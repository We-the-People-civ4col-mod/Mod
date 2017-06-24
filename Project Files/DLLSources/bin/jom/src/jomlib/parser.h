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

#ifndef PARSER_H
#define PARSER_H

#include <QRegExp>
#include <QHash>
#include <QVector>
#include <QStack>
#include <QStringList>

#include "makefile.h"

namespace NMakeFile {

class Preprocessor;
class PPExpression;

class Parser
{
public:
    Parser();
    virtual ~Parser();

    void apply(Preprocessor* pp,
               Makefile* mkfile,
               const QStringList& activeTargets = QStringList());
    MacroTable* macroTable();

private:
    void readLine();
    bool isEmptyLine(const QString& line);
    bool isDescriptionBlock(int& separatorPos, int& separatorLength, int& commandSeparatorPos);
    bool isInferenceRule(const QString& line);
    bool isDotDirective(const QString& line);
    DescriptionBlock* createTarget(const QString& targetName);
    void parseDescriptionBlock(int separatorPos, int separatorLength, int commandSeparatorPos);
    void parseInferenceRule();
    void parseDotDirective();
    bool parseCommand(QList<Command>& commands, bool inferenceRule);
    void parseCommandLine(const QString& cmdLine, QList<Command>& commands, bool inferenceRule);
    void parseInlineFiles(Command& cmd, bool inferenceRule);
    void checkForCycles(DescriptionBlock* target);
    QVector<InferenceRule*> findRulesByTargetName(const QString& targetFilePath);
    void preselectInferenceRules(DescriptionBlock *target);
    void error(const QString& msg);

private:
    Preprocessor*               m_preprocessor;
    QString                     m_line;
    bool                        m_silentCommands;
    bool                        m_ignoreExitCodes;

    QRegExp                     m_rexDotDirective;
    QRegExp                     m_rexInferenceRule;
    QRegExp                     m_rexSingleWhiteSpace;

    Makefile*                   m_makefile;
    QStringList                 m_suffixes;
    QStringList                 m_activeTargets;
    QHash<QString, QStringList> m_syncPoints;
    QHash<QString, QVector<InferenceRule *> > m_ruleIdxByToExtension;
};

} // namespace NMakeFile

#endif // PARSER_H
