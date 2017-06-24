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
#include "makefilefactory.h"
#include "macrotable.h"
#include "makefile.h"
#include "options.h"
#include "parser.h"
#include "preprocessor.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>

namespace NMakeFile {

MakefileFactory::MakefileFactory()
:   m_makefile(0),
    m_errorType(NoError)
{
}

void MakefileFactory::setEnvironment(const QStringList &env)
{
    for (QStringList::const_iterator it = env.begin(); it != env.end(); ++it) {
        int idx = it->indexOf(QLatin1Char('='));
        if (idx >= 0)
            m_environment.insert(it->left(idx), it->mid(idx + 1));
    }
}

void MakefileFactory::clear()
{
    m_makefile = 0;
    m_errorType = NoError;
    m_errorString.clear();
    m_activeTargets.clear();
}

static void readEnvironment(const ProcessEnvironment &environment, MacroTable *macroTable, bool forceReadOnly)
{
    ProcessEnvironment::const_iterator it = environment.begin();
    for (; it != environment.end(); ++it)
        macroTable->defineEnvironmentMacroValue(it.key().toQString(), it.value(), forceReadOnly);
}

/**
 * Returns true, if the path contains some whitespace.
 */
static bool isComplexPathName(const QString& path)
{
    for (int i=path.length()-1; i > 0; i--) {
        const QChar ch = path.at(i);
        if (ch.isSpace())
            return true;
    }
    return false;
}

/**
 * Enclose the path in double quotes, if its a complex one.
 */
static QString encloseInDoubleQuotesIfNeeded(const QString& path)
{
    if (isComplexPathName(path)) {
        QString result(QLatin1Char('"'));
        result.append(path);
        result.append(QLatin1Char('"'));
        return result;
    }
    return path;
}

bool MakefileFactory::apply(const QStringList& commandLineArguments, Options **outopt)
{
    if (m_makefile)
        clear();

    Options *options = new Options;
    if (outopt)
        *outopt = options;
    MacroTable *macroTable = new MacroTable;
    macroTable->setEnvironment(m_environment);

    QString filename;
    if (!options->readCommandLineArguments(commandLineArguments, filename, m_activeTargets, *macroTable)) {
        m_errorType = CommandLineError;
        return false;
    }
    if (options->showUsageAndExit || options->showVersionAndExit)
        return true;

    if (!options->stderrFile.isEmpty()) {
        // Try to open the file for writing.
        const wchar_t *wszFileName = reinterpret_cast<const wchar_t*>(options->stderrFile.utf16());
        FILE *f = _wfopen(wszFileName, L"w");
        if (!f) {
            m_errorString = QLatin1String("Cannot open stderr file for writing.");
            m_errorType = IOError;
            return false;
        }
        fclose(f);
        if (!_wfreopen(wszFileName, L"w", stderr)) {
            m_errorString = QLatin1String("Cannot reopen stderr handle for writing.");
            m_errorType = IOError;
            return false;
        }
    }

    options->fullAppPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());

    readEnvironment(m_environment, macroTable, options->overrideEnvVarMacros);
    if (!options->ignorePredefinedRulesAndMacros) {
        macroTable->setMacroValue("MAKE", encloseInDoubleQuotesIfNeeded(options->fullAppPath));
        macroTable->setMacroValue("MAKEDIR", encloseInDoubleQuotesIfNeeded(QDir::currentPath()));
        macroTable->setMacroValue("AS", "ml");       // Macro Assembler
        macroTable->setMacroValue("ASFLAGS", QString());
        macroTable->setMacroValue("BC", "bc");       // Basic Compiler
        macroTable->setMacroValue("BCFLAGS", QString());
        macroTable->setMacroValue("CC", "cl");       // C Compiler
        macroTable->setMacroValue("CCFLAGS", QString());
        macroTable->setMacroValue("COBOL", "cobol"); // COBOL Compiler
        macroTable->setMacroValue("COBOLFLAGS", QString());
        macroTable->setMacroValue("CPP", "cl");      // C++ Compiler
        macroTable->setMacroValue("CPPFLAGS", QString());
        macroTable->setMacroValue("CXX", "cl");      // C++ Compiler
        macroTable->setMacroValue("CXXFLAGS", QString());
        macroTable->setMacroValue("FOR", "fl");      // FORTRAN Compiler
        macroTable->setMacroValue("FORFLAGS", QString());
        macroTable->setMacroValue("PASCAL", "pl");   // Pascal Compiler
        macroTable->setMacroValue("PASCALFLAGS", QString());
        macroTable->setMacroValue("RC", "rc");       // Resource Compiler
        macroTable->setMacroValue("RCFLAGS", QString());
    }

    try {
        m_makefile = new Makefile(filename);
        m_makefile->setOptions(options);
        m_makefile->setMacroTable(macroTable);
        Preprocessor preprocessor;
        preprocessor.setMacroTable(macroTable);
        preprocessor.openFile(filename);
        Parser parser;
        parser.apply(&preprocessor, m_makefile, m_activeTargets);
    } catch (Exception &e) {
        m_errorType = ParserError;
        m_errorString = e.toString();
    }

    return m_errorType == NoError;
}

} //namespace NMakeFile
