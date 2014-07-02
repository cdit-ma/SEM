#ifndef MODEL_H
#define MODEL_H

#include "graph.h"
#include "edge.h"
#include "node.h"
#include "attribute.h"
#include "componentassembly.h"
#include "componentinstance.h"
#include "eventport.h"
#include "inputeventport.h"
#include "outputeventport.h"
#include "graphmlkey.h"
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
    bool importGraphML(QString inputGraphML, GraphMLContainer *currentParent=0);

    QString exportGraphML();

    Graph* getGraph();
    QString output;
private:
    enum PARSING_TYPE {NONE, GRAPH, NODE, EDGE, KEY, DATA};

    GraphMLKey* parseGraphMLKey(QXmlStreamReader& xml);
    GraphMLData* parseGraphMLData(QXmlStreamReader& xml, GraphMLKey* attachedKey);


    Node* parseGraphMLNode(QString ID, QVector<GraphMLData *> data);

    QString getAttribute(QXmlStreamReader& xml, QString attrID);

    void parseGraph(QXmlStreamReader &xml);


    TempEdge parseEdge(QXmlStreamReader &xml);


    Graph *parentGraph;


    QVector<GraphMLKey *> keys;
    QVector<Graph*> graphs;
    QVector<Edge *> edges;
    QVector<Node *> nodes;
    //QVector<GraphMLKey *> attributeKeys;


};

#endif // MODEL_H
