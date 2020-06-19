#include "tests.h"
#include <Entities/Keys/versionkey.h>
#include <version.h>
#include <QtTest/QtTest>

QTEST_APPLESS_MAIN(VersionKeyTests)

void VersionKeyTests::Test_Comparators(){
    //Get the Values
    QFETCH(QString, current);
    QFETCH(QString, compare);
    QFETCH(int, expected);

    QCOMPARE(VersionKey::CompareVersion(current, compare), expected);
    QCOMPARE(VersionKey::CompareVersion(compare, current), -expected);
    QCOMPARE(VersionKey::CompareVersion(current, current), 0);
    QCOMPARE(VersionKey::CompareVersion(compare, compare), 0);
}

void VersionKeyTests::Test_Validity(){
    QFETCH(QString, version);
    QFETCH(bool, expected);
    QCOMPARE(VersionKey::IsVersionValid(version), expected);
}

void VersionKeyTests::Test_Comparators_data()
{
    QTest::addColumn<QString>("current");
    QTest::addColumn<QString>("compare");
    QTest::addColumn<int>("expected");

    QTest::newRow("SimpleEmpty1") << "1.0.0" << "0.0.0" << 1;
    QTest::newRow("SimpleEmpty2") << "0.1.0" << "0.0.0" << 1;
    QTest::newRow("SimpleEmpty3") << "0.0.1" << "0.0.0" << 1;
    QTest::newRow("SimpleEmpty4") << "0.0.0-a" << "0.0.0" << 1;
    
    QTest::newRow("Basic6") << "1.0.0" << "0.1.0" << 1;
    QTest::newRow("Basic7") << "0.1.0" << "0.0.1" << 1;
    QTest::newRow("Basic8") << "0.0.1" << "0.0.0-a" << 1;


    QTest::newRow("Extension1") << "0.0.0-b" << "0.0.0-a" << 1;
    QTest::newRow("Extension2") << "0.0.0-B" << "0.0.0-a" << 1;
    QTest::newRow("Extension3") << "10.0.0-a" << "0.0.0-B" << 1;
    QTest::newRow("ExtensionCase1") << "0.0.0-a" << "0.0.0-A" << 0;
    QTest::newRow("ExtensionCase2") << "0.0.0-0a" << "0.0.0-0A" << 0;

    QTest::newRow("Basic1") << "2.0.0" << "1.0.0" << 1;
    QTest::newRow("Basic2") << "2.0.0" << "1.15.0" << 1;
    QTest::newRow("Basic3") << "2.0.0" << "1.1.1" << 1;

    QTest::newRow("Basic4") << "2.3.4-a" << "2.3.4" << 1;
    QTest::newRow("Basic5") << "2.3.4-ab" << "2.3.4-a" << 1;
    QTest::newRow("Basic6") << "2.3.5" << "2.3.4-a" << 1;

    QTest::newRow("Basic7") << "2.3.0" << "2.2.0" << 1;
    QTest::newRow("Basic8") << "2.3.0" << "2.2.0" << 1;

    QTest::newRow("Double1") << "10.0.0" << "9.0.0" << 1;
    QTest::newRow("Double2") << "0.10.0" << "0.9.0" << 1;
    QTest::newRow("Double3") << "0.0.10" << "0.0.9" << 1;

    QTest::newRow("Triple1") << "100.0.0" << "99.0.0" << 1;
    QTest::newRow("Triple2") << "0.100.0" << "0.99.0" << 1;
    QTest::newRow("Triple3") << "0.0.100" << "0.0.99" << 1;
    QTest::newRow("Triple4") << "100.99.88" << "100.99.87" << 1;
    
    QTest::newRow("PaddedZero1") << "00.00.00" << "0.0.0" << 0;
    QTest::newRow("PaddedZero2") << "010.010.010" << "9.9.9" << 1;
    QTest::newRow("PaddedZero3") << "10.10.10" << "010.010.010" << 0;
}

void VersionKeyTests::Test_MEDEAVersion(){
    //Test to check MEDEA Version is valid
    QCOMPARE(VersionKey::IsVersionValid(Version::GetMedeaVersion()), true);
}

void VersionKeyTests::Test_Validity_data(){
    QTest::addColumn<QString>("version");
    QTest::addColumn<bool>("expected");

    QTest::newRow("Blank") << "" << false;
    QTest::newRow("Letters") << "X.Y.Z" << false;
    
    QTest::newRow("Negative1") << "-1.2.3-a" << false;
    QTest::newRow("Negative2") << "1.-2.3-a" << false;
    QTest::newRow("Negative3") << "1.-2.-3-a" << false;

    QTest::newRow("Dots1") << "..." << false;
    QTest::newRow("Dots2") << "1.." << false;
    QTest::newRow("Dots3") << ".1." << false;
    QTest::newRow("Dots4") << "..1" << false;
    QTest::newRow("Dots5") << "10..1" << false;
    QTest::newRow("NoNumbers") << "...-" << false;

    QTest::newRow("NoExtension") << "10.10.10-" << false;

    QTest::newRow("Full") << "1.2.3-a" << true;
    QTest::newRow("FullTriple1") << "100.100.100-a" << true;
    QTest::newRow("FullTriple2") << "100.100.100-RC10" << true;
    QTest::newRow("Pi") << "3.14.159-PI" << true;
}