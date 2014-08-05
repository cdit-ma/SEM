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
#include <QThread>
#include <QObject>
#include <QXmlStreamReader>

struct EdgeStruct
{
    QString id;
    qint64 linenumber;
    QString source;
    QString target;
    QVector<GraphMLData *> data;
};

class Model: public QThread
{
    Q_OBJECT
public:
    Model(QObject *parent);
    ~Model();
    //Imports

    bool importGraphML(QString inputGraphML, GraphMLContainer *currentParent=0);

    QString exportGraphML();

    QVector<Edge *> getAllEdges() const;
     int getNodeCount();
    Graph* getGraph();
signals:
    void updatePercentage(int percentage);
    void componentCount(int percentage);

private:
    GraphMLKey* parseGraphMLKey(QXmlStreamReader& xml);

    //Construct a specific Node type given the attached Vector of data
    Node* parseGraphMLNode(QString ID, QVector<GraphMLData *> data);

    //Gets a specific Attribute from the current Element in the XML. returns "" if none.
    QString getAttribute(QXmlStreamReader& xml, QString attrID);


    bool importGraphMLThread();

    EdgeStruct parseEdge(QXmlStreamReader &xml);

    Graph *parentGraph;


    QVector<GraphMLKey *> keys;
    QVector<Graph*> graphs;
    QVector<Edge *> edges;
    QVector<Node *> nodes;



    // QObject interface
public:
    bool event(QEvent *);

    // QThread interface
protected:
    void run();

private:
    double percentage;
    QString importGraphMLData;
    GraphMLContainer *importGraphMLParent;

};
#endif // MODEL_H
