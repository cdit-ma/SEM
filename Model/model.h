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
    QString exportGraphML(GraphMLContainer * node);

    //Gets a list of all the child Nodes in this Model. Based on a depth variable.
    QVector<GraphMLContainer*> getChildren(int depth=-1);
    //Imports a GraphML XML Document into the Model, inserting it into the currentParent Variable.
    bool importGraphML(QString inputGraphML, GraphMLContainer *currentParent = 0);

private:
    //Returns the parentGraph of this Model.
    Graph* getGraph();
signals:
    //Enable and disables the GUI
    void view_EnableGUI(bool lock);

    void controller_ActionTrigger(QString action);

    //Updates the Progress Dialog
    void view_ShowProgressDialog(bool visible);
    void view_UpdateProgressDialog(int perc, QString label=0);

    //Returns the Exported Data from a successful GraphML Export.
    void view_ReturnExportedData(QString filePath, QString graphMLData);

    //Emitted by model_ConstructGUINode()
    void view_ConstructGUINode(GraphMLContainer* node);
    //Emitted by model_ConstructGUIEdge()
    void view_ConstructGUIEdge(Edge* edge);

    //Emitted by model_DestructGUINode()
    void view_DestructGUINode(GraphMLContainer* node, QString ID);
    //Emitted by model_DestructGUINode()
    void view_DestructGUIEdge(Edge* edge, QString srcID, QString dstID);

public slots:
    //MODEL SLOTS
    //Functions triggered by the entities in the Model.

    //Called when ever a new GraphML Node or Edge has been constructed in the Model.
    void model_ConstructGUINode(GraphMLContainer* node);
    void model_ConstructGUIEdge(Edge* edge);

    //Called when ever a GraphML Node or Edge has been destructed in the Model.
    void model_DestructGUINode(GraphMLContainer* node, QString ID);
    void model_DestructGUIEdge(Edge* edge, QString srcID, QString dstID);

    //VIEW SLOTS
    //Functions triggered by the Controller from the View.
    void view_ImportGraphML(QStringList inputGraphMLDatas, GraphMLContainer *currentParent=0);
    void view_ImportGraphML(QString inputGraphMLData, GraphMLContainer *currentParent=0);
    void view_ExportGraphML(QString file);


    //Called when the Controller constructs a new Node.
    void view_ConstructNode(QString kind, GraphMLContainer* parent);
    void view_ConstructEdge(GraphMLContainer* src, GraphMLContainer* dst);
private:
    //Constructs a GraphMLKey Object from a XML entity.
    GraphMLKey* parseGraphMLKeyXML(QXmlStreamReader& xml);

    //Constructs an EdgeStruct from the XML entity.
    EdgeStruct parseEdgeXML(QXmlStreamReader &xml);

    //Construct a GraphMLKey.
    GraphMLKey* constructGraphMLKey(QString name, QString type, QString forString);

    //Construct a specified Node type given the attached data.
    Node* constructGraphMLNode(QVector<GraphMLData *> data, GraphMLContainer *parent=0);

    //Gets a specific Attribute from the current Element in the XML. returns "" if none.
    QString getAttribute(QXmlStreamReader& xml, QString attrID);

    //Connects Edge object and stores into a vector.
    void setupEdge(Edge* edge);

    //Connects Node object and stores into a vector.
    void setupNode(Node* node);

    //Clears Model
    void clearModel();

    //Clears the Keys stored in the model.
    void removeKeys();

    //The Parent graph of this model.
    Graph *parentGraph;

    //Lists of elements in Model.
    QVector<GraphMLKey*> keys;
    QVector<Graph*> graphs;
    QVector<Edge *> edges;
    QVector<Node *> nodes;

    bool outputEvent;
};
#endif // MODEL_H
