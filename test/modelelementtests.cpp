#include "modelelementtests.h"
#include <QDebug>

#include "../src/Controllers/modelcontroller.h"



void ModelElementTests::nodeTest()
{
    QCOMPARE(true, true);
}


void ModelElementTests::modelCreationTest(){
    Model* model = new Model();

    InterfaceDefinitions* id = new InterfaceDefinitions();
    BehaviourDefinitions* bd = new BehaviourDefinitions();
    DeploymentDefinitions* dd = new DeploymentDefinitions();
    AssemblyDefinitions* ad = new AssemblyDefinitions();
    HardwareDefinitions* hd = new HardwareDefinitions();

    model->addChild(id);
    model->addChild(bd);
    model->addChild(dd);
    dd->addChild(ad);
    dd->addChild(hd);

    QList<Node*> children = model->getChildren();

    QList<Node::NODE_KIND> kinds({Node::NK_INTERFACE_DEFINITIONS,
                                 Node::NK_BEHAVIOUR_DEFINITIONS,
                                 Node::NK_DEPLOYMENT_DEFINITIONS,
                                 Node::NK_ASSEMBLY_DEFINITIONS,
                                 Node::NK_HARDWARE_DEFINITIONS});

    //Check that only required nodes were created
    //Use qverify to bail out of testing early
    foreach(Node* node, children){
        QVERIFY(kinds.contains(node->getNodeKind()));
        kinds.removeOne(node->getNodeKind());
    }

    //One of required child nodes was not created
    QVERIFY(kinds.empty());


}


void ModelElementTests::componentImplTest(){
    GraphML::resetID();

    Model* model = setupModel();
    BehaviourDefinitions* bd = 0;

    foreach(Node* child , model->getChildren()){
        if(child->getNodeKind() == Node::NK_BEHAVIOUR_DEFINITIONS){
            //bd = child;
            bd = (BehaviourDefinitions*)child;
        }
    }

    ComponentImpl *ci = new ComponentImpl();
    QCOMPARE(bd->canAdoptChild(ci), true);
}

Model* ModelElementTests::setupModel(){
    Model* model = new Model();
    InterfaceDefinitions* id = new InterfaceDefinitions();
    BehaviourDefinitions* bd = new BehaviourDefinitions();
    DeploymentDefinitions* dd = new DeploymentDefinitions();
    AssemblyDefinitions* ad = new AssemblyDefinitions();
    HardwareDefinitions* hd = new HardwareDefinitions();

    model->addChild(id);
    model->addChild(bd);
    model->addChild(dd);
    dd->addChild(ad);
    dd->addChild(hd);

    return model;
}


