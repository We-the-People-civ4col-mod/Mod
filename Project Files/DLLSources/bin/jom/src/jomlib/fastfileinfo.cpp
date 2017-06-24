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

#include "fastfileinfo.h"

#include <QtCore/QDebug>
#include <QtCore/QHash>
#include <windows.h>

namespace NMakeFile {

template<bool> struct CompileTimeAssert;
template<> struct CompileTimeAssert<true> {};
static CompileTimeAssert<
    sizeof(FastFileInfo::InternalType) == sizeof(WIN32_FILE_ATTRIBUTE_DATA)
        > internal_type_has_wrong_size;

inline WIN32_FILE_ATTRIBUTE_DATA* z(FastFileInfo::InternalType &internalData)
{
    return reinterpret_cast<WIN32_FILE_ATTRIBUTE_DATA*>(&internalData);
}

inline const WIN32_FILE_ATTRIBUTE_DATA* z(const FastFileInfo::InternalType &internalData)
{
    return reinterpret_cast<const WIN32_FILE_ATTRIBUTE_DATA*>(&internalData);
}

static WIN32_FILE_ATTRIBUTE_DATA createInvalidFAD()
{
    WIN32_FILE_ATTRIBUTE_DATA fad = {0};
    fad.dwFileAttributes = INVALID_FILE_ATTRIBUTES;
    return fad;
}

static QHash<QString, WIN32_FILE_ATTRIBUTE_DATA> fadHash;

FastFileInfo::FastFileInfo(const QString &fileName)
{
    static const WIN32_FILE_ATTRIBUTE_DATA invalidFAD = createInvalidFAD();
    *z(m_attributes) = fadHash.value(fileName, invalidFAD);
    if (z(m_attributes)->dwFileAttributes != INVALID_FILE_ATTRIBUTES)
        return;

    if (!GetFileAttributesEx(reinterpret_cast<const TCHAR*>(fileName.utf16()),
                             GetFileExInfoStandard, &m_attributes))
    {
        z(m_attributes)->dwFileAttributes = INVALID_FILE_ATTRIBUTES;
        return;
    }

    fadHash.insert(fileName, *z(m_attributes));
}

bool FastFileInfo::exists() const
{
    return z(m_attributes)->dwFileAttributes != INVALID_FILE_ATTRIBUTES; 
}

FileTime FastFileInfo::lastModified() const
{
    const WIN32_FILE_ATTRIBUTE_DATA *fattr = z(m_attributes);
    if (fattr->dwFileAttributes == INVALID_FILE_ATTRIBUTES) {
        return FileTime();
    } else {
        return FileTime(reinterpret_cast<const FileTime::InternalType&>(fattr->ftLastWriteTime));
    }
}

void FastFileInfo::clearCacheForFile(const QString &fileName)
{
    fadHash.remove(fileName);
}

} // NMakeFile
