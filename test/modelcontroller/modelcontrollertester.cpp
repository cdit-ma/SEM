#include "modelcontrollertester.h"
#include <QtTest/QtTest>

QTEST_MAIN(ModelControllerTester)

void ModelControllerTester::initTestCase(){
    //Get out of the binary directory
    test_dir =  QCoreApplication::applicationDirPath() + "/../";
}

void ModelControllerTester::init(){
    controller.reset(new ModelController);
    QVERIFY(controller);
}

void ModelControllerTester::cleanup(){
    controller.reset();
}

void ModelControllerTester::test_models_data(){
    QTest::addColumn<QString>("model");
    auto rel_dir = test_dir + "/modelcontroller/test_models/";

    QDirIterator it(rel_dir, QStringList() << "*.graphml", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()){
        QString file = it.next();
        auto test_name = file.mid(rel_dir.length());
        QTest::newRow(test_name.toStdString().c_str()) << file;
    }
}

void ModelControllerTester::test_models(){
    //Fetch the model column
    QFETCH(QString, model);
    //Validate the loading
    QVERIFY(try_open_graphml(model));
}

bool ModelControllerTester::try_open_rel_graphml(const QString rel_model_path){
    auto path = test_dir + "/modelcontroller/" + rel_model_path;
    return try_open_graphml(path);
}

bool ModelControllerTester::try_open_graphml(const QString model_path){
    return controller->SetupController(model_path);
} 
