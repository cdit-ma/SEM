#include "interfacedefinitionsadoptiontest.h"

#include "../src/Controllers/modelcontroller.h"

void InterfaceDefinitionsAdoptionTest::test(){
    QFETCH(Node*, parent);
    QFETCH(Node*, adoptee);
    QFETCH(bool, result);
    QCOMPARE(parent->canAdoptChild(adoptee), result);
}

void InterfaceDefinitionsAdoptionTest::test_data(){
    QTest::addColumn<Node*>("parent");
    QTest::addColumn<Node*>("adoptee");
    QTest::addColumn<bool>("result");

    Model* model = new Model();
    InterfaceDefinitions* id = new InterfaceDefinitions();
    IDL* idl = new IDL();

    model->addChild(id);


    QTest::newRow("InterfaceDescriptions<-IDL (basic)")
            << (Node*)id << (Node*)idl << true;

    QTest::newRow("InterfaceDefinitions<-Member")
            << (Node*)id << (Node*) new Member << false;

    QTest::newRow("InterfaceDefinitions<-Aggregate")
            << (Node*)id << (Node*) new Aggregate << false;

    QTest::newRow("InterfaceDefinitions<-InEventPort")
            << (Node*)id << (Node*) new InEventPort << false;



}
