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

#include "commandexecutor.h"
#include "options.h"
#include "exception.h"
#include "helperfunctions.h"
#include "fastfileinfo.h"

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QRegExp>
#include <QStringList>
#include <windows.h>

namespace NMakeFile {

ulong CommandExecutor::m_startUpTickCount = 0;
QString CommandExecutor::m_tempPath;

CommandExecutor::CommandExecutor(QObject* parent, const ProcessEnvironment &environment)
:   QObject(parent),
    m_pTarget(0),
    m_blocked(false),
    m_processFinishedWhileBlocked(false),
    m_ignoreProcessErrors(false),
    m_active(false)
{
    if (m_startUpTickCount == 0)
        m_startUpTickCount = GetTickCount();

    if (m_tempPath.isNull()) {
        WCHAR buf[MAX_PATH];
        DWORD count = GetTempPathW(MAX_PATH, buf);
        if (count) {
            m_tempPath = QString::fromWCharArray(buf, count);
            if (!m_tempPath.endsWith(QDir::separator())) m_tempPath.append(QDir::separator());
        }
    }

    m_process.setEnvironment(environment);
    connect(&m_process, SIGNAL(error(Process::ProcessError)), SLOT(onProcessError(Process::ProcessError)));
    connect(&m_process, SIGNAL(finished(int, Process::ExitStatus)), SLOT(onProcessFinished(int, Process::ExitStatus)));
}

CommandExecutor::~CommandExecutor()
{
    cleanupTempFiles();
}

void CommandExecutor::start(DescriptionBlock* target)
{
    m_pTarget = target;
    m_active = true;

    if (target->m_commands.isEmpty()) {
        finishExecution(false);
        return;
    }

    target->expandFileNameMacros();
    cleanupTempFiles();
    createTempFiles();

    m_ignoreProcessErrors = false;
    m_currentCommandIdx = 0;
    m_nextWorkingDir.clear();
    m_process.setWorkingDirectory(m_nextWorkingDir);
    executeCurrentCommandLine();
}

void CommandExecutor::block()
{
    m_blocked = true;
}

void CommandExecutor::unblock()
{
    if (m_blocked) {
        m_blocked = false;
        if (m_processFinishedWhileBlocked) {
            m_processFinishedWhileBlocked = false;
            onProcessFinished(m_process.exitCode(), m_process.exitStatus());
        }
    }
}

void CommandExecutor::onProcessError(Process::ProcessError error)
{
    //qDebug() << "onProcessError" << error;
    if (!m_ignoreProcessErrors)
        onProcessFinished(2, (error == Process::Crashed) ? Process::CrashExit : Process::NormalExit);
}

void CommandExecutor::onProcessFinished(int exitCode, Process::ExitStatus exitStatus)
{
    //qDebug() << "onProcessFinished" << m_pTarget->m_targetName;
    if (m_blocked) {
        m_processFinishedWhileBlocked = true;
        return;
    }

    if (exitStatus != Process::NormalExit)
        exitCode = 2;

    const Command &currentCommand = m_pTarget->m_commands.at(m_currentCommandIdx);
    if (exitCode > currentCommand.m_maxExitCode) {
        QByteArray msg = "jom: ";
        msg += QDir::toNativeSeparators(QDir::currentPath() + QDir::separator()).toLocal8Bit();
        msg += m_pTarget->makefile()->fileName().toLocal8Bit();
        msg += " [" + m_pTarget->targetName().toLocal8Bit() + "] Error ";
        msg += QByteArray::number(exitCode);
        msg += "\n";
        writeToStandardError(msg);
        finishExecution(true);
        return;
    }

    ++m_currentCommandIdx;
    if (m_currentCommandIdx < m_pTarget->m_commands.count()) {
        executeCurrentCommandLine();
    } else {
        finishExecution(false);
    }
}

void CommandExecutor::finishExecution(bool commandFailed)
{
    m_active = false;
    emit finished(this, commandFailed);
}

void CommandExecutor::waitForFinished()
{
    m_process.waitForFinished();
}

inline bool commandLineStartsWithCommand(const QString &str, const QString &searchString)
{
    return str.length() > searchString.length()
        && str.at(searchString.length()).isSpace()
        && str.startsWith(searchString, Qt::CaseInsensitive);
}

static bool startsWithShellBuiltin(const QString &commandLine)
{
    static QRegExp rex(QLatin1String(
        "^(assoc|break|call|chdir|cd|cls|color|copy|del|dir|echo|endlocal|erase|exit|for|ftype|goto"
        "|if|md|move|path|pause|popd|prompt|pushd|ren|rename|setlocal|shift|time|title|type|ver|verify|vol)\\s"
        ), Qt::CaseInsensitive, QRegExp::RegExp2);
    return rex.indexIn(commandLine) >= 0;
}

void CommandExecutor::executeCurrentCommandLine()
{
    m_processFinishedWhileBlocked = false;
    const Command& cmd = m_pTarget->m_commands.at(m_currentCommandIdx);
    QString commandLine = cmd.m_commandLine;
    int jomCallIdx = commandLine.indexOf(m_pTarget->makefile()->options()->fullAppPath);
    bool spawnJOM = (jomCallIdx >= 0);

    if (m_pTarget->makefile()->options()->dryRun
        || (!cmd.m_silent && !m_pTarget->makefile()->options()->suppressExecutedCommandsDisplay))
    {
        QByteArray output = commandLine.toLocal8Bit();
        output.prepend('\t');
        output.append('\n');
        writeToStandardOutput(output);
    }

    // Unescape commandline characters.
    commandLine.replace(QLatin1String("%%"), QLatin1String("%"));

    static QRegExp rexShellComment(QLatin1String("^(:|rem\\s)"),
                                   Qt::CaseInsensitive, QRegExp::RegExp2);
    if (m_pTarget->makefile()->options()->dryRun || (rexShellComment.indexIn(commandLine) >= 0))
    {
        onProcessFinished(0, Process::NormalExit);
        return;
    }

    if (!m_nextWorkingDir.isEmpty()) {
        m_process.setWorkingDirectory(m_nextWorkingDir);
        m_nextWorkingDir.clear();
    }

    const bool simpleCmdLine = isSimpleCommandLine(commandLine);
    if (simpleCmdLine)
    {
        // handle builtins
        bool builtInHandled = true;
        bool success = true;
        if (commandLineStartsWithCommand(commandLine, QLatin1String("cd"))) {
            success = exec_cd(commandLine);
        } else if (commandLineStartsWithCommand(commandLine, QLatin1String("set"))) {
            QString variableAssignment = commandLine;
            variableAssignment = variableAssignment.remove(0, 4).trimmed();
            int idx = variableAssignment.indexOf(QLatin1Char('='));
            if (idx >= 0) {
                QString variableName = variableAssignment.left(idx);
                QString variableValue = variableAssignment.mid(idx + 1);
                ProcessEnvironment environment = m_process.environment();
                environment.insert(variableName, variableValue);
                setEnvironment(environment);
                emit environmentChanged(environment);
            }
        } else {
            builtInHandled = false;
        }
        if (builtInHandled) {
            onProcessFinished(success ? 0 : 1, Process::NormalExit);
            return;
        }
    }

    if (spawnJOM)
        emit subJomStarted();

    bool executionSucceeded = false;
    if (simpleCmdLine && !startsWithShellBuiltin(commandLine)) {
        // ### It would be cool if we would not try to start every command directly.
        //     If its a shell builtin not handled by "startsWithShellBuiltin" IncrediBuild
        //     might complain about the failed process.
        //     Instead try to locate the binary using the PATH and so on and directly
        //     start the program using the absolute path.
        //     Code to locate the binary must be compatible to what CreateProcess does.

        //qDebug("+++ direct exec");
        m_ignoreProcessErrors = true;
        m_process.start(commandLine);
        executionSucceeded = m_process.isRunning();
        m_ignoreProcessErrors = false;
    }

    if (!executionSucceeded) {
        //qDebug("+++ shell exec");

        // Check if there are more than three double quotes in the command.
        // We must properly escape it. See "cmd /?" for the reason.
        int doubleQuoteCount(0), idx(0);
        const QChar doubleQuote = QLatin1Char('"');
        while (doubleQuoteCount < 3 && (commandLine.indexOf(doubleQuote, idx) >= 0))
            ++doubleQuoteCount;

        if (doubleQuoteCount >= 3) {
            commandLine.prepend(doubleQuote);
            commandLine.append(doubleQuote);
        }

        QString shellCmd = qGetEnvironmentVariable(L"ComSpec");
        if (shellCmd.isEmpty())
            shellCmd = QLatin1String("cmd.exe");

        commandLine = shellCmd + QLatin1Literal(" /C ") + commandLine;
        m_process.start(commandLine);
        executionSucceeded = m_process.isRunning();
    }

    if (!executionSucceeded)
        qFatal("Can't start command: %s", qPrintable(commandLine));
}

void CommandExecutor::createTempFiles()
{
    QList<Command>::iterator it = m_pTarget->m_commands.begin();
    QList<Command>::iterator itEnd = m_pTarget->m_commands.end();
    for (; it != itEnd; ++it) {
        Command& cmd = *it;
        foreach (InlineFile* inlineFile, cmd.m_inlineFiles) {
            QString fileName;
            if (inlineFile->m_filename.isEmpty()) {
                do {
                    QString simplifiedTargetName = m_pTarget->targetName();
                    simplifiedTargetName = fileNameFromFilePath(simplifiedTargetName);
                    fileName = m_tempPath + simplifiedTargetName + QLatin1Char('.')
                               + QString::number(GetCurrentProcessId()) + QLatin1Char('.')
                               + QString::number(GetTickCount() - m_startUpTickCount)
                               + QLatin1Literal(".jom");
                } while (QFile::exists(fileName));
            } else
                fileName = inlineFile->m_filename;

            TempFile tempFile;
            tempFile.keep = inlineFile->m_keep || m_pTarget->makefile()->options()->keepTemporaryFiles;
            tempFile.file = new QFile(fileName);
            if (!tempFile.file->open(QFile::WriteOnly)) {
                delete tempFile.file;
                QString msg = QLatin1String("cannot open %1 for write");
                throw Exception(msg.arg(fileName));
            }

            const QByteArray content = inlineFile->m_content.toLocal8Bit();
            if (m_pTarget->makefile()->options()->dumpInlineFiles) {
                writeToStandardOutput("---");
                writeToStandardOutput(fileName.toLocal8Bit());
                writeToStandardOutput("---\n");
                writeToStandardOutput(content);
                writeToStandardOutput("---end of inline file---\n");
            }

            // TODO: do something with inlineFile->m_unicode;
            tempFile.file->write(content);
            tempFile.file->close();

            QString replacement = QDir::toNativeSeparators(tempFile.file->fileName());
            if (replacement.contains(QLatin1Char(' ')) || replacement.contains(QLatin1Char('\t'))) {
                replacement.prepend(QLatin1Char('"'));
                replacement.append(QLatin1Char('"'));
            }

            int idx = cmd.m_commandLine.indexOf(QLatin1String("<<"));
            if (idx > 0)
                cmd.m_commandLine.replace(idx, 2, replacement);

            m_tempFiles.append(tempFile);
        }
    }
}

void CommandExecutor::cleanupTempFiles()
{
    while (!m_tempFiles.isEmpty()) {
        const TempFile& tempfile = m_tempFiles.takeLast();
        if (!tempfile.keep) tempfile.file->remove();
        delete tempfile.file;
    }
}

void CommandExecutor::writeToChannel(const QByteArray& data, FILE *channel)
{
    fputs(data, channel);
    fflush(channel);
}

void CommandExecutor::writeToStandardOutput(const QByteArray& output)
{
    if (m_process.isBufferedOutputSet())
        m_process.writeToStdOutBuffer(output);
    else
        writeToChannel(output, stdout);
}

void CommandExecutor::writeToStandardError(const QByteArray& output)
{
    if (m_process.isBufferedOutputSet())
        m_process.writeToStdErrBuffer(output);
    else
        writeToChannel(output, stderr);
}

bool CommandExecutor::isSimpleCommandLine(const QString &commandLine)
{
    static QRegExp rex(QLatin1String("\\||>|<|&"));
    return rex.indexIn(commandLine) == -1;
}

bool CommandExecutor::exec_cd(const QString &commandLine)
{
    QString args = commandLine.right(commandLine.count() - 3);    // cut of "cd "
    args = args.trimmed();

    if (args.isEmpty()) {
        // cd, called without arguments, prints the current working directory.
        QString cwd = m_process.workingDirectory();
        if (cwd.isNull())
            cwd = QDir::currentPath();
        writeToStandardOutput(QDir::toNativeSeparators(cwd).toLocal8Bit());
        writeToStandardOutput("\n");
        return true;
    }

    bool changeDrive = false;
    if (args.startsWith(QLatin1String("/d"), Qt::CaseInsensitive)) {
        changeDrive = true;
        args.remove(0, 3);
        args = args.trimmed();
        if (args.isEmpty()) {
            // "cd /d" does nothing.
            return true;
        }
    }

    removeDoubleQuotes(args);
    QFileInfo fi(args);
    if (!fi.exists()) {
        QString msg = QLatin1String("Couldn't change working directory to %0.\n");
        writeToStandardError(msg.arg(args).toLocal8Bit());
        return false;
    }

    if (!changeDrive) {
        // ### handle the case where we cd into a dir on a drive != current drive 
    }

    m_nextWorkingDir = fi.absoluteFilePath();
    return true;
}

void CommandExecutor::setEnvironment(const ProcessEnvironment &environment)
{
    m_process.setEnvironment(environment);
}

} // namespace NMakeFile
