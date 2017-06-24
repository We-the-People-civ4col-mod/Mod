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

#ifndef FILETIME_H
#define FILETIME_H

#include <QtGlobal>
#include <QString>

namespace NMakeFile {

class FileTime
{
public:
    FileTime();

    typedef quint64 InternalType;

    FileTime(const InternalType &ft)
        : m_fileTime(ft)
    { }

    bool operator < (const FileTime &rhs) const;
    bool operator <= (const FileTime &rhs) const
    {
        return operator < (rhs) || operator == (rhs);
    }
    bool operator == (const FileTime &rhs) const
    {
        return m_fileTime == rhs.m_fileTime;
    }

    void clear();
    bool isValid() const;
    QString toString() const;

    static FileTime currentTime();

private:
    friend class FastFileInfo;
    InternalType m_fileTime;
};

} // namespace NMakeFile

#endif // FILETIME_H
