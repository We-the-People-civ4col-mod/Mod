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

#include "exception.h"

#include <QDir>

namespace NMakeFile {

Exception::Exception(const QString& message)
:   m_message(message)
{
}

const QString Exception::toString() const
{
    return m_message;
}

FileException::FileException(const QString& message, const QString& fileName, int line)
:   Exception(message),
    m_fileName(fileName),
    m_line(line)
{
}

const QString FileException::toString() const
{
    QString output = m_message;
    if (!m_fileName.isEmpty()) {
        output += QLatin1String(" in ");
        QString fileName = m_fileName;
        fileName = QDir::toNativeSeparators(fileName);
        output += fileName;
        output += QLatin1String(" ");
    }

    if (m_line > 0) {
        output += QString::fromLatin1("line %1").arg(m_line);
    }

    return output;
}

} // namespace NMakeFile
