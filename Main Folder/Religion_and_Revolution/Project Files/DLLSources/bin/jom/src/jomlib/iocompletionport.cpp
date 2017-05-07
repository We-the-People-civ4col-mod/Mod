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

#include "iocompletionport.h"

namespace NMakeFile {

IoCompletionPort::IoCompletionPort()
    : hPort(INVALID_HANDLE_VALUE)
{
    setObjectName(QLatin1String("I/O completion port thread"));
    HANDLE hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
    if (!hIOCP) {
        qWarning("CreateIoCompletionPort failed with error code %d.\n", GetLastError());
        return;
    }
    hPort = hIOCP;
}

IoCompletionPort::~IoCompletionPort()
{
    PostQueuedCompletionStatus(hPort, 0, NULL, NULL);
    QThread::wait();
    CloseHandle(hPort);
}

void IoCompletionPort::registerObserver(IoCompletionPortObserver *observer, HANDLE hFile)
{
    HANDLE hIOCP = CreateIoCompletionPort(hFile, hPort, reinterpret_cast<ULONG_PTR>(observer), 0);
    if (!hIOCP) {
        qWarning("Can't associate file handle with I/O completion port. Error code %d.\n", GetLastError());
        return;
    }
    mutex.lock();
    observers.insert(observer);
    mutex.unlock();
    if (!QThread::isRunning())
        QThread::start();
}

void IoCompletionPort::unregisterObserver(IoCompletionPortObserver *observer)
{
    mutex.lock();
    observers.remove(observer);
    mutex.unlock();
}

void IoCompletionPort::run()
{
    DWORD dwBytesRead;
    ULONG_PTR pulCompletionKey;
    OVERLAPPED *overlapped;

    for (;;) {
        BOOL success = GetQueuedCompletionStatus(hPort,
                                                &dwBytesRead,
                                                &pulCompletionKey,
                                                &overlapped,
                                                INFINITE);

        DWORD errorCode = success ? ERROR_SUCCESS : GetLastError();
        if (!success && !overlapped) {
            printf("GetQueuedCompletionStatus failed with error code %d.\n", errorCode);
            return;
        }

        if (success && !(dwBytesRead || pulCompletionKey || overlapped)) {
            // We've posted null values via PostQueuedCompletionStatus to end this thread.
            return;
        }

        IoCompletionPortObserver *observer = reinterpret_cast<IoCompletionPortObserver *>(pulCompletionKey);
        mutex.lock();
        if (observers.contains(observer))
            observer->completionPortNotified(dwBytesRead, errorCode);
        mutex.unlock();
    }
}

} // namespace NMakeFile
