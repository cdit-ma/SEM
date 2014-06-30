#ifndef MODEL_H
#define MODEL_H

#include "graph.h"
#include "edge.h"
#include "node.h"
#include "assembly.h"
#include "componentinstance.h"
#include "eventport.h"
#include "inputeventport.h"
#include "outputeventport.h"
#include "graphmlattribute.h"

#include <QString>
#include <QVector>

#include <QXmlStreamReader>


class Model
{
public:
    Model();
    ~Model();
    //Imports
    bool importGraphML(QString inputGraphML, GraphML *parent=0);

    void parseGraphML(QXmlStreamReader xml, GraphML *parent);
    QString exportGraphML();

    Graph* getGraph();
    QString output;
private:
    void parseDataAttribute(QXmlStreamReader& xml);
    void parseGraph(QXmlStreamReader &xml);
    void parseNode(QXmlStreamReader &xml);
    void parseEdge(QXmlStreamReader &xml);


    Graph *parentGraph;

    QVector<Graph*> graphs;
    QVector<Edge *> edges;
    QVector<Node *> nodes;
    QVector<GraphMLAttribute *> attributeTypes;
};

#endif // MODEL_H
