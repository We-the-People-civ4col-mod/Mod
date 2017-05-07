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

#include "filetime.h"
#include <windows.h>
#include <strsafe.h>

namespace NMakeFile {

template<bool> struct CompileTimeAssert;
template<> struct CompileTimeAssert<true> {};
static CompileTimeAssert<sizeof(FileTime::InternalType) == sizeof(FILETIME)> internal_type_has_wrong_size;

FileTime::FileTime()
    : m_fileTime(0)
{
}

bool FileTime::operator < (const FileTime &rhs) const
{
    const FILETIME *const t1 = reinterpret_cast<const FILETIME *>(&m_fileTime);
    const FILETIME *const t2 = reinterpret_cast<const FILETIME *>(&rhs.m_fileTime);
    return CompareFileTime(t1, t2) < 0;
}

void FileTime::clear()
{
    m_fileTime = 0;
}

bool FileTime::isValid() const
{
    return m_fileTime != 0;
}

FileTime FileTime::currentTime()
{
    FileTime result;
    SYSTEMTIME st;
    GetSystemTime(&st);
    FILETIME *const ft = reinterpret_cast<FILETIME *>(&result.m_fileTime);
    SystemTimeToFileTime(&st, ft);
    return result;
}

QString FileTime::toString() const
{
    const FILETIME *const ft = reinterpret_cast<const FILETIME *>(&m_fileTime);
    SYSTEMTIME stUTC, stLocal;
    FileTimeToSystemTime(ft, &stUTC);
    SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
    WCHAR szString[512];
    HRESULT hr = StringCchPrintf(szString, sizeof(szString) / sizeof(WCHAR),
                                 L"%02d.%02d.%d %02d:%02d:%02d",
                                 stLocal.wDay, stLocal.wMonth, stLocal.wYear,
                                 stLocal.wHour, stLocal.wMinute, stLocal.wSecond);
    return SUCCEEDED(hr) ? QString::fromUtf16((unsigned short*)szString) : QString();
}

} // namespace NMakeFile
