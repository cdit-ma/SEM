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

QPair<bool, QString> IdlParserTester::ParseIdls(const QStringList idl_paths){
    auto old_buffer = std::cerr.rdbuf();  
    std::stringstream ss;
    std::cerr.rdbuf(ss.rdbuf());
    std::vector<std::string> idl_paths_ss;
    
    for(auto idl_path : idl_paths){
        idl_paths_ss.push_back(idl_path.toStdString());
    }
    auto result = IdlParser::IdlParser::ParseIdls(idl_paths_ss, true);
    std::cerr.rdbuf (old_buffer);
    return {result.first, QString::fromStdString(result.second)};
}

bool IdlParserTester::try_import_idls(const QStringList idl_paths){
    auto result = ParseIdls(idl_paths);
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
        auto test_name = file.mid(rel_dir.length());
        //If it doesn't end with -fail.idl its a pass case (so true), else its a fail case (so false)
        bool expected_result = !file.endsWith("-fail.idl");
        QTest::newRow(test_name.toStdString().c_str()) << file << expected_result;
    }
}

void IdlParserTester::test_idl_import(){
    //Fetch the model column
    QFETCH(QString, idl_path);
    //Fetch the model column
    QFETCH(bool, expected_result);
    
    //Validate the loading
    QVERIFY(try_import_idls({idl_path}) == expected_result);
}

void IdlParserTester::test_multiple_import(){

    QStringList idl_paths;

    auto rel_dir = test_dir + "/idlparser/idl_files/";
    
    QDirIterator pass_tests(rel_dir, QStringList() << "test-*.idl", QDir::Files);
    while (pass_tests.hasNext()){
        QString file = pass_tests.next();
        if(!file.endsWith("-fail.idl")){
            idl_paths += file;
        }
    }

    QVERIFY(try_import_idls(idl_paths));
}

void IdlParserTester::test_no_file_import(){
    QStringList idl_paths;
    QVERIFY(try_import_idls(idl_paths) == true);
}