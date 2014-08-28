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
#include <QStringList>

#include <QString>
#include <QVector>
#include <QThread>
#include <QObject>
#include <QXmlStreamReader>

#ifdef Q_OS_WIN
#include <windows.h> // for Sleep
#endif

struct EdgeStruct
{
    QString id;
    qint64 linenumber;
    QString source;
    QString target;
    QVector<GraphMLData *> data;
};

class Model: public QObject
{
    Q_OBJECT
public:
    Model();
    ~Model();
    //Imports

    bool importGraphML(QString inputGraphML, GraphMLContainer *currentParent=0);

    QString exportGraphML();

    QVector<Edge *> getAllEdges();
     int getNodeCount();
    Graph* getGraph();

signals:
    void enableGUI(bool lock);


    void constructNodeItemNew(GraphMLContainer* gml);

    void currentAction_ShowProgress(bool visible);
    void currentAction_UpdateProgress(int perc, QString label=0);


    void constructNodeItem(Node* node);

    void constructEdgeItem(Edge* edge);
    void setComponentCount(int count);

    void updateGUIComponent(GraphMLContainer* component);

    void returnExportedGraphMLData(QString file, QString data);
    void removeUIComponent(GraphMLContainer*);

public slots:
    void constructedGraphML(GraphMLContainer* newlyCreated);

    void model_MakeChildNode(Node* parent);


    void init_ImportGraphML(QStringList inputGraphMLData, GraphMLContainer *currentParent=0);
    void init_ExportGraphML(QString file);
    void deleteUIComponent(GraphMLContainer * comp);


    void updatePosition(GraphMLContainer* comp, QPointF pos);



private:
    GraphMLKey* parseGraphMLKey(QXmlStreamReader& xml);

    GraphMLKey* buildGraphMLKey(QString name, QString type, QString forString);
    //Construct a specific Node type given the attached Vector of data
    Node* parseGraphMLNode(QString ID, QVector<GraphMLData *> data);

    //Gets a specific Attribute from the current Element in the XML. returns "" if none.
    QString getAttribute(QXmlStreamReader& xml, QString attrID);


    void reset();
    void removeKeys();

    EdgeStruct parseEdge(QXmlStreamReader &xml);

    Graph *parentGraph;


    QVector<GraphMLKey*> keys;
    QVector<Graph*> graphs;
    QVector<Edge *> edges;
    QVector<Node *> nodes;


private:
    bool outputEvent;
    double percentage;
    bool isOperating;
    int loadCount;

};
#endif // MODEL_H
