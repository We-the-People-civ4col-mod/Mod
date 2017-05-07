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

#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H

#include <QtCore/QString>
#include <QtCore/QStringList>

inline QString fileNameFromFilePath(const QString& filePath)
{
    int idx = qMax(filePath.lastIndexOf(QLatin1Char('/')), filePath.lastIndexOf(QLatin1Char('\\')));
    if (idx == -1)
        return filePath;
    QString fileName = filePath;
    fileName.remove(0, idx+1);
    return fileName;
}

inline bool isSpaceOrTab(const QChar& ch)
{
    return ch == QLatin1Char(' ') || ch == QLatin1Char('\t');
}

inline bool startsWithSpaceOrTab(const QString& str)
{
    Q_ASSERT(!str.isEmpty());
    return isSpaceOrTab(str.at(0));
}

inline void removeDirSeparatorAtEnd(QString& directory)
{
    if (directory.endsWith(QLatin1Char('/')) || directory.endsWith(QLatin1Char('\\')))
        directory.chop(1);
}

inline void removeDoubleQuotes(QString& targetName)
{
    const QChar dblQuote = QLatin1Char('"');
    if (targetName.startsWith(dblQuote) && targetName.endsWith(dblQuote)) {
        targetName.chop(1);
        targetName.remove(0, 1);
    }
}

/**
 * Splits the string, respects "foo bar" and "foo ""knuffi"" bar".
 */
QStringList splitCommandLine(QString commandLine);

/**
 * Returns a copy of s with all whitespace removed from the left.
 */
QString trimLeft(const QString &s);

QString qGetEnvironmentVariable(const wchar_t *lpName);
bool qSetEnvironmentVariable(const QString &name, const QString &value);

#endif // HELPERFUNCTIONS_H
