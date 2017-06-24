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

#include "tests.h"

#include <QTest>
#include <QDir>
#include <QScopedPointer>
#include <QDebug>
#include <QStringBuilder>

#include <ppexprparser.h>
#include <makefilefactory.h>
#include <preprocessor.h>
#include <parser.h>
#include <options.h>
#include <exception.h>

using namespace NMakeFile;

void Tests::initTestCase()
{
    m_makefileFactory = new MakefileFactory;
    m_preprocessor = 0;
    m_jomProcess = new QProcess(this);
    m_oldCurrentPath = QDir::currentPath();
    QDir::setCurrent(SRCDIR "makefiles");
}

void Tests::cleanupTestCase()
{
    delete m_makefileFactory;
    delete m_preprocessor;
    QDir::setCurrent(m_oldCurrentPath);
}

bool Tests::openMakefile(const QString& fileName)
{
    return m_makefileFactory->apply(QStringList() << QLatin1String("/F") << fileName);
}

void Tests::includeFiles()
{
    MacroTable macroTable;
    Preprocessor pp;
    pp.setMacroTable(&macroTable);
    bool exceptionCaught = false;
    try {
        QVERIFY( pp.openFile(QLatin1String("include_test.mk")) );
        while (!pp.readLine().isNull());
    } catch (Exception &e) {
        qDebug() << e.message();
        exceptionCaught = true;
    }
    QVERIFY(!exceptionCaught);
    QVERIFY(macroTable.isMacroDefined("INCLUDE"));
    QCOMPARE(macroTable.macroValue("INCLUDE1"), QLatin1String("TRUE"));
    QCOMPARE(macroTable.macroValue("INCLUDE2"), QLatin1String("TRUE"));
    QCOMPARE(macroTable.macroValue("INCLUDE3"), QLatin1String("TRUE"));
    QCOMPARE(macroTable.macroValue("INCLUDE4"), QLatin1String("TRUE"));
    QCOMPARE(macroTable.macroValue("INCLUDE5"), QLatin1String("TRUE"));
    QCOMPARE(macroTable.macroValue("INCLUDE6"), QLatin1String("TRUE"));
    QCOMPARE(macroTable.macroValue("INCLUDE7"), QLatin1String("TRUE"));
}

void Tests::includeCycle()
{
    MacroTable macroTable;
    Preprocessor pp;
    pp.setMacroTable(&macroTable);
    bool bExceptionCaught = false;
    try {
        QVERIFY( pp.openFile(QLatin1String("circular_include.mk")) );
        while (!pp.readLine().isNull());
    } catch (Exception &e) {
        qDebug() << e.message();
        bExceptionCaught = true;
    }
    QVERIFY(bExceptionCaught);
}

void Tests::macros()
{
    MacroTable macroTable;
    Preprocessor pp;
    pp.setMacroTable(&macroTable);
    bool bExceptionCaught = false;
    try {
        QVERIFY( pp.openFile(QLatin1String("macrotest.mk")) );
        while (!pp.readLine().isNull());
        QCOMPARE(macroTable.macroValue("VERY_LONG_Macro_Name_With_mucho_mucho_characters_and_some_number_too_1234458789765421200218427824996512548989654486630110059699471421"), QLatin1String("AHA"));
        QCOMPARE(macroTable.macroValue("SEIN"), QLatin1String("ist"));
        QCOMPARE(macroTable.macroValue("vielipsum_istwallewalle_"), QLatin1String("Icke wa dsch und er denn uurrrgh..."));
        QVERIFY(macroTable.isMacroDefined("NoContent"));
        QCOMPARE(macroTable.macroValue("NoContent"), QLatin1String(""));
        QCOMPARE(macroTable.macroValue("Literal1"), QLatin1String("# who does that anyway? #"));
        QCOMPARE(macroTable.macroValue("Literal2"), QLatin1String("thi$$ i$$ pricele$$$$"));
        QCOMPARE(macroTable.expandMacros("$(Literal2)"), QLatin1String("thi$ i$ pricele$$"));
        QCOMPARE(macroTable.macroValue("Literal3"), QLatin1String("schnupsi\nwupsi\ndupsi"));
        QCOMPARE(macroTable.macroValue("Literal4"), QLatin1String("backslash at the end\\"));
        QCOMPARE(macroTable.macroValue("Literal5"), QLatin1String("backslash at the end\\"));
        QCOMPARE(macroTable.macroValue("Literal6"), QLatin1String("backslash at the end\\"));
        QCOMPARE(macroTable.macroValue("SplitOverLines"), QLatin1String("one  two three"));
        QCOMPARE(macroTable.macroValue("Incremental"), QLatin1String("one two"));
        QCOMPARE(macroTable.expandMacros(macroTable.macroValue("LateDefinition")),
                 QLatin1String("_thi$ i$ pricele$$_"));
        QCOMPARE(macroTable.expandMacros("$B"), QLatin1String("B"));
        QCOMPARE(macroTable.expandMacros("$y"), QLatin1String("y"));
        QCOMPARE(macroTable.expandMacros("$(BANANA)"), QLatin1String("yellow"));
        QCOMPARE(macroTable.expandMacros("$1"), QLatin1String("x"));
        QCOMPARE(macroTable.expandMacros("$(XXX)"), QLatin1String("xxx"));
        QCOMPARE(macroTable.expandMacros("dollar at the end$"), QLatin1String("dollar at the end$"));
        QVERIFY(!macroTable.isMacroDefined("ThisIsNotDefined"));
        QCOMPARE(macroTable.expandMacros("$(MACROSUBSTITUTION:not=perfectly)"), QLatin1String("is perfectly working"));
        QCOMPARE(macroTable.expandMacros("$(MACROSUBSTITUTION: not = of course )"), QLatin1String("is of course working"));
        QCOMPARE(macroTable.expandMacros("$(MACROSUBSTITUTION:not=(properly^))"), QLatin1String("is (properly) working"));
        QCOMPARE(macroTable.expandMacros("$(MKSPECDIR:root_dir=C:\\Qt\\4.7.0)"), QLatin1String("C:\\Qt\\4.7.0\\mkspecs"));
        QCOMPARE(macroTable.macroValue("_STARTING_WITH_AN_UNDERSCORE"), QLatin1String("underscores are beautiful"));
        QCOMPARE(macroTable.macroValue("__STARTING_WITH_TWO_UNDERSCORES"), QLatin1String("underscores are beautiful"));
    } catch (Exception &e) {
        qDebug() << e.message();
        bExceptionCaught = true;
    }
    QVERIFY(!bExceptionCaught);
}

void Tests::invalidMacros_data()
{
    QTest::addColumn<QString>("expression");
    QTest::newRow("missing paranthesis")    << QString("$(FOO) $(MISSING_PARANTHESIS");
    QTest::newRow("empty macro name 1") << QString("empty macro name $()");
    QTest::newRow("empty macro name 2") << QString("empty $() macro name");
}

void Tests::invalidMacros()
{
    QFETCH(QString, expression);
    bool exceptionCaught = false;
    MacroTable macroTable;
    try {
        macroTable.expandMacros(expression);
    } catch (Exception &e) {
        Q_UNUSED(e);
        exceptionCaught = true;
    }
    QVERIFY(exceptionCaught);
}

void Tests::preprocessorExpressions_data()
{
    QTest::addColumn<QByteArray>("expression");
    QTest::addColumn<int>("expected");
    QTest::newRow("max number") << QByteArray("2147483647") << 2147483647;
    QTest::newRow("min number") << QByteArray("-2147483647") << -2147483647;
    QTest::newRow("file exists 1") << QByteArray("EXIST(include_test.mk)") << 1;
    QTest::newRow("file exists 2") << QByteArray("eXiSt( \"include_test.mk\" )") << 1;
    QTest::newRow("file exists 3") << QByteArray("EXIST  ( include_test.mk\t)") << 1;
    QTest::newRow("file not exists") << QByteArray("EXIST(\"no such file\")") << 0;
    QTest::newRow("macro defined 1") << QByteArray("DEFINED ( ThisIsDefined\t)") << 1;
    QTest::newRow("macro defined 2") << QByteArray("DeFiNeD(\"ThisIsDefined\")") << 1;
    QTest::newRow("macro defined 3") << QByteArray("DEFINED\t(ThisIsDefinedButEmpty)") << 1;
    QTest::newRow("macro defined 4") << QByteArray("defined (   ThisIsUnfortunatelyNotDefined    )") << 0;
    QTest::newRow("shellcommand") << QByteArray("[ cmd /c exit 12 ]") << 12;
    QTest::newRow("ops +*") << QByteArray("2+3*5") << 17;
    QTest::newRow("ops (+)*") << QByteArray("(2+3)*5") << 25;
    QTest::newRow("op !") << QByteArray("!0") << 1;
    QTest::newRow("op !") << QByteArray("!(123)") << 0;
    QTest::newRow("op ~") << QByteArray("~(-1)") << 0;
    QTest::newRow("op ~") << QByteArray("~0") << -1;
    QTest::newRow("op -") << QByteArray("-156") << -156;
    QTest::newRow("op %") << QByteArray("156 % 7") << 2;
    QTest::newRow("op /") << QByteArray("156 / 12") << 13;
    QTest::newRow("op *") << QByteArray("12 * 13") << 156;
    QTest::newRow("op <<") << QByteArray("12 << 1") << 24;
    QTest::newRow("op >>") << QByteArray("12 >> 2") << 3;
    QTest::newRow("a > b") << QByteArray("1 > 0") << 1;
    QTest::newRow("a > b") << QByteArray("1 > 1") << 0;
    QTest::newRow("a >= b") << QByteArray("2 >= 1") << 1;
    QTest::newRow("a >= b") << QByteArray("1 >= 1") << 1;
    QTest::newRow("a >= b") << QByteArray("1 >= 2") << 0;
    QTest::newRow("a < b") << QByteArray("1 < 2") << 1;
    QTest::newRow("a < b") << QByteArray("1 < 1") << 0;
    QTest::newRow("a <= b") << QByteArray("1 <= 1") << 1;
    QTest::newRow("a <= b") << QByteArray("1 <= 2") << 1;
    QTest::newRow("a <= b") << QByteArray("2 <= 1") << 0;
    QTest::newRow("a == b") << QByteArray("156 == 156") << 1;
    QTest::newRow("a == b") << QByteArray("156 == 157") << 0;
    QTest::newRow("a != b") << QByteArray("156 != 156") << 0;
    QTest::newRow("a != b") << QByteArray("156 != 157") << 1;
    QTest::newRow("a && b") << QByteArray("156 && 157") << 1;
    QTest::newRow("a && b") << QByteArray("(156-156) && 157") << 0;
    QTest::newRow("a || b") << QByteArray("156 || 157") << 1;
    QTest::newRow("a || b") << QByteArray("(156-156) || 157") << 1;
    QTest::newRow("a || b") << QByteArray("(156-156) || 0") << 0;
    QTest::newRow("a & b") << QByteArray("156 & 12") << 12;
    QTest::newRow("a | b") << QByteArray("156 | 1") << 157;
    QTest::newRow("1+-1") << QByteArray("1+-1") << 0;
    QTest::newRow("string equality 1") << QByteArray("\"string one\" == \"string one\"") << 1;
    QTest::newRow("string equality 2") << QByteArray("\"one \"\" two\" == \"one \"\" two\"") << 1;
    QTest::newRow("string inquality") << QByteArray("\"one two\" != \"two one\"") << 1;
    QTest::newRow("string number mix") << QByteArray("(\"foo\" != \"bar\") == 1") << 1;
    QTest::newRow("macros in qmake Makefile") << QByteArray("\"$(QMAKESPEC)\" == \"win32-msvc\" || \"$(QMAKESPEC)\" == \"win32-msvc.net\" || \"$(QMAKESPEC)\" == \"win32-msvc2002\" || \"$(QMAKESPEC)\" == \"win32-msvc2003\" || \"$(QMAKESPEC)\" == \"win32-msvc2005\" || \"$(QMAKESPEC)\" == \"win32-msvc2008\" || \"$(QMAKESPEC)\" == \"win32-icc\"") << 1;
    QTest::newRow("pretty random test") << QByteArray("1 == 1 || 2 == 2 + 1 + 1 + -1|| 3 == 3") << 1;
}

void Tests::preprocessorExpressions()
{
    if (!m_preprocessor)
        m_preprocessor = new Preprocessor;

    MacroTable* macroTable = 0;
    QByteArray cdt(QTest::currentDataTag());
    if (cdt.startsWith("macro")) {
        macroTable = new MacroTable;
        macroTable->setMacroValue("ThisIsDefined", "yes");
        macroTable->setMacroValue("ThisIsDefinedButEmpty", QString());
        macroTable->setMacroValue("QMAKESPEC", "win32-msvc2008");
        m_preprocessor->setMacroTable(macroTable);
    }

    QFETCH(QByteArray, expression);
    QFETCH(int, expected);
    bool success = true;
    int expressionValue = -1;
    try {
        expressionValue = m_preprocessor->evaluateExpression(QString::fromLocal8Bit(expression.data()));
    } catch (...) {
        success = false;
    }
    QVERIFY(success);
    QCOMPARE(expressionValue, expected);

    if (macroTable) {
        m_preprocessor->setMacroTable(0);
        delete macroTable;
    }
}

void Tests::preprocessorDivideByZero()
{
    if (!m_preprocessor)
        m_preprocessor = new Preprocessor;

    NMakeFile::Exception error;
    bool exceptionCaught = false;
    try {
        m_preprocessor->evaluateExpression("1 / (156-156)");
    } catch (NMakeFile::Exception &e) {
        exceptionCaught = true;
        error = e;
    }
    QCOMPARE(exceptionCaught, true);
    QVERIFY(error.message().contains("division by zero"));
}

void Tests::preprocessorInvalidExpressions_data()
{
     QTest::addColumn<QByteArray>("expression");
     QTest::newRow("empty") << QByteArray("");
     QTest::newRow("invalid characters") << QByteArray("\0x01\0x02\0x03");
     QTest::newRow("missing (") << QByteArray("1 + 1)");
     QTest::newRow("missing )") << QByteArray("(1 + 1");
     QTest::newRow("double op") << QByteArray("1++1");
     QTest::newRow("string == number") << QByteArray("\"foo\" == 156");
     //QTest::newRow("") << QByteArray("");
}

void Tests::preprocessorInvalidExpressions()
{
    if (!m_preprocessor)
        m_preprocessor = new Preprocessor;

    QFETCH(QByteArray, expression);
    NMakeFile::Exception error;
    bool exceptionCaught = false;
    try {
        m_preprocessor->evaluateExpression(QString::fromLocal8Bit(expression));
    } catch (NMakeFile::Exception &e) {
        error = e;
        exceptionCaught = true;
    }

    QVERIFY(exceptionCaught);
    QVERIFY(!error.message().isEmpty());
}

void Tests::conditionals()
{
    QEXPECT_FAIL("", "QTCREATORBUG-8621", Continue);
    QVERIFY( openMakefile(QLatin1String("conditionals.mk")) );
    QScopedPointer<Makefile> mkfile(m_makefileFactory->makefile());
    QVERIFY(mkfile);

    const MacroTable *macroTable = mkfile->macroTable();
    QVERIFY(macroTable);
    QCOMPARE(macroTable->macroValue("TEST1"), QLatin1String("true"));
    QCOMPARE(macroTable->macroValue("TEST2"), QLatin1String("true"));
    QCOMPARE(macroTable->macroValue("TEST3"), QLatin1String("true"));
    QCOMPARE(macroTable->macroValue("TEST4"), QLatin1String("true"));
    QCOMPARE(macroTable->macroValue("TEST5"), QLatin1String("true"));
    QCOMPARE(macroTable->macroValue("TEST6"), QLatin1String("true"));
    QCOMPARE(macroTable->macroValue("TEST7"), QLatin1String("true"));
    QCOMPARE(macroTable->macroValue("TEST8"), QLatin1String("true"));
    QEXPECT_FAIL("", "QTCREATORBUG-8621", Continue);
    QCOMPARE(macroTable->macroValue("TEST9"), QLatin1String("foo  bar  baz"));
    QEXPECT_FAIL("", "QTCREATORBUG-8621", Continue);
    QCOMPARE(macroTable->macroValue("TEST10"), QLatin1String("foo  bar  boo  hoo"));
}

void Tests::dotDirectives()
{
    QVERIFY( openMakefile(QLatin1String("dotdirectives.mk")) );
    QScopedPointer<Makefile> mkfile(m_makefileFactory->makefile());
    QVERIFY(mkfile);

    DescriptionBlock* target;
    Command cmd;

    target = mkfile->target(QLatin1String("silence_one"));
    QVERIFY(target != 0);
    QCOMPARE(target->m_commands.count(), 1);
    cmd = target->m_commands.takeFirst();
    QCOMPARE(cmd.m_silent, false);

    target = mkfile->target(QLatin1String("silence_two"));
    QVERIFY(target != 0);
    QCOMPARE(target->m_commands.count(), 1);
    cmd = target->m_commands.takeFirst();
    QCOMPARE(cmd.m_silent, true);

    target = mkfile->target(QLatin1String("silence_three"));
    QVERIFY(target != 0);
    QCOMPARE(target->m_commands.count(), 1);
    cmd = target->m_commands.takeFirst();
    //QCOMPARE(cmd.m_silent, false);    // TODO: implement !CMDSWITCHES

    target = mkfile->target(QLatin1String("ignorance_one"));
    QVERIFY(target != 0);
    QCOMPARE(target->m_commands.count(), 1);
    cmd = target->m_commands.takeFirst();
    QCOMPARE(int(cmd.m_maxExitCode), 0);

    target = mkfile->target(QLatin1String("ignorance_two"));
    QVERIFY(target != 0);
    QCOMPARE(target->m_commands.count(), 2);
    cmd = target->m_commands.takeFirst();
    QCOMPARE(int(cmd.m_maxExitCode), 255);

    target = mkfile->target(QLatin1String("ignorance_three"));
    QVERIFY(target != 0);
    QCOMPARE(target->m_commands.count(), 1);
    cmd = target->m_commands.takeFirst();
    //QCOMPARE(int(cmd.m_maxExitCode), 0);   // TODO: implement !CMDSWITCHES

    QCOMPARE(mkfile->preciousTargets().count(), 3);
    QCOMPARE(mkfile->preciousTargets().at(0), QLatin1String("preciousness_one"));
    QCOMPARE(mkfile->preciousTargets().at(1), QLatin1String("preciousness_two"));
    QCOMPARE(mkfile->preciousTargets().at(2), QLatin1String("preciousness_three"));
}

void Tests::descriptionBlocks()
{
    QVERIFY( openMakefile(QLatin1String("descriptionBlocks.mk")) );
    QScopedPointer<Makefile> mkfile(m_makefileFactory->makefile());
    QVERIFY(mkfile);

    DescriptionBlock* target = mkfile->target("one");
    QVERIFY(target != 0);
    QCOMPARE(target->m_dependents.count(), 3);
    QCOMPARE(target->m_dependents.at(0), QLatin1String("a"));
    QCOMPARE(target->m_dependents.at(1), QLatin1String("b"));
    QCOMPARE(target->m_dependents.at(2), QLatin1String("c"));
    QCOMPARE(target->m_commands.count(), 1);
    
    Command cmd = target->m_commands.first();
    QCOMPARE(cmd.m_commandLine, QLatin1String("echo one"));

    target = mkfile->target("two");
    QVERIFY(target != 0);
    QCOMPARE(target->m_dependents.count(), 0);
    QCOMPARE(target->m_commands.count(), 1);
    
    cmd = target->m_commands.first();
    QCOMPARE(cmd.m_commandLine, QLatin1String("echo two"));

    target = mkfile->target("three");
    QVERIFY(target != 0);
    QCOMPARE(target->m_dependents.count(), 1);
    QCOMPARE(target->m_commands.count(), 1);
    
    cmd = target->m_commands.first();
    QCOMPARE(cmd.m_commandLine, QLatin1String("echo three; @echo end of three"));

    target = mkfile->target("four");
    QVERIFY(target);
    QCOMPARE(target->m_dependents.count(), 0);
    QCOMPARE(target->m_commands.count(), 1);

    cmd = target->m_commands.first();
    QCOMPARE(cmd.m_commandLine, QLatin1String("echo four=4"));

    target = mkfile->target(".");
    QVERIFY(target != 0);
    QCOMPARE(target->m_dependents.count(), 0);
    QCOMPARE(target->m_commands.count(), 1);
    cmd = target->m_commands.first();
    QCOMPARE(cmd.m_commandLine, QLatin1String("echo directory . doesn't exist. That's strange."));

    target = mkfile->target("..");
    QVERIFY(target != 0);
    QCOMPARE(target->m_dependents.count(), 0);
    QCOMPARE(target->m_commands.count(), 1);
    cmd = target->m_commands.first();
    QCOMPARE(cmd.m_commandLine, QLatin1String("echo directory .. doesn't exist. That's strange."));

    target = mkfile->target(QLatin1String("dollarSigns"));
    QVERIFY(target != 0);
    QCOMPARE(target->m_dependents.count(), 0);
    QCOMPARE(target->m_commands.count(), 2);
    cmd = target->m_commands.takeFirst();
    QCOMPARE(cmd.m_commandLine, QLatin1String("echo ($dollar-signs$)"));
    cmd = target->m_commands.first();
    QCOMPARE(cmd.m_commandLine, QLatin1String("echo $(dollar-signs)$"));

    target = mkfile->target(QLatin1String("SubstitutedTargetName"));
    QVERIFY(target != 0);
    QCOMPARE(target->m_dependents.count(), 3);
    QCOMPARE(target->m_commands.count(), 1);
}

// inferenceRules test mode
static const char IRTM_Init = 0;
static const char IRTM_Cleanup = 1;
static const char IRTM_ParseTimeRule = 2;
static const char IRTM_DeferredRule = 3;

void Tests::inferenceRules_data()
{
    QTest::addColumn<char>("mode");
    QTest::addColumn<QString>("targetName");
    QTest::addColumn<QString>("expectedCommandLine");
    QTest::addColumn<QString>("fileToCreate");

    QTest::newRow("init") << IRTM_Init << "" << "" << "";
    QTest::newRow("1") << IRTM_ParseTimeRule << "foo1.obj" << "echo {subdir}.cpp.obj (subdir\\foo1.cpp)" << "";
    QTest::newRow("2") << IRTM_ParseTimeRule << "foo2.obj" << "echo {subdir}.cpp.obj (subdir\\foo2.cpp)" << "";
    QTest::newRow("3") << IRTM_ParseTimeRule << "foo3.obj" << "echo .cpp.obj (foo3.cpp)" << "";
    QTest::newRow("4") << IRTM_ParseTimeRule << "foo4.obj" << "echo {subdir}.cpp.obj (subdir\\foo4.cpp)" << "";
    QTest::newRow("5") << IRTM_DeferredRule << "foo5.obj" << "echo {subdir}.cpp.obj (subdir\\foo5.cpp)" << "subdir\\foo5.cpp";
    QTest::newRow("6") << IRTM_DeferredRule << "foo6.obj" << "echo .cpp.obj (foo6.cpp)" << "foo6.cpp";
    QTest::newRow("7") << IRTM_DeferredRule << "foo.bar.obj" << "echo .cpp.obj (foo.bar.cpp)" << "foo.bar.cpp";
    QTest::newRow("cleanup") << IRTM_Cleanup << "" << "" << "";

    QStringList filesToCreate;
    filesToCreate << "subdir\\foo5.cpp" << "foo6.cpp";
    foreach (const QString& fileName, filesToCreate)
        if (QFile::exists(fileName))
            system("del " + fileName.toLocal8Bit());
}

void Tests::inferenceRules()
{
    static QScopedPointer<Makefile> mkfile;

    QFETCH(char, mode);
    QFETCH(QString, targetName);
    QFETCH(QString, expectedCommandLine);
    QFETCH(QString, fileToCreate);

    switch (mode) {
        case IRTM_Init: // init
            QVERIFY( openMakefile(QLatin1String("infrules.mk")) );
            mkfile.reset(m_makefileFactory->makefile());
            QVERIFY(mkfile);
            return;
        case IRTM_Cleanup: // cleanup
            mkfile.reset();
            return;
    }

    QVERIFY(mkfile);
    DescriptionBlock* target = mkfile->target(targetName);
    QVERIFY(target);
    mkfile->applyInferenceRules(QList<DescriptionBlock*>() << target);
    QVERIFY(target);
    if (mode == IRTM_DeferredRule) {
        QVERIFY(target->m_commands.count() == 0);
        system("echo.>" + fileToCreate.toLocal8Bit());
        QVERIFY(QFile::exists(fileToCreate));
        mkfile->applyInferenceRules(QList<DescriptionBlock*>() << target);
        system("del " + fileToCreate.toLocal8Bit());
        QVERIFY(!QFile::exists(fileToCreate));
    }
    QCOMPARE(target->m_commands.count(), 1);
    QCOMPARE(target->m_commands.first().m_commandLine, expectedCommandLine);
}

void Tests::cycleInTargets()
{
    MacroTable *macroTable = new MacroTable;
    Makefile mkfile(QLatin1String("cycle_in_targets.mk"));
    mkfile.setOptions(new Options);
    mkfile.setMacroTable(macroTable);
    Preprocessor pp;
    Parser parser;
    pp.setMacroTable(macroTable);

    bool exceptionThrown = false;
    try {
        QVERIFY( pp.openFile(QLatin1String("cycle_in_targets.mk")) );
        parser.apply(&pp, &mkfile);
    } catch (...) {
        exceptionThrown = true;
    }
    QVERIFY(exceptionThrown);
}

void Tests::dependentsWithSpace()
{
    QVERIFY( openMakefile(QLatin1String("depswithspace.mk")) );
    QScopedPointer<Makefile> mkfile(m_makefileFactory->makefile());
    QVERIFY(mkfile);
    DescriptionBlock* target = mkfile->target("first");
    QVERIFY(target);
    QCOMPARE(target->m_dependents.count(), 3);
    QCOMPARE(target->m_dependents.at(0), QLatin1String("one"));
    QCOMPARE(target->m_dependents.at(1), QLatin1String("dependent two with spaces"));
    QCOMPARE(target->m_dependents.at(2), QLatin1String("three"));
}

void Tests::multipleTargets()
{
    QVERIFY( openMakefile(QLatin1String("targetmultidef.mk")) );
    QScopedPointer<Makefile> mkfile(m_makefileFactory->makefile());
    QVERIFY(mkfile);
    DescriptionBlock* target = mkfile->target("foo");
    QVERIFY(target);
    QCOMPARE(target->m_dependents.count(), 3);
    QVERIFY(target->m_dependents.contains("foo1.cpp"));
    QVERIFY(target->m_dependents.contains("foo3.cpp"));
    QVERIFY(target->m_dependents.contains("foo4.cpp"));
    QCOMPARE(target->m_commands.count(), 1);

    target = mkfile->target("bar");
    QVERIFY(target);
    QCOMPARE(target->m_dependents.count(), 3);
    QVERIFY(target->m_dependents.contains("foo1.cpp"));
    QVERIFY(target->m_dependents.contains("foo3.cpp"));
    QVERIFY(target->m_dependents.contains("foo4.cpp"));
    QCOMPARE(target->m_commands.count(), 3);
}

void Tests::commandModifiers()
{
    QVERIFY( openMakefile(QLatin1String("commandmodifiers.mk")) );
    QScopedPointer<Makefile> mkfile(m_makefileFactory->makefile());
    QVERIFY(mkfile);

    DescriptionBlock* target = mkfile->target("first");
    QVERIFY(target);
    QCOMPARE(target->m_commands.count(), 5);
    Command cmd = target->m_commands.at(0);
    QCOMPARE(cmd.m_silent, true);

    cmd = target->m_commands.at(1);
    QCOMPARE((int)cmd.m_maxExitCode, 255);

    cmd = target->m_commands.at(2);
    QCOMPARE((int)cmd.m_maxExitCode, 5);

    cmd = target->m_commands.at(3);
    QCOMPARE((int)cmd.m_maxExitCode, 15);

    cmd = target->m_commands.at(4);
    QCOMPARE(cmd.m_singleExecution, true);
}

void Tests::comments()
{
    QVERIFY( openMakefile(QLatin1String("comments.mk")) );
    QScopedPointer<Makefile> mkfile(m_makefileFactory->makefile());
    QVERIFY(mkfile);
    QCOMPARE(mkfile->macroTable()->macroValue("COMPILER"), QLatin1String("Ada95"));
    QCOMPARE(mkfile->macroTable()->macroValue("DEF"), QLatin1String("#define"));

    DescriptionBlock* target = mkfile->target("first");
    QVERIFY(target);
    QCOMPARE(target->m_dependents.count(), 2);
    QCOMPARE(target->m_commands.count(), 2);
    
    Command cmd1 = target->m_commands.at(0);
    Command cmd2 = target->m_commands.at(1);
    QCOMPARE(cmd1.m_commandLine, QLatin1String("echo I'm Winneone"));
    QCOMPARE(cmd2.m_commandLine, QLatin1String("echo I'm Winnetou"));

    target = mkfile->target("forth");
    QVERIFY(target != 0);
    QCOMPARE(target->m_dependents.count(), 1);
    QCOMPARE(target->m_commands.count(), 2);
    cmd1 = target->m_commands.at(0);
    cmd2 = target->m_commands.at(1);
    QCOMPARE(cmd1.m_commandLine, QLatin1String("echo # this is no comment"));
    QCOMPARE(cmd2.m_commandLine, QLatin1String("echo # this neither"));

    target = mkfile->target("fifth");
    QVERIFY(target != 0);
    QCOMPARE(target->m_dependents.count(), 1);
    QCOMPARE(target->m_dependents.first(), QLatin1String("file#99.txt"));
    QCOMPARE(target->m_commands.count(), 1);
}

void Tests::fileNameMacros()
{
    bool ok = m_makefileFactory->apply(QStringList() << "MAKEDIR=" + QDir::currentPath()
                                                     << "/f" << QLatin1String("filenamemacros.mk"));
    QVERIFY(ok);
    QScopedPointer<Makefile> mkfile(m_makefileFactory->makefile());
    QVERIFY(mkfile);

    DescriptionBlock* target;
    Command command;
    target = mkfile->target(QLatin1String("all"));
    QVERIFY(target);
    QVERIFY(target->m_dependents.contains("Football"));

    target = mkfile->target(QLatin1String("Football"));
    QVERIFY(target);
    target->expandFileNameMacros();
    QCOMPARE(target->m_commands.count(), 6);
    command = target->m_commands.takeFirst();
    QCOMPARE(command.m_commandLine, QLatin1String("echo $@"));
    command = target->m_commands.takeFirst();
    QCOMPARE(command.m_commandLine, QLatin1String("echo Football Football"));
    command = target->m_commands.takeFirst();
    QCOMPARE(command.m_commandLine, QLatin1String("echo $** "));
    command = target->m_commands.takeFirst();
    QCOMPARE(command.m_commandLine, QLatin1String("echo $? "));
    command = target->m_commands.takeFirst();
    QCOMPARE(command.m_commandLine, QLatin1String("echo $(**) "));
    command = target->m_commands.takeFirst();
    QCOMPARE(command.m_commandLine, QLatin1String("echo $(?) "));

    target = mkfile->target(QLatin1String("LolCatExtractorManager.tar.gz"));
    QVERIFY(target);
    target->expandFileNameMacros();
    QVERIFY(!target->m_commands.isEmpty());
    command = target->m_commands.first();
    QCOMPARE(command.m_commandLine, QLatin1String("echo LolCatExtractorManager.tar"));

    target = mkfile->target(QLatin1String("manyDependents"));
    QVERIFY(target);
    target->expandFileNameMacros();
    QVERIFY(!target->m_commands.isEmpty());
    command = target->m_commands.takeFirst();
    QCOMPARE(command.m_commandLine, QLatin1String("echo Timmy Jimmy Kenny Eric Kyle Stan"));
    QVERIFY(!target->m_commands.isEmpty());
    command = target->m_commands.takeFirst();
    QCOMPARE(command.m_commandLine, QLatin1String("echo Timmy Jimmy Kenny Eric Kyle Stan"));

    target = mkfile->target(QLatin1String("manyDependentsSingleExecution"));
    QVERIFY(target);
    target->expandFileNameMacros();
    QCOMPARE(target->m_commands.size(), 12);
    QStringList lst = QStringList() << "Timmy" << "Jimmy" << "Kenny" << "Eric" << "Kyle" << "Stan";
    lst.append(lst);
    foreach (const QString& str, lst) {
        QVERIFY(!target->m_commands.isEmpty());
        command = target->m_commands.takeFirst();
        QCOMPARE(command.m_commandLine, QString(QLatin1String("echo ") + str));
    }

    target = mkfile->target(QLatin1String("manyDependentsSubstitutedNames"));
    QVERIFY(target);
    target->expandFileNameMacros();
    QCOMPARE(target->m_commands.size(), 4);
    command = target->m_commands.takeFirst();
    QCOMPARE(command.m_commandLine, QLatin1String("echo $(**) Tilly Jilly"));
    command = target->m_commands.takeFirst();
    QCOMPARE(command.m_commandLine, QLatin1String("echo $(?) Tilly Jilly"));
    command = target->m_commands.takeFirst();
    QCOMPARE(command.m_commandLine, QLatin1String("echo $(**:ll=mm) Timmy Jimmy"));
    command = target->m_commands.takeFirst();
    QCOMPARE(command.m_commandLine, QLatin1String("echo $(?:ll=mm) Timmy Jimmy"));

    target = mkfile->target(QLatin1String("manyDependentsInlineFile"));
    QVERIFY(target);
    target->expandFileNameMacros();
    QCOMPARE(target->m_commands.size(), 1);
    command = target->m_commands.first();
    QCOMPARE(command.m_inlineFiles.count(), 1);
    InlineFile *inlineFile = command.m_inlineFiles.first();
    QStringList content = inlineFile->m_content.split("\r\n", QString::SkipEmptyParts);
    QCOMPARE(content.count(), 2);
    QCOMPARE(content.at(0), QLatin1String("$@ manyDependentsInlineFile"));
    QCOMPARE(content.at(1), QLatin1String("$** Timmy Jimmy Kenny Eric Kyle Stan"));

    target = mkfile->target(QLatin1String("manyDependentsWithModifiers"));
    QVERIFY(target);
    target->expandFileNameMacros();
    QVERIFY(!target->m_commands.isEmpty());
    command = target->m_commands.takeFirst();
    QCOMPARE(command.m_commandLine, QLatin1String("echo $(**D) subdir subdir\\subsubdir . . . ."));
    QVERIFY(!target->m_commands.isEmpty());
    command = target->m_commands.takeFirst();
    QCOMPARE(command.m_commandLine, QLatin1String("echo $(**B) Timmy Jimmy Kenny Eric Kyle Stan"));
    QVERIFY(!target->m_commands.isEmpty());
    command = target->m_commands.takeFirst();
    QCOMPARE(command.m_commandLine, QLatin1String("echo $(**F) Timmy.txt Jimmy.txt Kenny.txt Eric.txt Kyle.txt Stan.txt"));
    QVERIFY(!target->m_commands.isEmpty());
    command = target->m_commands.takeFirst();
    QCOMPARE(command.m_commandLine, QLatin1String("echo $(**R) subdir\\Timmy subdir\\subsubdir\\Jimmy Kenny Eric Kyle Stan"));
    command = target->m_commands.takeFirst();
    QCOMPARE(command.m_commandLine, QLatin1String("echo $(?D) subdir subdir\\subsubdir . . . ."));
    QVERIFY(!target->m_commands.isEmpty());
    command = target->m_commands.takeFirst();
    QCOMPARE(command.m_commandLine, QLatin1String("echo $(?B) Timmy Jimmy Kenny Eric Kyle Stan"));
    QVERIFY(!target->m_commands.isEmpty());
    command = target->m_commands.takeFirst();
    QCOMPARE(command.m_commandLine, QLatin1String("echo $(?F) Timmy.txt Jimmy.txt Kenny.txt Eric.txt Kyle.txt Stan.txt"));
    QVERIFY(!target->m_commands.isEmpty());
    command = target->m_commands.takeFirst();
    QCOMPARE(command.m_commandLine, QLatin1String("echo $(?R) subdir\\Timmy subdir\\subsubdir\\Jimmy Kenny Eric Kyle Stan"));

    system("del generated.txt gen1.txt gen2.txt gen3.txt > NUL 2>&1");
    target = mkfile->target(QLatin1String("gen_init"));
    QVERIFY(target);
    QVERIFY(!target->m_commands.isEmpty());
    foreach (const Command& cmd, target->m_commands)
        system(qPrintable(cmd.m_commandLine));

    target = mkfile->target(QLatin1String("generated.txt"));
    QVERIFY(target);
    target->expandFileNameMacros();
    QVERIFY(!target->m_commands.isEmpty());
    command = target->m_commands.first();
    QCOMPARE(command.m_commandLine, QLatin1String("echo gen2.txt gen3.txt"));

    target = mkfile->target(QLatin1String("gen_cleanup"));
    QVERIFY(target);
    QVERIFY(!target->m_commands.isEmpty());
    foreach (const Command& cmd, target->m_commands)
        system(qPrintable(cmd.m_commandLine));

    target = mkfile->target(QLatin1String("macros.mk"));
    QVERIFY(target);
    target->expandFileNameMacros();
    QCOMPARE(target->m_commands.count(), 4);
    command = target->m_commands.at(0);
    QCOMPARE(command.m_commandLine, QLatin1String("echo $(@D) ."));
    command = target->m_commands.at(1);
    QCOMPARE(command.m_commandLine, QLatin1String("echo $(@B) macros"));
    command = target->m_commands.at(2);
    QCOMPARE(command.m_commandLine, QLatin1String("echo $(@F) macros.mk"));
    command = target->m_commands.at(3);
    QCOMPARE(command.m_commandLine, QLatin1String("echo $(@R) macros"));

    const QString currentPath = QDir::currentPath().replace('/', '\\');
    target = mkfile->target(currentPath + QLatin1String("\\infrules.mk"));
    QVERIFY(target);
    target->expandFileNameMacros();
    QCOMPARE(target->m_commands.count(), 4);
    command = target->m_commands.at(0);
    QCOMPARE(command.m_commandLine, QString(QLatin1String("echo $(@D) ") + currentPath));
    command = target->m_commands.at(1);
    QCOMPARE(command.m_commandLine, QLatin1String("echo $(@B) infrules"));
    command = target->m_commands.at(2);
    QCOMPARE(command.m_commandLine, QLatin1String("echo $(@F) infrules.mk"));
    command = target->m_commands.at(3);
    QCOMPARE(command.m_commandLine, QString(QLatin1String("echo $(@R) ") + currentPath + QLatin1String("\\infrules")));

    target = mkfile->target("root_dir\\substitutionWithColon");
    QVERIFY(target);
    target->expandFileNameMacros();
    QCOMPARE(target->m_commands.count(), 2);
    command = target->m_commands.at(0);
    QCOMPARE(command.m_commandLine, QLatin1String("echo C:\\somewhere\\substitutionWithColon"));
    command = target->m_commands.at(1);
    QCOMPARE(command.m_commandLine, QLatin1String("echo C:\\somewhere\\substitutionWithColon"));
}

void Tests::fileNameMacrosInDependents()
{
    QVERIFY( openMakefile(QLatin1String("fileNameMacrosInDependents.mk")) );
    QScopedPointer<Makefile> mkfile(m_makefileFactory->makefile());
    QVERIFY(mkfile);

    DescriptionBlock* target;
    target = mkfile->target(QLatin1String("foo"));
    QVERIFY(target);
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("foo"));                               // $@
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("foo"));                               // $$@
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("foo"));                               // $*
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("."));                                 // $(@D)
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("."));                                 // $$(@D)
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("."));                                 // $(*D)
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("foo"));                               // $(@B)
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("foo"));                               // $$(@B)
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("foo"));                               // $(*B)
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("foo"));                               // $(@F)
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("foo"));                               // $$(@F)
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("foo"));                               // $(*F)
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("foo"));                               // $(@R)
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("foo"));                               // $$(@R)
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("foo"));                               // $(*R)

    target = mkfile->target(QLatin1String("foo.obj"));
    QVERIFY(target);
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("foo.obj"));                           // $@
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("foo.obj"));                           // $$@
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("foo"));                               // $*
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("."));                                 // $(@D)
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("."));                                 // $$(@D)
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("."));                                 // $(*D)
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("foo"));                               // $(@B)
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("foo"));                               // $$(@B)
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("foo"));                               // $(*B)
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("foo.obj"));                           // $(@F)
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("foo.obj"));                           // $$(@F)
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("foo"));                               // $(*F)
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("foo"));                               // $(@R)
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("foo"));                               // $$(@R)
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("foo"));                               // $(*R)

    target = mkfile->target(QLatin1String("C:\\MyProject\\tmp\\foo.obj"));
    QVERIFY(target);
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("C:\\MyProject\\tmp\\foo.obj"));       // $@
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("C:\\MyProject\\tmp\\foo.obj"));       // $$@
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("C:\\MyProject\\tmp\\foo"));           // $*
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("C:\\MyProject\\tmp"));                // $(@D)
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("C:\\MyProject\\tmp"));                // $$(@D)
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("C:\\MyProject\\tmp"));                // $(*D)
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("foo"));                               // $(@B)
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("foo"));                               // $$(@B)
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("foo"));                               // $(*B)
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("foo.obj"));                           // $(@F)
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("foo.obj"));                           // $$(@F)
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("foo"));                               // $(*F)
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("C:\\MyProject\\tmp\\foo"));           // $(@R)
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("C:\\MyProject\\tmp\\foo"));           // $$(@R)
    QCOMPARE(target->m_dependents.takeFirst(), QLatin1String("C:\\MyProject\\tmp\\foo"));           // $(*R)
}

void Tests::wildcardsInDependencies()
{
    QVERIFY( openMakefile(QLatin1String("wildcardsInDependencies.mk")) );
    QScopedPointer<Makefile> mkfile(m_makefileFactory->makefile());
    QVERIFY(mkfile);

    DescriptionBlock *target = mkfile->firstTarget();
    QVERIFY(target);
    QCOMPARE(target->targetName(), QLatin1String("all"));

    QCOMPARE(target->m_dependents.count(), 4);
    QCOMPARE(target->m_dependents.at(0), QLatin1String("file#99.txt"));
    QCOMPARE(target->m_dependents.at(1), QLatin1String("foo1.cpp"));
    QCOMPARE(target->m_dependents.at(2), QLatin1String("foo3.cpp"));
    QCOMPARE(target->m_dependents.at(3), QLatin1String("foo4.cpp"));
}

void Tests::windowsPathsInTargetName()
{
    QVERIFY( openMakefile(QLatin1String("windowspaths.mk")) );
    QScopedPointer<Makefile> mkfile(m_makefileFactory->makefile());
    QVERIFY(mkfile);

    DescriptionBlock* target = mkfile->firstTarget();
    QVERIFY(target != 0);
    QCOMPARE(target->targetName(), QLatin1String("C:\\foo.txt"));

    target = mkfile->target(QLatin1String("C:\\bar.txt"));
    QVERIFY(target != 0);
    QCOMPARE(target->targetName(), QLatin1String("C:\\bar.txt"));

    target = mkfile->target(QLatin1String("C:\\three.txt"));
    QVERIFY(target != 0);
    QCOMPARE(target->targetName(), QLatin1String("C:\\three.txt"));
    QCOMPARE(target->m_commands.count(), 2);

    target = mkfile->target(QLatin1String("S"));
    QVERIFY(target != 0);
    QCOMPARE(target->targetName(), QLatin1String("S"));
    QCOMPARE(target->m_commands.count(), 2);
}

/**
 * Note: this function clears the environment of m_jomProcess after every start.
 */
bool Tests::runJom(const QStringList &args, const QString &workingDirectory)
{
#ifdef _DEBUG
    const QLatin1String jomBinaryName("jomd.exe");
#else
    const QLatin1String jomBinaryName("jom.exe");
#endif
    QString jomBinary = QFileInfo(QCoreApplication::applicationDirPath() + "/" + jomBinaryName).absoluteFilePath();
    if (!QFile::exists(jomBinary)) {
        jomBinary = QFileInfo(QCoreApplication::applicationDirPath() + QLatin1String("/../../bin/") + jomBinaryName).absoluteFilePath();
        if (!QFile::exists(jomBinary)) {
            qDebug("could not find jom");
        }
    }
    QString oldWorkingDirectory;
    if (!workingDirectory.isNull()) {
        oldWorkingDirectory = QDir::currentPath();
        QDir::setCurrent(workingDirectory);
    }
    m_jomProcess->setProcessChannelMode(QProcess::MergedChannels);
    m_jomProcess->start(jomBinary, args);
    bool success = true;
    if (!m_jomProcess->waitForStarted()) {
        qDebug("could not start jom");
        success = false;
    }
    if (success && !m_jomProcess->waitForFinished()) {
        qDebug("error while executing jom");
        success = false;
    }
    if (!workingDirectory.isNull())
        QDir::setCurrent(oldWorkingDirectory);
    m_jomProcess->setEnvironment(QStringList());
    return success;
}

bool Tests::fileContentsEqual(const QString& fileName1, const QString& fileName2)
{
    QFile file1(fileName1);
    if (!file1.open(QFile::ReadOnly)) {
        qWarning("fileContentsEqual: Cannot open file1.");
        return false;
    }
    QFile file2(fileName2);
    if (!file2.open(QFile::ReadOnly)) {
        qWarning("fileContentsEqual: Cannot open file2.");
        return false;
    }

    QByteArray line1, line2;
    forever {
        line1 = file1.readLine();
        line2 = file2.readLine();
        if (line1 != line2) {
            qDebug() << "file1:" << line1;
            qDebug() << "file2:" << line2;
            return false;
        }
        bool atEnd1 = file1.atEnd();
        bool atEnd2 = file2.atEnd();
        if (atEnd1 != atEnd2)
            return false;
        if (atEnd1)
            break;
    }

    return true;
}

QStringList Tests::readJomStdOutput()
{
    QList<QByteArray> lines = m_jomProcess->readAllStandardOutput().split('\n');
    QStringList s;
    s.reserve(lines.count());
    foreach (const QByteArray &line, lines) {
        QString str = QString::fromLocal8Bit(line.trimmed());
        if (!str.isEmpty())
            s.append(str);
    }
    return s;
}

void Tests::touchFile(const QString &fileName)
{
    QFile file(fileName);
    QVERIFY(file.exists());
    file.open(QFile::WriteOnly);
    const qint64 s = file.size();
    file.resize(s + 1);
    file.resize(s);
}

void Tests::caseInsensitiveDependents()
{
    QVERIFY(runJom(QStringList() << "/f" << "test.mk" << "/nologo", "blackbox/caseInsensitiveDependents"));
    QCOMPARE(m_jomProcess->exitCode(), 0);
}

void Tests::environmentVariables_data()
{
    QTest::addColumn<QStringList>("environment");
    QTest::addColumn<QStringList>("arguments");
    QTest::addColumn<QString>("expectedVar1");
    QTest::addColumn<QString>("expectedVar2");

    // VAR1 is defined in the makefile, VAR2 is not.

    // Test definition of VAR2 via the environment. VAR1 must not be overridden.
    QTest::newRow("definition by environment")
        << (QStringList() << "VAR1=env" << "VAR2=env")
        << QStringList()
        << "file" << "env";

    // Test the /E command line switch. VAR1 and VAR2 are defined by the environment.
    QTest::newRow("definition by environment /E")
        << (QStringList() << "VAR1=env" << "VAR2=env")
        << (QStringList() << "/E")
        << "env" << "env";

    // Test macro definition on the command line. Both macros should be overridden.
    QTest::newRow("definition by command line, empty environment")
        << QStringList()
        << (QStringList() << "VAR1=override" << "VAR2=override")
        << "override" << "override";

    // Test macro definition on the command line. Both macros should be overridden by the command line arguments.
    // Environment is ignored.
    QTest::newRow("definition by command line, predefined by environment")
        << (QStringList() << "VAR1=env" << "VAR2=env")
        << (QStringList() << "VAR1=override" << "VAR2=override")
        << "override" << "override";

    // Test macro definition on the command line. Both macros should be overridden by the command line arguments.
    // Environment is ignored even if the /E switch is passed.
    QTest::newRow("definition by command line, predefined by environment, /E given")
        << (QStringList() << "VAR1=env" << "VAR2=env")
        << (QStringList() << "VAR1=override" << "VAR2=override")
        << "override" << "override";

    // Environment variables that contain invalid macro invocations are ignored.
    QTest::newRow("invalid macro invocations in environment variable value")
        << (QStringList() << "VAR1=$+$P$_$G" << "VAR2=$+$P$_$G")
        << QStringList()
        << "file" << QString();
}

void Tests::environmentVariables()
{
    QFETCH(QStringList, environment);
    QFETCH(QStringList, arguments);
    QFETCH(QString, expectedVar1);
    QFETCH(QString, expectedVar2);

    m_jomProcess->setEnvironment(environment);
    QVERIFY(runJom(QStringList() << "/f" << "test.mk" << "/nologo" << arguments, "blackbox/environmentVariables"));
    QCOMPARE(m_jomProcess->exitCode(), 0);
    QVERIFY(!m_jomProcess->atEnd());
    QCOMPARE(QString::fromLatin1(m_jomProcess->readLine().trimmed()),
             QString(QLatin1String("VAR1 ") + expectedVar1).trimmed());
    QVERIFY(!m_jomProcess->atEnd());
    QCOMPARE(QString::fromLatin1(m_jomProcess->readLine().trimmed()),
             QString(QLatin1String("VAR2 ") + expectedVar2).trimmed());
}

void Tests::environmentVariablesCaseInsensitivity()
{
    const QStringList environment = QStringList() << "Path=foobidoo";
    m_jomProcess->setEnvironment(environment);
    QVERIFY(runJom(QStringList() << "/f" << "test.mk" << "/sl" << "PATH=C:\\Narf",
                   "blackbox/environmentVariablesCaseInsensitivity"));
    QCOMPARE(m_jomProcess->exitCode(), 0);
    const QByteArray output = m_jomProcess->readAllStandardOutput().trimmed();
    QCOMPARE(output.data(), "PATH C:\\Narf C:\\Narf");
}

void Tests::environmentVariablesInCommands()
{
    QVERIFY(runJom(QStringList() << "/f" << "test.mk" << "/sl",
                   "blackbox/environmentVariablesInCommands"));
    QCOMPARE(m_jomProcess->exitCode(), 0);
    QByteArray output = m_jomProcess->readAllStandardOutput().trimmed();
    QCOMPARE(output.data(), "narf");
}

void Tests::ignoreExitCodes()
{
    QVERIFY(runJom(QStringList() << "/f" << "blackbox\\ignoreExitCodes\\test.mk"));
    QCOMPARE(m_jomProcess->exitCode(), 0);
    QByteArray output = m_jomProcess->readAllStandardOutput();
    QVERIFY(output.contains("Failing command was properly ignored"));
}

void Tests::inlineFiles()
{
    QVERIFY(runJom(QStringList() << "/f" << "test.mk" << "init" << "tests", "blackbox/inlineFiles"));
    QCOMPARE(m_jomProcess->exitCode(), 0);
    QVERIFY(fileContentsEqual("blackbox/inlineFiles/test_basic_expected.txt", "blackbox/inlineFiles/output/test_basic.txt"));
    QVERIFY(fileContentsEqual("blackbox/inlineFiles/test_multipleFiles_expected.txt", "blackbox/inlineFiles/output/test_multipleFiles.txt"));
    QVERIFY(fileContentsEqual("blackbox/inlineFiles/test_escaping_expected.txt", "blackbox/inlineFiles/output/test_escaping.txt"));
}

void Tests::unicodeFiles_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::newRow("UTF8") << QString("test_utf8.mk");
    QTest::newRow("UTF16") << QString("test_utf16.mk");
}

void Tests::unicodeFiles()
{
    QFETCH(QString, fileName);
    fileName.prepend(QLatin1String("blackbox\\unicodeFiles\\"));
    QVERIFY(runJom(QStringList() << "/f" << fileName));
    QCOMPARE(m_jomProcess->exitCode(), 0);
}

void Tests::builtin_cd_data()
{
    QTest::addColumn<QString>("testCase");
    QTest::addColumn<QByteArray>("expectedOutput");
    QTest::newRow("1") << QString("test1") << QByteArray("blackbox\\builtins\\subdir");
    QTest::newRow("2") << QString("test2") << QByteArray("blackbox\\builtins\\subdir");
    QTest::newRow("3") << QString("test3") << QByteArray("blackbox\\builtins");
}

void Tests::builtin_cd()
{
    QFETCH(QString, testCase);
    QFETCH(QByteArray, expectedOutput);
    QVERIFY(runJom(QStringList() << "/nologo" << "/f" << "cd.mk" << testCase, "blackbox/builtins"));
    QByteArray output = m_jomProcess->readAllStandardOutput();
    while (output.endsWith('\r') || output.endsWith('\n'))
        output.chop(1);
    bool success = output.endsWith(expectedOutput);
    if (!success) {
        qDebug() << "actual:      " << output;
        qDebug() << "expected end:" << expectedOutput;
    }
    QVERIFY(success);
}

void Tests::suffixes()
{
    QVERIFY(runJom(QStringList() << "/nologo" << "/f" << "test.mk", "blackbox/suffixes"));
    QList<QByteArray> output = m_jomProcess->readAllStandardOutput().split('\n');
    for (QList<QByteArray>::iterator it = output.begin(); it != output.end(); ++it)
        *it = it->trimmed();
    QCOMPARE(output.takeFirst(), QByteArray("a -> x"));
    QCOMPARE(output.takeFirst(), QByteArray("b -> x"));
    QCOMPARE(output.takeFirst(), QByteArray("c -> x"));
}

void Tests::nonexistentDependent()
{
    QVERIFY(runJom(QStringList() << "/nologo" << "/f" << "test.mk", "blackbox/nonexistentdependent"));
    QCOMPARE(m_jomProcess->exitCode(), 2);
    QList<QByteArray> output = m_jomProcess->readAllStandardOutput().split('\n');
    QVERIFY(!output.contains("we should not see this"));
    QEXPECT_FAIL("", "behaviour difference to nmake", Continue);
    QVERIFY(output.contains("yo ho ho ho"));
}

void Tests::outOfDateCheck()
{
    QVERIFY(runJom(QStringList() << "/nologo" << "/j1" << "/f" << "test.mk" << "clean" << "all",
            "blackbox/outofdatecheck"));
    QCOMPARE(m_jomProcess->exitCode(), 0);
    QStringList output = readJomStdOutput();

    // The order of this output is different from nmake, because we're evaluating
    // all leaves first before we're considering new nodes that became leaves.
    QCOMPARE(output.takeFirst(), QLatin1String("one.txt"));
    QCOMPARE(output.takeFirst(), QLatin1String("two.txt"));
    QCOMPARE(output.takeFirst(), QLatin1String("five"));
    QCOMPARE(output.takeFirst(), QLatin1String("two"));
    QCOMPARE(output.takeFirst(), QLatin1String("four.txt"));
    QCOMPARE(output.takeFirst(), QLatin1String("three.txt"));
    QCOMPARE(output.takeFirst(), QLatin1String("four"));
    QCOMPARE(output.takeFirst(), QLatin1String("six.txt"));
    QCOMPARE(output.takeFirst(), QLatin1String("three"));
    QCOMPARE(output.takeFirst(), QLatin1String("six"));
    QVERIFY(output.isEmpty());

    // now try an incremental build without any changed files
    QVERIFY(runJom(QStringList() << "/nologo" << "/j1" << "/f" << "test.mk",
            "blackbox/outofdatecheck"));
    QCOMPARE(m_jomProcess->exitCode(), 0);
    output = readJomStdOutput();
    QCOMPARE(output.takeFirst(), QLatin1String("five"));
    QCOMPARE(output.takeFirst(), QLatin1String("two"));
    QCOMPARE(output.takeFirst(), QLatin1String("four"));
    QCOMPARE(output.takeFirst(), QLatin1String("three.txt"));
    QCOMPARE(output.takeFirst(), QLatin1String("six"));
    QCOMPARE(output.takeFirst(), QLatin1String("three"));
    QVERIFY(output.isEmpty());

    touchFile("blackbox/outofdatecheck/one.txt");
    QVERIFY(runJom(QStringList() << "/nologo" << "/j1" << "/f" << "test.mk",
            "blackbox/outofdatecheck"));
    QCOMPARE(m_jomProcess->exitCode(), 0);
    output = readJomStdOutput();
    QCOMPARE(output.takeFirst(), QLatin1String("five"));
    QCOMPARE(output.takeFirst(), QLatin1String("two"));
    QCOMPARE(output.takeFirst(), QLatin1String("four"));
    QCOMPARE(output.takeFirst(), QLatin1String("three.txt"));
    QCOMPARE(output.takeFirst(), QLatin1String("six"));
    QCOMPARE(output.takeFirst(), QLatin1String("three"));
    QVERIFY(output.isEmpty());

    touchFile("blackbox/outofdatecheck/two.txt");
    QVERIFY(runJom(QStringList() << "/nologo" << "/j1" << "/f" << "test.mk",
            "blackbox/outofdatecheck"));
    QCOMPARE(m_jomProcess->exitCode(), 0);
    output = readJomStdOutput();
    QCOMPARE(output.takeFirst(), QLatin1String("five"));
    QCOMPARE(output.takeFirst(), QLatin1String("two"));
    QCOMPARE(output.takeFirst(), QLatin1String("four.txt"));
    QCOMPARE(output.takeFirst(), QLatin1String("three.txt"));
    QCOMPARE(output.takeFirst(), QLatin1String("four"));
    QCOMPARE(output.takeFirst(), QLatin1String("six.txt"));
    QCOMPARE(output.takeFirst(), QLatin1String("three"));
    QCOMPARE(output.takeFirst(), QLatin1String("six"));
    QVERIFY(output.isEmpty());
}

QTEST_MAIN(Tests)
