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

    QVector<Edge *> getAllEdges() const;
     int getNodeCount();
    Graph* getGraph();

signals:
    void enableGUI(bool lock);

    void progressDialog_Show();
    void progressDialog_Hide();
    void progressDialog_SetValue(int perc);
    void progressDialog_SetText(QString text);

    void constructNodeItem(Node* node);
    void setComponentCount(int count);

    void returnExportedGraphMLData(QString file, QString data);
public slots:
    void init_ImportGraphML(QStringList inputGraphMLData, GraphMLContainer *currentParent=0);
    void init_ExportGraphML(QString file);

private:
    GraphMLKey* parseGraphMLKey(QXmlStreamReader& xml);

    //Construct a specific Node type given the attached Vector of data
    Node* parseGraphMLNode(QString ID, QVector<GraphMLData *> data);

    //Gets a specific Attribute from the current Element in the XML. returns "" if none.
    QString getAttribute(QXmlStreamReader& xml, QString attrID);



    EdgeStruct parseEdge(QXmlStreamReader &xml);

    Graph *parentGraph;


    QVector<GraphMLKey *> keys;
    QVector<Graph*> graphs;
    QVector<Edge *> edges;
    QVector<Node *> nodes;


private:
    double percentage;
    bool isOperating;

};
#endif // MODEL_H
