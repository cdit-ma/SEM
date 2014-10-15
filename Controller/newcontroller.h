#ifndef NEWCONTROLLER_H
#define NEWCONTROLLER_H
#include <QStack>
#include "../GUI/nodeview.h"
#include "../GUI/nodeconnection.h"

#include "../Model/graphmlcontainer.h"
#include "../Model/model.h"
#include <QMenu>
#include <QAction>
#include <QInputDialog>

enum ACTION_TYPE {CONSTRUCTED, DESTRUCTED, MODIFIED};

struct EdgeTemp
{
    QString id;
    qint64 lineNumber;
    QString source;
    QString target;
    QVector<GraphMLData *> data;
};

struct ActionItem{
    ACTION_TYPE actionType;
    GraphML::KIND actionKind;
    QString parentID;
    QString srcID;
    QString dstID;
    QString ID;
    QString keyName;
    QString dataValue;
    QString removedXML;
    //In the form KeyName, KeyType, KeyFor, Data Value.
    QVector<QStringList> dataValues;
    QString actionName;
    int actionID;
};


class NewController: public QObject
{
    Q_OBJECT
public:
    NewController(NodeView *view);
    ~NewController();
    //Exports a Selection of Containers to export into GraphML
    QString exportGraphML(QVector<Node *> nodes);
    QString exportGraphML(Node* node);


    QStandardItemModel* getModel();
    //Gets a list of all the kinds of Components which have a visual representation.
    QStringList getNodeKinds();
    QStringList getViewAspects();

signals:
    void view_SetGUIEnabled(bool setEnabled);
    void view_SetNodeItemCentered(NodeItem* node);

    void view_SetRubberbandSelectionMode(bool on);

    void view_ExportGraphML(QString data);

    void view_SetSelectedAttributeModel(AttributeTableModel* model);
    void view_UpdateUndoList(QStringList list);
    void view_UpdateRedoList(QStringList list);

    void view_UpdateProgressBar(int percentage, QString label="");

    void view_updateCopyBuffer(QString data);

public slots:
    void view_ImportGraphML(QStringList inputGraphML, GraphMLContainer *currentParent=0);
    void view_ImportGraphML(QString, GraphMLContainer *currentParent=0, bool linkID = false);


    //Informants from the AttributeTableModel Class.
    void view_UpdateGraphMLData(GraphML* parent, QString keyName, QString dataValue);
    void view_ConstructGraphMLData(GraphML* parent, QString keyName);
    void view_DestructGraphMLData(GraphML* parent, QString keyName);



    void view_ConstructMenu(QPoint position);

    void view_ExportGraphML();

    void view_SetNodeSelected(Node* node, bool setSelected);
    void view_SetEdgeSelected(Edge* edge, bool setSelected);

    void view_SetItemSelected(GraphML* item, bool setSelected);

    void view_SetNodeCentered(Node* node);
    void view_ConstructChildNode(QPointF centerPoint);
    void view_ConstructChildNode();
    void view_ConstructEdge(Node* src, Node* dst);
    void view_ConstructEdge(Node* src, Node* dst, QVector<QStringList> data, QString previousID=0);
    void view_MoveSelectedNodes(QPointF delta);

    void view_SetChildNodeKind(QString nodeKind);

    void view_FilterNodes(QStringList filterString);

    void view_HideUnconnectableNodes(Node* node);
    void view_ShowAllNodes();


    void view_ActionTriggered(QString actionName);

    //Keyboard Actions
    void view_ClearHistory();
    void view_ControlPressed(bool isDown);
    void view_ShiftPressed(bool isDown);
    void view_DeletePressed(bool isDown=true);

    //Copy/Pase Operationsview_DeletePressed
    void view_Undo();
    void view_Redo();

    void view_Copy();
    void view_Cut();
    void view_Paste(QString xmlData);

    void view_ClearKeyModifiers();

    //Selection Actions.
    void view_SelectAll();
    void view_ClearCenteredNode();
    void view_ClearSelection();

private slots:
    void view_ConstructNodeItem(Node* node);
    void view_ConstructNodeEdge(Edge* edge);


private:
    bool copySelection();
    //Gets a specific Attribute from the current Element in the XML. returns "" if none.
    QString getXMLAttribute(QXmlStreamReader& xml, QString attrID);

    //Constructs a GraphMLKey Object from a XML entity.
    GraphMLKey* parseGraphMLKeyXML(QXmlStreamReader& xml);

    //Construct a GraphMLKey.
    GraphMLKey* constructGraphMLKey(QString name, QString type, QString forString);

    //Construct a specified Node type given the attached data.
    Node* constructGraphMLNode(QVector<GraphMLData *> data, GraphMLContainer *parent = 0);

    //Connects Node object and stores into a vector.
    void setupNode(Node* node);

    bool isGraphMLValid(QString inputGraphML);


    //Connects Edge object and stores into a vector.
    void setupEdge(Edge* edge);
    void updateGUIUndoRedoLists();

    //Selection methods
    Node* getSelectedNode();
    Edge* getSelectedEdge();

    void deleteNode(Node* node);
    void deleteEdge(Edge* edge, bool addAction = true);

    void setNodeSelected(Node* node, bool setSelected=true);
    void setEdgeSelected(Edge* edge, bool setSelected=true);
    void clearSelectedNodes();
    void clearSelectedEdges();
    void deleteSelectedNodes();
    void deleteSelectedEdges();

    bool isNodesAncestorSelected(Node* node);
    bool isNodeSelected(Node* node);
    bool isEdgeSelected(Edge* edge);

    bool isEdgeLegal(Node* src, Node* dst);



    bool isNodeKindImplemented(QString nodeKind);


    void reverseAction(ActionItem action);

    void attachGraphMLData(GraphML* item, QVector<QStringList> dataList);
    void addActionToStack(ActionItem action);


    void undoRedo();

    QStack<ActionItem> undoStack;
    QStack<ActionItem> redoStack;


    NodeEdge* getNodeEdgeFromEdge(Edge* edge);
    NodeItem* getNodeItemFromNode(Node* node);

    GraphML* getGraphMLFromID(QString ID);
    Node* getNodeFromID(QString ID);
    Edge* getEdgeFromID(QString ID);


    QVector<GraphMLKey*> keys;

    QVector<Edge *> edges;
    QVector<Node *> nodes;

    QVector<NodeItem*> nodeItems;
    QVector<NodeEdge*> nodeEdges;


    //Selection Lists
    QVector<Edge *> selectedEdges;
    QVector<Node *> selectedNodes;


    Node* getNodeFromPreviousID(QString ID);
    QString getNewIDFromPreviousID(QString ID);
    GraphML* getGraphMLFromPreviousID(QString ID);

    void linkPreviousIDToID(QString previousID, QString newID);

    bool isGraphMLNode(GraphML* item);
    bool isGraphMLEdge(GraphML* item);
    Node* getNodeFromGraphML(GraphML* item);
    Edge* getEdgeFromGraphML(GraphML* item);

    //Provides a lookup for old IDs.
    QHash<QString, QString> pastIDLookup;
    QPointF menuPosition;

    Node* centeredNode;

    QStringList nodeKinds;

    QStringList viewAspects;
    QStringList protectedKeyNames;

    bool UNDOING;
    bool REDOING;
    bool SELECT_NEWLY_CREATED;

    bool KEY_CONTROL_DOWN;
    bool KEY_SHIFT_DOWN;

    qreal HIDDEN_OPACITY;

    NodeView* view;

    Graph* getParentGraph();
    Graph* parentGraph;

    bool CUT_LINKING;
    int actionCount;
    QString currentAction;
    int currentActionID;

    QString childNodeKind;

    QStandardItemModel* treeModel;
};

#endif // NEWCONTROLLER_H
