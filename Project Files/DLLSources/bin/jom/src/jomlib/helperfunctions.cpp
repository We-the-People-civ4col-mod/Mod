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

#include "helperfunctions.h"
#include <qt_windows.h>

/**
 * Splits the string, respects "foo bar" and "foo ""knuffi"" bar".
 */
QStringList splitCommandLine(QString commandLine)
{
    QString str;
    QStringList arguments;
    commandLine.append(QLatin1Char(' '));   // append artificial space
    bool escapedQuote = false;
    bool insideQuotes = false;
    for (int i=0; i < commandLine.count(); ++i) {
        if (commandLine.at(i).isSpace() && !insideQuotes) {
            escapedQuote = false;
            str = str.trimmed();
            if (!str.isEmpty()) {
                arguments.append(str);
                str.clear();
            }
        } else {
            if (commandLine.at(i) == QLatin1Char('"')) {
                if (escapedQuote)  {
                    str.append(QLatin1Char('"'));
                    escapedQuote = false;
                } else {
                    escapedQuote = true;
                }
                insideQuotes = !insideQuotes;
            } else {
                str.append(commandLine.at(i));
                escapedQuote = false;
            }
        }
    }
    return arguments;
}

QString trimLeft(const QString &s)
{
    QString result = s;
    while (!result.isEmpty() && result[0].isSpace())
        result.remove(0, 1);
    return result;
}

QString qGetEnvironmentVariable(const wchar_t *lpName)
{
    const size_t bufferSize = 32767;
    TCHAR buffer[bufferSize];
    if (GetEnvironmentVariable(lpName, buffer, bufferSize))
        return QString::fromWCharArray(buffer);
    return QString();
}

bool qSetEnvironmentVariable(const QString &name, const QString &value)
{
    return SetEnvironmentVariable(
            reinterpret_cast<const wchar_t *>(name.utf16()),
            reinterpret_cast<const wchar_t *>(value.utf16()));
}
