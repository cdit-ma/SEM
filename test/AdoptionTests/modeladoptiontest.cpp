#include "modeladoptiontest.h"
#include "../src/Controllers/modelcontroller.h"

void ModelAdoptionTest::test()
{
    QFETCH(Node*, parent);
    QFETCH(Node*, adoptee);
    QFETCH(bool, result);

    QCOMPARE(parent->canAdoptChild(adoptee), result);
}

void ModelAdoptionTest::test_data()
{
    QTest::addColumn<Node*>("parent");
    QTest::addColumn<Node*>("adoptee");
    QTest::addColumn<bool>("result");

    Model* model = new Model();
    InterfaceDefinitions* id = new InterfaceDefinitions();
    BehaviourDefinitions* bd = new BehaviourDefinitions();
    DeploymentDefinitions* dd = new DeploymentDefinitions();
    AssemblyDefinitions* ad = new AssemblyDefinitions();
    HardwareDefinitions* hd = new HardwareDefinitions();

    QTest::newRow("model<-InterfaceDefinitions")
            << (Node*)model << (Node*)id << true;

    QTest::newRow("model<-BehaviourDefinitions")
            << (Node*)model << (Node*)bd << true;

    QTest::newRow("model<-DeploymentDefinitions")
            << (Node*)model << (Node*)dd << true;

    QTest::newRow("model<-AssemblyDefinitions")
            << (Node*)model << (Node*)ad << false;

    QTest::newRow("model<-HardwareDefinitions")
            << (Node*)model << (Node*)hd << false;

    QTest::newRow("model<-IDL")
            << (Node*)model << (Node*) new IDL() << false;
}
