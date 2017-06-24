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

#ifndef COMMANDEXECUTOR_H
#define COMMANDEXECUTOR_H

#include "makefile.h"
#include "process.h"
#include <QFile>
#include <QString>

QT_BEGIN_NAMESPACE
class QStringList;
QT_END_NAMESPACE

namespace NMakeFile {

class CommandExecutor : public QObject
{
    Q_OBJECT
public:
    CommandExecutor(QObject* parent, const ProcessEnvironment &environment);
    ~CommandExecutor();

    void start(DescriptionBlock* target);
    DescriptionBlock* target() { return m_pTarget; }
    void block();
    void unblock();
    bool isActive() const { return m_active; }
    void waitForFinished();
    void cleanupTempFiles();
    void setBufferedOutput(bool b) { m_process.setBufferedOutput(b); }
    bool isBufferedOutputSet() const { return m_process.isBufferedOutputSet(); }

public slots:
    void setEnvironment(const ProcessEnvironment &environment);

signals:
    void environmentChanged(const ProcessEnvironment &environment);
    void finished(CommandExecutor* process, bool abortMakeProcess);
    void subJomStarted();

private slots:
    void onProcessError(Process::ProcessError error);
    void onProcessFinished(int exitCode, Process::ExitStatus exitStatus);

private:
    void finishExecution(bool commandFailed);
    void executeCurrentCommandLine();
    void createTempFiles();
    void writeToChannel(const QByteArray& data, FILE *channel);
    void writeToStandardOutput(const QByteArray& data);
    void writeToStandardError(const QByteArray& data);
    bool isSimpleCommandLine(const QString &cmdLine);
    bool exec_cd(const QString &commandLine);

private:
    static ulong        m_startUpTickCount;
    static QString      m_tempPath;
    Process             m_process;
    DescriptionBlock*   m_pTarget;
    bool                m_blocked;
    bool                m_processFinishedWhileBlocked;

    struct TempFile
    {
        QFile* file;
        bool   keep;
    };

    QList<TempFile>     m_tempFiles;
    int                 m_currentCommandIdx;
    QString             m_nextWorkingDir;
    bool                m_ignoreProcessErrors;
    bool                m_active;
};

} // namespace NMakeFile

#endif // COMMANDEXECUTOR_H
