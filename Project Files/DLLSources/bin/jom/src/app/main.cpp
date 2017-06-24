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
#include "application.h"
#include <helperfunctions.h>
#include <options.h>
#include <parser.h>
#include <preprocessor.h>
#include <targetexecutor.h>
#include <exception.h>
#include <makefilefactory.h>

#include <QDebug>
#include <QDir>
#include <QProcess>
#include <QScopedPointer>
#include <QTextCodec>

#include <windows.h>
#include <Tlhelp32.h>

#ifndef __in
#define __in
#endif

using namespace NMakeFile;

const int nVersionMajor = 1;
const int nVersionMinor = 0;
const int nVersionPatch = 14;

static void showLogo()
{
    fprintf(stderr, "\njom %d.%d.%d - empower your cores\n\n",
        nVersionMajor, nVersionMinor, nVersionPatch);
    fflush(stderr);
}

static void showUsage()
{
    printf("Usage: jom @commandfile\n"
           "       jom [options] [/f makefile] [macro definitions] [targets]\n\n"
           "nmake compatible options:\n"
           "/A build all targets\n"
           "/D display build information\n"
           "/E override environment variable macros\n"
           "/F <filename> use the specified makefile\n"
           "/G display included makefiles\n"
           "/H show help\n"
           "/I ignore all exit codes\n"
           "/K keep going - build unrelated targets on error\n"
           "/N dry run - just print commands\n"
           "/NOLOGO do not print logo\n"
           "/P print makefile info\n"
           "/R ignore predefined rules and macros\n"
           "/S silent mode\n"
           "/U print content of inline files\n"
           "/L same as /NOLOGO\n"
           "/W print the working directory before and after other processing\n"
           "/X <filename> write stderr to file.\n"
           "/Y disable batch mode inference rules\n\n"
           "jom only options:\n"
           "/DUMPGRAPH show the generated dependency graph\n"
           "/DUMPGRAPHDOT dump dependency graph in dot format\n"
           "/J <n> use up to n processes in parallel\n"
           "/KEEPTEMPFILES keep all temporary files\n"
           "/VERSION print version and exit\n");
}

static TargetExecutor* g_pTargetExecutor = 0;

BOOL WINAPI ConsoleCtrlHandlerRoutine(__in  DWORD /*dwCtrlType*/)
{
    fprintf(stderr, "jom terminated by user (pid=%u)\n", QCoreApplication::applicationPid());
    fflush(stderr);

    GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0);
    if (g_pTargetExecutor)
        g_pTargetExecutor->removeTempFiles();

    exit(2);
    return TRUE;
}

QStringList getCommandLineArguments()
{
    QStringList commandLineArguments = qApp->arguments().mid(1);
    QString makeFlags = qGetEnvironmentVariable(L"MAKEFLAGS");
    if (!makeFlags.isEmpty())
        commandLineArguments.prepend(QLatin1Char('/') + makeFlags);
    return commandLineArguments;
}

int main(int argc, char* argv[])
{
    int result = 0;
    try {
        SetConsoleCtrlHandler(&ConsoleCtrlHandlerRoutine, TRUE);
        Application app(argc, argv);
        QTextCodec::setCodecForLocale(QTextCodec::codecForName("IBM 850"));
        MakefileFactory mf;
        Options* options = 0;
        mf.setEnvironment(QProcess::systemEnvironment());
        if (!mf.apply(getCommandLineArguments(), &options)) {
            switch (mf.errorType()) {
            case MakefileFactory::CommandLineError:
                showUsage();
                return 128;
            case MakefileFactory::ParserError:
            case MakefileFactory::IOError:
                fprintf(stderr, "Error: %s\n", qPrintable(mf.errorString()));
                return 2;
            }
        }

        if (options->showUsageAndExit) {
            if (options->showLogo)
                showLogo();
            showUsage();
            return 0;
        } else if (options->showVersionAndExit) {
            printf("jom version %d.%d.%d\n", nVersionMajor, nVersionMinor, nVersionPatch);
            return 0;
        }

        if (options->showLogo && !app.isSubJOM())
            showLogo();

        QScopedPointer<Makefile> mkfile(mf.makefile());
        if (options->displayMakeInformation) {
            printf("MACROS:\n\n");
            mkfile->macroTable()->dump();
            printf("\nINFERENCE RULES:\n\n");
            mkfile->dumpInferenceRules();
            printf("\nTARGETS:\n\n");
            mkfile->dumpTargets();
        }

        // ### HACK: pass the modified MAKEFLAGS variable to our environment.
        if (g_options.isMaxNumberOfJobsSet) {
            ProcessEnvironment environment = mkfile->macroTable()->environment();
            const QString makeFlags = mkfile->macroTable()->macroValue(QLatin1String("MAKEFLAGS"));
            environment[QLatin1String("MAKEFLAGS")] = makeFlags;
            MacroTable *mt = const_cast<MacroTable *>(mkfile->macroTable());
            mt->setEnvironment(environment);
            qSetEnvironmentVariable(QLatin1String("MAKEFLAGS"), makeFlags);
        }

        if (options->printWorkingDir) {
            printf("jom: Entering directory '%s\n",
                   qPrintable(QDir::toNativeSeparators(QDir::currentPath())));
            fflush(stdout);
        }

        TargetExecutor executor(mkfile->macroTable()->environment());
        QObject::connect(&executor, SIGNAL(finished(int)), &app, SLOT(exit(int)));
        g_pTargetExecutor = &executor;
        executor.apply(mkfile.data(), mf.activeTargets());

        QMetaObject::invokeMethod(&executor, "startProcesses", Qt::QueuedConnection);
        result = app.exec();
        g_pTargetExecutor = 0;
        if (options->printWorkingDir) {
            printf("jom: Leaving directory '%s'\n",
                   qPrintable(QDir::toNativeSeparators(QDir::currentPath())));
            fflush(stdout);
        }
    } catch (const Exception &e) {
        fprintf(stderr, "jom: %s", qPrintable(e.message()));
        result = 2;
    }
    return result;
}
