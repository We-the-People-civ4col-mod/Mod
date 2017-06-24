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

#ifndef PROCESS_H
#define PROCESS_H

#include "processenvironment.h"
#include <QObject>
#include <QStringList>

#ifdef USE_QPROCESS

#include <QProcess>

namespace NMakeFile {

class Process : public QProcess
{
    Q_OBJECT
public:
    enum ProcessError
    {
        UnknownError,
        FailedToStart,
        Crashed
    };

    enum ExitStatus
    {
        NormalExit,
        CrashExit
    };

    Process(QObject *parent = 0);
    void setBufferedOutput(bool bufferedOutput);
    bool isBufferedOutputSet() const;
    void setEnvironment(const ProcessEnvironment &e);
    ProcessEnvironment environment() const;
    bool isRunning() const;
    void start(const QString &commandLine);
    void writeToStdOutBuffer(const QByteArray &output);
    void writeToStdErrBuffer(const QByteArray &output);
    ExitStatus exitStatus() const;

signals:
    void error(Process::ProcessError);
    void finished(int, Process::ExitStatus);

private slots:
    void forwardError(QProcess::ProcessError);
    void forwardFinished(int, QProcess::ExitStatus);
};

} // namespace NMakeFile

#else

namespace NMakeFile {

class Process : public QObject
{
    Q_OBJECT
public:
    explicit Process(QObject *parent = 0);
    ~Process();

    enum ProcessError
    {
        UnknownError,
        FailedToStart,
        Crashed
    };

    enum ExitStatus
    {
        NormalExit,
        CrashExit
    };

    enum ProcessState
    {
        NotRunning,
        Starting,
        Running
    };

    void setBufferedOutput(bool b);
    bool isBufferedOutputSet() const { return m_bufferedOutput; }
    void writeToStdOutBuffer(const QByteArray &output);
    void writeToStdErrBuffer(const QByteArray &output);
    void setWorkingDirectory(const QString &path);
    const QString &workingDirectory() const { return m_workingDirectory; }
    void setEnvironment(const ProcessEnvironment &environment);
    const ProcessEnvironment &environment() const { return m_environment; }
    int exitCode() const { return m_exitCode; }
    ExitStatus exitStatus() const { return m_exitStatus; }
    bool isRunning() const { return m_state == Running; }

signals:
    void error(Process::ProcessError);
    void finished(int, Process::ExitStatus);

public slots:
    void start(const QString &commandLine);
    bool waitForFinished();

private:
    void printBufferedOutput();

private slots:
    void tryToRetrieveExitCode();
    void onProcessFinished();

private:
    class ProcessPrivate *d;
    QString m_workingDirectory;
    ProcessEnvironment m_environment;
    QByteArray m_envBlock;
    ProcessState m_state;
    int m_exitCode;
    ExitStatus m_exitStatus;
    bool m_bufferedOutput;

    friend class ProcessPrivate;
};

} // namespace NMakeFile

#endif // USE_QPROCESS

#endif // PROCESS_H
