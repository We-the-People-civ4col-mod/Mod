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

#include "process.h"
#include <cstdio>

namespace NMakeFile {

Process::Process(QObject *parent)
    : QProcess(parent)
{
    connect(this, SIGNAL(error(QProcess::ProcessError)), SLOT(forwardError(QProcess::ProcessError)));
    connect(this, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(forwardFinished(int, QProcess::ExitStatus)));
}

void Process::setBufferedOutput(bool bufferedOutput)
{
    QProcess::setProcessChannelMode(bufferedOutput ? SeparateChannels : ForwardedChannels);
}

bool Process::isBufferedOutputSet() const
{
    return QProcess::processChannelMode() == SeparateChannels;
}

void Process::setEnvironment(const ProcessEnvironment &e)
{
    QProcessEnvironment qpenv;
    for (ProcessEnvironment::const_iterator it = e.constBegin(); it != e.constEnd(); ++it)
        qpenv.insert(it.key().toQString(), it.value());
    QProcess::setProcessEnvironment(qpenv);
}

ProcessEnvironment Process::environment() const
{
    ProcessEnvironment env;
    const QProcessEnvironment qpenv = QProcess::processEnvironment();
    foreach (const QString &key, qpenv.keys())
        env.insert(key, qpenv.value(key));
    return env;
}

bool Process::isRunning() const
{
    return QProcess::state() == QProcess::Running;
}

void Process::start(const QString &commandLine)
{
    QProcess::start(commandLine);
    QProcess::waitForStarted();
}

void Process::writeToStdOutBuffer(const QByteArray &output)
{
    fputs(output.data(), stdout);
    fflush(stdout);
}

void Process::writeToStdErrBuffer(const QByteArray &output)
{
    fputs(output.data(), stderr);
    fflush(stderr);
}

Process::ExitStatus Process::exitStatus() const
{
    return static_cast<Process::ExitStatus>(QProcess::exitStatus());
}

void Process::forwardError(QProcess::ProcessError qe)
{
    ProcessError e;
    switch (qe) {
    case QProcess::FailedToStart:
        e = FailedToStart;
        break;
    case QProcess::Crashed:
        e = Crashed;
        break;
    default:
        e = UnknownError;
    }
    emit error(e);
}

void Process::forwardFinished(int exitCode, QProcess::ExitStatus status)
{
    emit finished(exitCode, static_cast<Process::ExitStatus>(status));
}

} // namespace NMakeFile
