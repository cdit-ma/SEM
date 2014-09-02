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
    qint64 lineNumber;
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

    //Exports a Selection of Containers to export into GraphML
    QString exportGraphML(QVector<GraphMLContainer *> nodes);

    QVector<GraphMLContainer*> getChildren(int depth=-1);
private:
    //Imports
    bool importGraphML(QString inputGraphML, GraphMLContainer *currentParent = 0);

    Graph* getGraph();

signals:
    //Enable the GUI
    void view_EnableGUI(bool lock);

    //Update the Progress Dialog
    void view_ShowProgressDialog(bool visible);
    void view_UpdateProgressDialog(int perc, QString label=0);

    //Return the Exported Data from a successful GraphML Export.
    void view_ReturnExportedData(QString filePath, QString graphMLData);

    //Emitted by model_ConstructGUINode()
    void view_ConstructGUINode(GraphMLContainer* node);
    //Emitted by model_ConstructGUIEdge()
    void view_ConstructGUIEdge(Edge* edge);

    //Emitted by model_DestructGUINode()
    void view_DestructGUINode(GraphMLContainer* node);
    //Emitted by model_DestructGUINode()
    void view_DestructGUIEdge(Edge* edge);

public slots:
    //Called when ever a new GraphML Node or Edge has been constructed in the Model.
    void model_ConstructGUINode(GraphMLContainer* node);
    void model_ConstructGUIEdge(Edge* edge);

    //Called when ever a GraphML Node or Edge has been destructed in the Model.
    void model_DestructGUINode(GraphMLContainer* node);
    void model_DestructGUIEdge(Edge* edge);


    void model_MakeChildNode(Node* parent);


    void init_ImportGraphML(QStringList inputGraphMLData, GraphMLContainer *currentParent=0);
    void init_ExportGraphML(QString file);


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
};
#endif // MODEL_H
