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
#include <QDebug>

#include <QFile>
#include <QCoreApplication>
#include <QTextStream>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Graph *parentGraph = new Graph("Parent Graph");
    Graph *nodeGraph = new Graph("Node Graph");
    Graph *componentGraph = new Graph("Component Graph");
    qDebug() << "\n";

    Assembly *vUAV = new Assembly("vUAV");


    //Make some Hardware Nodes
    HardwareNode *gouda31 = new HardwareNode("Gouda31");
    HardwareNode *gouda32 = new HardwareNode("Gouda32");
    HardwareNode *gouda33 = new HardwareNode("Gouda33");
    HardwareNode *gouda34 = new HardwareNode("Gouda34");

    ComponentInstance *GPS = new ComponentInstance("GPS");
    ComponentInstance *COMMS = new ComponentInstance("COMMS");
    ComponentInstance *CONTROLLER = new ComponentInstance("CONTROLLER");


    if(parentGraph->isAdoptLegal(vUAV)){
        parentGraph->adopt(vUAV);
    }



    //Adopt the componentInstance nodes into the componentGraph
    if(vUAV->isAdoptLegal(GPS)){
        vUAV->adopt(GPS);
    }

    //Adopt the componentInstance nodes into the componentGraph
    if(vUAV->isAdoptLegal(COMMS)){
        vUAV->adopt(COMMS);
    }
    //Adopt the componentInstance nodes into the componentGraph
    if(vUAV->isAdoptLegal(CONTROLLER)){
        vUAV->adopt(CONTROLLER);
    }



    //Adopt the hardware node into the nodeGraph
    if(parentGraph->isAdoptLegal(gouda31)){
        parentGraph->adopt(gouda31);
    }

    //Adopt the hardware node into the nodeGraph
    if(parentGraph->isAdoptLegal(gouda32)){
        parentGraph->adopt(gouda32);
    }

    //Adopt the hardware node into the nodeGraph
    if(parentGraph->isAdoptLegal(gouda33)){
        parentGraph->adopt(gouda33);
    }

    //Adopt the hardware node into the nodeGraph
    if(parentGraph->isAdoptLegal(gouda34)){
        parentGraph->adopt(gouda34);
    }

    if(GPS->isEdgeLegal(gouda31)){
        Edge *gpsToGouda31 = new Edge(GPS,gouda31);
    }


    if(vUAV->isEdgeLegal(gouda33)){
        Edge *vuavToGouda31 = new Edge(vUAV,gouda33);
    }

    qDebug();
    qDebug() << nodeGraph->toString() << " CHILDREN";
    QVector<GraphML*> children = nodeGraph->getChildren();

    for(int i = 0; i <children.size();i++){
        qDebug()<< children[i]->toString();
        qDebug() << children[i]->getParent()->toString();
    }

    qDebug() << componentGraph->toString() << " CHILDREN";
    children = componentGraph->getChildren();

    for(int i = 0; i <children.size();i++){
        qDebug()<<children[i]->toString();
        qDebug() << children[i]->getParent()->toString();
    }


    QFile file("c://out.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
       out << parentGraph->toGraphML();
        file.close();


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
