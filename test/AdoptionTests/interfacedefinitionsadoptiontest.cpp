#include "interfacedefinitionsadoptiontest.h"

#include "../src/Controllers/modelcontroller.h"

void InterfaceDefinitionsAdoptionTest::test(){
    QFETCH(Node*, parent);
    QFETCH(Node*, adoptee);
    QFETCH(bool, result);
    QCOMPARE(parent->canAdoptChild(adoptee), result);
}

//Test to ensure that ports can only ever accept one message type (aggregate)
void InterfaceDefinitionsAdoptionTest::portTest(){

    Model* model = new Model();
    InterfaceDefinitions* id = new InterfaceDefinitions();
    model->addChild(id);

    IDL* idl = new IDL();
    IDL* idl2 = new IDL();
    id->addChild(idl);
    id->addChild(idl2);

    InEventPort* in = new InEventPort();
    Aggregate* agg = new Aggregate();

    idl->addChild(agg);
    idl2->addChild(in);

    if(in->canAcceptEdge(Edge::EC_AGGREGATE, agg)){
        in->setAggregate(agg);
    }

    QList<Edge::EDGE_KIND> kinds({Edge::EC_ASSEMBLY,
                                 Edge::EC_DATA,
                                 Edge::EC_DEPLOYMENT,
                                 Edge::EC_NONE,
                                 Edge::EC_QOS,
                                 Edge::EC_UNDEFINED,
                                 Edge::EC_WORKFLOW});

    //Should not accept any kinds other than definition and aggregate
    foreach(Edge::EDGE_KIND kind, kinds){
        QVERIFY(!in->acceptsEdgeKind(kind));
    }

    Aggregate* agg2 = new Aggregate();
    idl->addChild(agg2);

    QVERIFY(!in->canAcceptEdge(Edge::EC_AGGREGATE, agg));

    QVERIFY(in->getAggregate() == agg);

    QCOMPARE(idl2->getChildren().length() == 1, true);
}


void InterfaceDefinitionsAdoptionTest::test_data(){
    QTest::addColumn<Node*>("parent");
    QTest::addColumn<Node*>("adoptee");
    QTest::addColumn<bool>("result");

    Model* model = new Model();
    InterfaceDefinitions* id = new InterfaceDefinitions();
    IDL* idl = new IDL();

    model->addChild(id);
    id->addChild(idl);


    QTest::newRow("InterfaceDescriptions<-IDL (basic)")
            << (Node*)id << (Node*) new IDL<< true;

    QTest::newRow("InterfaceDefinitions<-Member")
            << (Node*)id << (Node*) new Member << false;

    QTest::newRow("InterfaceDefinitions<-Aggregate")
            << (Node*)id << (Node*) new Aggregate << false;

    QTest::newRow("InterfaceDefinitions<-InEventPort")
            << (Node*)id << (Node*) new InEventPort << false;

    QTest::newRow("InterfaceDefinitions<-OutEventPort")
            << (Node*)id << (Node*) new OutEventPort << false;

    QTest::newRow("IDL<-Aggregate")
            << (Node*)idl << (Node*) new Aggregate << true;

    QTest::newRow("IDL<-Blackbox")
            << (Node*)idl << (Node*) new BlackBox << true;

    QTest::newRow("IDL<-Vector")
            << (Node*)idl << (Node*) new Vector << true;



    //Set up second model to test adoption of components
    Model* model2 = new Model();
    InterfaceDefinitions* id2 = new InterfaceDefinitions();
    model2->addChild(id2);
    IDL* idl2 = new IDL();

    id2->addChild(idl2);

    Component* component = new Component();

    idl2->addChild(component);

    QTest::newRow("IDL<-Component")
            << (Node*)idl2 << (Node*) new Component << true;

    QTest::newRow("Component<-Member")
            << (Node*) component << (Node*) new Attribute << true;

    QTest::newRow("Component<-InEventPort")
            << (Node*) component << (Node*) new InEventPort << true;

    QTest::newRow("Component<-OutEventPort")
            << (Node*) component << (Node*) new OutEventPort << true;


}
