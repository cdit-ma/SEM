#ifndef NEWCONTROLLER_H
#define NEWCONTROLLER_H
#include "../GUI/nodeview.h"
#include "../Model/graphmlcontainer.h"
#include "../Model/model.h"

struct EdgeTemp
{
    QString id;
    qint64 lineNumber;
    QString source;
    QString target;
    QVector<GraphMLData *> data;
};

class NewController: public QObject
{
    Q_OBJECT
public:
    NewController(NodeView *view);

signals:
    void view_SetNodeItemCentered(NodeItem* node);

     void view_TriggerRubberbandMode(bool on);

public slots:
    void view_ImportGraphML(QStringList inputGraphML, GraphMLContainer *currentParent=0);
    void view_ImportGraphML(QString, GraphMLContainer *currentParent=0);

    void view_SetNodeSelected(Node* node, bool setSelected);
    void view_SetNodeCentered(Node* node);
    void view_ConstructChildNode(QPointF centerPoint);
    void view_MoveSelectedNodes(QPointF delta);


    void view_ControlPressed(bool isDown);
    void view_ShiftTriggered(bool isDown);
    void view_DeleteTriggered(bool isDown);
    void view_SelectAll();
    void view_ClearSelection();


private slots:
    void model_ConstructNodeItem(Node* node);


private:
    bool KEY_CONTROL_DOWN;
    bool KEY_SHIFT_DOWN;

    //Gets a specific Attribute from the current Element in the XML. returns "" if none.
    QString getXMLAttribute(QXmlStreamReader& xml, QString attrID);

    //Constructs a GraphMLKey Object from a XML entity.
    GraphMLKey* parseGraphMLKeyXML(QXmlStreamReader& xml);

    //Construct a specified Node type given the attached data.
    Node* constructGraphMLNode(QVector<GraphMLData *> data, GraphMLContainer *parent=0);

    //Construct a GraphMLKey.
    GraphMLKey* constructGraphMLKey(QString name, QString type, QString forString);

    //Connects Edge object and stores into a vector.
    void setupEdge(Edge* edge);

    void clearSelectedNodes();
    void setNodeSelected(Node* node, bool setSelected=true);

    Node* getSelectedNode();

    bool isNodesAncestorSelected(Node* node);
    bool isNodeKindImplemented(QString nodeKind);

    void deleteSelectedNodes();


    //Connects Node object and stores into a vector.
    void setupNode(Node* node);

    NodeItem* getNodeItemFromNode(Node* node);

    //Lists of elements in Model.
    QVector<GraphMLKey*> keys;
    QVector<Edge *> edges;
    QVector<Node *> nodes;
    QVector<NodeItem*> nodeItems;


    QVector<Node *> selectedNodes;
    Node* centeredNode;

    QStringList nodeKindsImplemented;
    QStringList aspects;


    NodeView* view;
    Model* model;
};

#endif // NEWCONTROLLER_H
