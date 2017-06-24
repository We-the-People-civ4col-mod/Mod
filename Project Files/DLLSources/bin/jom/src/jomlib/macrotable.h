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

#ifndef MACROTABLE_H
#define MACROTABLE_H

#include "processenvironment.h"

#include <QtCore/QSet>
#include <QtCore/QStringList>

namespace NMakeFile {

class MacroTable
{
public:
    static const QChar fileNameMacroMagicEscape;

    MacroTable();
    ~MacroTable();

    void setEnvironment(const ProcessEnvironment &e) { m_environment = e; }
    const ProcessEnvironment &environment() const { return m_environment; }

    bool isMacroDefined(const QString& name) const;
    bool isMacroNameValid(const QString& name) const;
    QString macroValue(const QString& macroName) const;
    void defineEnvironmentMacroValue(const QString& name, const QString& value, bool readOnly = false);
    void setMacroValue(const QString& name, const QString& value);
    void setMacroValue(const char *szStr, const QString& value) { setMacroValue(QString::fromLatin1(szStr), value); }
    void setMacroValue(const char *szStr, const char *szValue) { setMacroValue(QString::fromLatin1(szStr), QString::fromLatin1(szValue)); }
    void undefineMacro(const QString& name);
    QString expandMacros(const QString& str, bool inDependentsLine = false) const;
    void dump() const;

    struct Substitution
    {
        QString before;
        QString after;
    };

    static Substitution parseSubstitutionStatement(const QString &str, int substitutionStartIdx, int &macroInvokationEndIdx);
    static void applySubstitution(const Substitution &substitution, QString &value);

private:
    struct MacroData
    {
        MacroData()
            : isEnvironmentVariable(false), isReadOnly(false)
        {}

        bool isEnvironmentVariable;
        bool isReadOnly;
        QString value;
    };

    MacroData* internalSetMacroValue(const QString& name, const QString& value);
    void setEnvironmentVariable(const QString& name, const QString& value);
    QString expandMacros(const QString& str, bool inDependentsLine, QSet<QString>& usedMacros) const;
    QString cycleCheckedMacroValue(const QString& macroName, QSet<QString>& usedMacros) const;

    QHash<QString, MacroData>   m_macros;
    ProcessEnvironment          m_environment;
};

} // namespace NMakeFile

#endif // MACROTABLE_H
