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

#ifndef PROCESSENVIRONMENT_H
#define PROCESSENVIRONMENT_H

#include <QtCore/QMap>
#include <QtCore/QString>

namespace NMakeFile {

/**
 * Key for the ProcessEnvironment class.
 */
class ProcessEnvironmentKey
{
public:
    ProcessEnvironmentKey(const QString &key)
        : m_key(key)
    {
    }

    ProcessEnvironmentKey(const QLatin1String &key)
        : m_key(key)
    {
    }

    const QString &toQString() const
    {
        return m_key;
    }

    int compare(const ProcessEnvironmentKey &other) const
    {
        return m_key.compare(other.m_key, Qt::CaseInsensitive);
    }

private:
    QString m_key;
};

inline bool operator < (const ProcessEnvironmentKey &lhs, const ProcessEnvironmentKey &rhs)
{
    return lhs.compare(rhs) < 0;
}

typedef QMap<ProcessEnvironmentKey, QString> ProcessEnvironment;

} // namespace NMakeFile

#endif // PROCESSENVIRONMENT_H
