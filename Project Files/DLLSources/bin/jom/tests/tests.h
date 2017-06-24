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

#ifndef TESTS_H
#define TESTS_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QProcess>

namespace NMakeFile
{
    class Preprocessor;
    class MakefileFactory;
}

class Tests : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    // preprocessor tests
    void includeFiles();
    void includeCycle();
    void macros();
    void invalidMacros_data();
    void invalidMacros();
    void preprocessorExpressions_data();
    void preprocessorExpressions();
    void preprocessorDivideByZero();
    void preprocessorInvalidExpressions_data();
    void preprocessorInvalidExpressions();
    void conditionals();
    void dotDirectives();

    // parser tests
    void descriptionBlocks();
    void inferenceRules_data();
    void inferenceRules();
    void cycleInTargets();
    void dependentsWithSpace();
    void multipleTargets();
    void commandModifiers();
    void comments();
    void fileNameMacros();
    void fileNameMacrosInDependents();
    void wildcardsInDependencies();
    void windowsPathsInTargetName();

    // black-box tests
    void caseInsensitiveDependents();
    void environmentVariables_data();
    void environmentVariables();
    void environmentVariablesCaseInsensitivity();
    void environmentVariablesInCommands();
    void ignoreExitCodes();
    void inlineFiles();
    void unicodeFiles_data();
    void unicodeFiles();
    void builtin_cd_data();
    void builtin_cd();
    void suffixes();
    void nonexistentDependent();
    void outOfDateCheck();

private:
    bool openMakefile(const QString& fileName);
    bool runJom(const QStringList &args, const QString &workingDirectory = QString());
    bool fileContentsEqual(const QString& fileName1, const QString& fileName2);
    QStringList readJomStdOutput();
    void touchFile(const QString &fileName);

private:
    QString m_oldCurrentPath;
    NMakeFile::Preprocessor* m_preprocessor;
    NMakeFile::MakefileFactory* m_makefileFactory;
    QProcess *m_jomProcess;
};

#endif // TESTS_H
