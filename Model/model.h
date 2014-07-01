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
#include "graphmldata.h"

#include <QString>
#include <QVector>

#include <QXmlStreamReader>

struct TempEdge
{
    QString id;
    QString source;
    QString target;
    QVector<GraphMLData *> data;
};
class Model
{
public:
    Model();
    ~Model();
    //Imports
    bool importGraphML(QString inputGraphML, GraphMLContainer *parent=0);

    QString exportGraphML();

    Graph* getGraph();
    QString output;
private:
    enum PARSING_TYPE {NONE, GRAPH, NODE, EDGE, KEY, DATA};
    GraphMLAttribute* parseKeyAttribute(QXmlStreamReader& xml);
    GraphMLData* parseDataAttribute(QXmlStreamReader& xml);

    QString getAttribute(QXmlStreamReader& xml, QString attrID);

    void parseGraph(QXmlStreamReader &xml);
    void parseNode(QXmlStreamReader &xml);
    TempEdge parseEdge(QXmlStreamReader &xml);

    GraphMLAttribute* getGraphMLAttribute(QString key);

    Graph *parentGraph;

    QVector<Graph*> graphs;
    QVector<Edge *> edges;
    QVector<Node *> nodes;
    QVector<GraphMLAttribute *> attributeTypes;


};

#endif // MODEL_H
