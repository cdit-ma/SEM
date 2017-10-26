#include "idlparsertester.h"
#include "../../src/idlparser/idlparser.h"

#include <iostream>
#include <sstream>
#include <QtTest/QtTest>

QTEST_MAIN(IdlParserTester)

void IdlParserTester::initTestCase(){
    //Get out of the binary directory
    test_dir =  QCoreApplication::applicationDirPath() + "/../";
}

void IdlParserTester::init(){
    controller.reset(new ModelController);
    QVERIFY(controller);
    QVERIFY(controller->SetupController());
}

void IdlParserTester::cleanup(){
    controller.reset();
}

QPair<bool, QString> IdlParserTester::ParseIdl(const QString idl_path){
    auto old_buffer = std::cerr.rdbuf();  
    std::stringstream ss;
    std::cerr.rdbuf(ss.rdbuf());
    auto result = IdlParser::IdlParser::ParseIdl(idl_path.toStdString(), true);
    std::cerr.rdbuf (old_buffer);
    return {result.first, QString::fromStdString(result.second)};
}

bool IdlParserTester::try_import_idl(const QString idl_path){
    auto result = ParseIdl(idl_path);
    if(result.first){
        return controller->importProjects({result.second});
    }
    return false;
}

void IdlParserTester::test_idl_import_data(){
    QTest::addColumn<QString>("idl_path");
    QTest::addColumn<bool>("expected_result");

    auto rel_dir = test_dir + "/idlparser/idl_files/";
    
    QDirIterator pass_tests(rel_dir, QStringList() << "test-*.idl", QDir::Files);
    while (pass_tests.hasNext()){
        QString file = pass_tests.next();
        bool is_fail_test = file.endsWith("-fail.idl");
        auto test_name = file.mid(rel_dir.length());

        bool expected_result = is_fail_test ? false : true;
        QTest::newRow(test_name.toStdString().c_str()) << file << expected_result;
    }
}

void IdlParserTester::test_idl_import(){
    //Fetch the model column
    QFETCH(QString, idl_path);
    //Fetch the model column
    QFETCH(bool, expected_result);
    //Validate the loading
    QVERIFY(try_import_idl(idl_path) == expected_result);
}