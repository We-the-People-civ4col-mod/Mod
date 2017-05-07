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

#ifndef OPTIONS_H
#define OPTIONS_H

#include <QtCore/QStringList>
#include <QtCore/QSharedPointer>

namespace NMakeFile {

class MacroTable;

class Options
{
public:
    Options();

    bool readCommandLineArguments(QStringList arguments, QString& makefile,
                                  QStringList& targets, MacroTable& macroTable);

    bool buildAllTargets;
    bool buildIfTimeStampsAreEqual;
    bool showLogo;
    bool suppressOutputMessages;
    bool overrideEnvVarMacros;
    bool displayIncludeFileNames;
    bool dryRun;
    bool stopOnErrors;
    bool buildUnrelatedTargetsOnError;
    bool checkTimeStampsButDoNotBuild;
    bool changeTimeStampsButDoNotBuild;
    bool ignorePredefinedRulesAndMacros;
    bool suppressExecutedCommandsDisplay;
    bool printWorkingDir;
    bool batchModeEnabled;
    bool dumpInlineFiles;
    bool dumpDependencyGraph;
    bool dumpDependencyGraphDot;
    bool displayMakeInformation;
    bool showUsageAndExit;
    bool displayBuildInfo;
    bool debugMode;
    bool showVersionAndExit;
    bool keepTemporaryFiles;
    QString fullAppPath;
    QString stderrFile;

private:
    bool expandCommandFiles(QStringList& arguments);
    bool handleCommandLineOption(const QStringList &originalArguments, QString arg, QStringList& arguments, QString& makefile, QString& makeflags);
};

class GlobalOptions
{
public:
    GlobalOptions();
    int maxNumberOfJobs;
    bool isMaxNumberOfJobsSet;
};

extern GlobalOptions g_options;

} // namespace NMakeFile

#endif // OPTIONS_H
