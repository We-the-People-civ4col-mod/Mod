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

#ifndef FASTFILEINFO_H
#define FASTFILEINFO_H

#include "filetime.h"

namespace NMakeFile {

class FastFileInfo
{
public:
    FastFileInfo(const QString &fileName);

    bool exists() const;
    FileTime lastModified() const;

    static void clearCacheForFile(const QString &fileName);

    struct InternalType
    {
        quint8 z[36];
    };

private:
    InternalType m_attributes;
};

} // NMakeFile

#endif // FASTFILEINFO_H
