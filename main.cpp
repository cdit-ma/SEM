#include <QCoreApplication>
//#include "graphml.h"
//#include "edge.h"
#include "Model/graphml.h"
#include "Model/edge.h"
#include "Model/node.h"
#include "Model/graph.h"
#include "Model/hardwarenode.h"
#include "Model/componentinstance.h"
#include "Model/assembly.h"
#include "Model/inputeventport.h"
#include "Model/outputeventport.h"
#include <QDebug>

#include <QFile>
#include <QCoreApplication>
#include <QTextStream>

bool adopt(GraphML* object1, GraphML *object2){
    if(object1->isAdoptLegal(object2)){
        object1->adopt(object2);
        return true;
    }
    return false;
}

bool connect(GraphML* object1, GraphML *object2){
    if(object1->isEdgeLegal(object2)){
        Edge * newEdge = new Edge(object1,object2);
        return true;
    }
    return false;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Graph *parentGraph = new Graph("Parent Graph");

    qDebug() << "\n";

    //Make an Assembly
    Assembly *vUAV = new Assembly("vUAV");

    //Make some ComponentInstances
    ComponentInstance *GPS = new ComponentInstance("GPS");
    ComponentInstance *COMMS = new ComponentInstance("COMMS");
    ComponentInstance *CONTROLLER = new ComponentInstance("CONTROLLER");

    //Make some EventPorts
    InputEventPort *iep1 = new InputEventPort("GPS-Rx");
    OutputEventPort *oep1 = new OutputEventPort("GPS-Tx");

    //Make some Hardware Nodes
    HardwareNode *gouda31 = new HardwareNode("Gouda31");
    HardwareNode *gouda32 = new HardwareNode("Gouda32");
    HardwareNode *gouda33 = new HardwareNode("Gouda33");
    HardwareNode *gouda34 = new HardwareNode("Gouda34");


    //Connect/Adopt

    connect(iep1,oep1);

    adopt(parentGraph,vUAV);

    adopt(GPS,iep1);

    adopt(COMMS,oep1);

    adopt(vUAV,GPS);
    adopt(vUAV,COMMS);
    adopt(vUAV,CONTROLLER);


    adopt(parentGraph, gouda31);
    adopt(parentGraph, gouda32);
    adopt(parentGraph, gouda33);
    adopt(parentGraph, gouda34);


    connect(GPS, gouda31);
    connect(vUAV, gouda33);

    QFile file("c://out.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << parentGraph->toGraphML();
    file.close();

    qDebug() << "ERROR CHECKING!";



    adopt(vUAV, CONTROLLER);
    adopt(vUAV, gouda31);
    adopt(gouda31, vUAV);
    connect(gouda31, vUAV);
    connect(gouda31, vUAV);
    connect(gouda33, vUAV);

    //Trying a commit!


    /*

    if(gouda34->isAdoptLegal(parentGraph)){
        gouda34->adopt(parentGraph);
        qDebug() << "Node adpot graph";

    }






    if(parentGraph->isAdoptLegal(gouda32)){
        parentGraph->adopt(gouda32);

    }

    if(parentGraph->isAdoptLegal(gouda33)){
        parentGraph->adopt(gouda33);

    }

    if(parentGraph->isAdoptLegal(testGraph)){
        qDebug() << "ADOPT LEGAL!?";
    }else{
        qDebug() << "ADOPT ILLEGAL!?";

    }

    Edge *currentEdge;
    if(gouda32->isEdgeLegal(gouda33)){
        currentEdge = new Edge(gouda32,gouda33);
    }


    qDebug() << "\n";
    qDebug() << "Parent Graphs Children";
    QVector<GraphML*> children = parentGraph->getChildren();

    for(int i = 0; i <children.size();i++){
        qDebug()<<children[i]->toString();
    }

    qDebug() << gouda34->isAncestorOf(gouda33);
    qDebug() << gouda33->isDescendantOf(gouda34);



    if(gouda33->isAdoptLegal(gouda34)){
        gouda33->adopt(gouda34);
    }

    qDebug() << gouda33->isDescendantOf(gouda34);
    qDebug() << gouda33->isDescendantOf(gouda35);

*/
    //Edge test = new Edge(null,null);
    return a.exec();
}
