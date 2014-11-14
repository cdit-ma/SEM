#ifndef NEWCONTROLLER_H
#define NEWCONTROLLER_H
#include <QStack>
#include "../GUI/nodeview.h"
#include "../GUI/nodeconnection.h"
#include "../Model/model.h"
#include <QStandardItemModel>
#include "../Model/node.h"

//Include Validation Engine
#include "../ValidationEngine/validationengine.h"
#include "../ValidationEngine/Plugins/interfacedefinitionplugin.h"


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

    //In the form KeyName, KeyType, KeyFor, Data Value,.
    QVector<QStringList> dataValues;
    //In the form ID
    QVector<QStringList> boundDataIDs;
    QVector<QString> parentDataID;

    QString boundDataID;
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
    QStringList getNodeKinds();
    QStringList getViewAspects();

signals:
    void view_SetGUIEnabled(bool setEnabled);
    //Inform the View to Center the Canvas around an Node.
    void view_SetNodeItemCentered(NodeItem* node);

    void view_SetRubberbandSelectionMode(bool on);

    void view_WriteGraphML(QString filename, QString data);

    void view_SetSelectedAttributeModel(AttributeTableModel* model);
    void view_UpdateUndoList(QStringList list);
    void view_UpdateRedoList(QStringList list);

    void view_UpdateProgressBar(int percentage, QString label="");

    void view_UpdateCopyBuffer(QString data);

    void view_UpdateProjectName(QString name);

    void view_PrintErrorCode(NodeItem* node, QString text);
    void view_UpdateStatusText(QString statusText);

public slots:
    void view_ImportGraphML(QStringList inputGraphML, Node *currentParent=0);
    void view_ImportGraphML(QString, Node *currentParent=0, bool linkID = false);

    void validator_HighlightError(Node* node, QString error);
    void view_ValidateModel();

    //Informants from the AttributeTableModel Class.
    void view_UpdateGraphMLData(GraphML* parent, QString keyName, QString dataValue);
    void view_ConstructGraphMLData(GraphML* parent, QString keyName);
    void view_DestructGraphMLData(GraphML* parent, QString keyName);


    void view_ConstructComponentInstance(Component* definition = 0 );
    void view_ConstructComponentImpl(Component* definition = 0 );

    void view_SortModel();
    void view_CenterComponentImpl(Node* node = 0);
    void view_CenterComponentDefinition(Node* node = 0);
    void view_CenterAggregate(Node* node = 0);

    void view_ProjectNameUpdated(GraphMLData* label);



    void view_ConstructMenu(QPoint position);

    void view_ExportGraphML(QString filename);

    void view_SetNodeSelected(Node* node, bool setSelected);
    void view_SetEdgeSelected(Edge* edge, bool setSelected);

    void view_SetItemSelected(GraphML* item, bool setSelected);

    void view_SetNodeCentered(Node* node);
    void view_ConstructChildNode(QPointF centerPoint);
    void view_ConstructChildNode();

    void view_ConstructEdge(Node* src, Node* dst);
    void view_ConstructEdge(Node* src, Node* dst, QVector<GraphMLData*>data, QString previousID=0);
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

    QStringList getAdoptableNodes(Node* parent);
    //Gets a specific Attribute from the current Element in the XML. returns "" if none.
    QString getXMLAttribute(QXmlStreamReader& xml, QString attrID);

    //Constructs a GraphMLKey Object from a XML entity.
    GraphMLKey* parseGraphMLKeyXML(QXmlStreamReader& xml);

    //Construct a GraphMLKey.
    GraphMLKey* constructGraphMLKey(QString name, QString type, QString forString);



    Node* constructNodeEntity(QVector<GraphMLData*> dataToAttach);
    Node* constructNodeChild(Node* parentNode, QVector<GraphMLData*> dataToAttach);

    QVector<GraphMLData*> constructGraphMLDataVector(QString nodeKind, QPointF relativePosition);
    QVector<GraphMLData*> constructGraphMLDataVector(QString nodeKind);


    Node* constructNodeInstance(Node* parent, Node* definition,  bool forceCreate = false);
    Node* constructNodeImpl(Node* parent, Node* definition,  bool forceCreate = false);


    Node* constructGraphMLNode(QVector<GraphMLData *> data, Node *parent = 0);

    //Connects Node object and stores into a vector.
    void setupNode(Node* node);

    void setupModel();
    void setupValidator();

    void setupInstance(Node* definition, Node* instance);
    void setupImpl(Node* definition, Node* implementation);
    void setupAggregate(EventPort* eventPort, Aggregate* aggregate);
    void tearDownImpl(Node* definition, Node* implementation);

    void teardownInstance(Node* definition, Node* instance);

    bool isGraphMLValid(QString inputGraphML);


    //Connects Edge object and stores into a vector.
    void setupEdge(Edge* edge);
    void updateGUIUndoRedoLists();

    //Selection methods
    Node* getSelectedNode();
    Edge* getSelectedEdge();

    bool deleteNode(Node* node);
    bool deleteEdge(Edge* edge, bool addAction = true);

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
    void attachGraphMLData(GraphML* item, QVector<GraphMLData*> dataList);
    
    void addActionToStack(ActionItem action);


    void undoRedo();

    QStack<ActionItem> undoStack;
    QStack<ActionItem> redoStack;


    NodeEdge* getNodeEdgeFromEdge(Edge* edge);
    NodeItem* getNodeItemFromNode(Node* node);

    GraphML* getGraphMLFromID(QString ID);
    Node* getNodeFromID(QString ID);
    NodeItem* getNodeItemFromGraphML(GraphML* item);
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

    QStringList containerNodeKinds;
    QStringList constructableNodeKinds;

    QStringList viewAspects;
    QStringList protectedKeyNames;

    bool UNDOING;
    bool REDOING;
    bool SELECT_NEWLY_CREATED;

    bool KEY_CONTROL_DOWN;
    bool KEY_SHIFT_DOWN;

    qreal HIDDEN_OPACITY;

    NodeView* view;

    Model* getParentModel();

    Model* model;
    BehaviourDefinitions* behaviourDefinitions;
    DeploymentDefinitions* deploymentDefinitions;
    InterfaceDefinitions* interfaceDefinitions;
    HardwareDefinitions* hardwareDefinitions;
    AssemblyDefinitions* assemblyDefinitions;

    bool CUT_LINKING;
    int actionCount;
    QString currentAction;
    int currentActionID;

    QString childNodeKind;

    QStandardItemModel* treeModel;
    ValidationEngine* validator;
};

#endif // NEWCONTROLLER_H
