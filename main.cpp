#include <QCoreApplication>
//#include "graphml.h"
//#include "edge.h"
#include "Model/Model.h"
/*
#include "Model/edge.h"
#include "Model/node.h"
#include "Model/graph.h"
#include "Model/hardwarenode.h"
#include "Model/componentinstance.h"
#include "Model/assembly.h"
#include "Model/inputeventport.h"
#include "Model/outputeventport.h"
*/
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
QString readFile(QString filePointer){
    QFile file(filePointer);

    if(!file.open(QFile::ReadOnly | QFile::Text)){
        qDebug() << "could not open file for read";
        return "";
    }

    QTextStream in(&file);
    QString mText = in.readAll();

    file.close();

    return mText;
}


bool writeFile(QString filePointer, QString data){
    try{
        QFile file(filePointer);
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&file);
        out << data;
        file.close();
        qDebug()<<"Successfully written file: " <<filePointer;
    }catch(...){
        return false;
    }
    return true;

}



int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Model *model = new Model();

    Graph *parentGraph = model->getGraph();

    QString input = readFile("c:\\inputGML.graphml");

    model->importGraphML(input, parentGraph);


    qDebug() << "\n";

    /*
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

    qDebug() << "ERROR CHECKING!";

    adopt(vUAV, CONTROLLER);
    adopt(vUAV, gouda31);
    adopt(gouda31, vUAV);
    connect(gouda31, vUAV);
    connect(gouda31, vUAV);
    connect(gouda33, vUAV);

    */

    writeFile("c:\\file.output",model->exportGraphML());

    return a.exec();
}
