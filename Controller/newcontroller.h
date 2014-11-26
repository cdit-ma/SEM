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
enum MESSAGE_TYPE{CRITICAL, WARNING, MESSAGE};

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

    //In the form KeyName, KeyType, KeyFor, Data Value, isProtected.
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

    QStringList getNodeKinds();
    QStringList getViewAspects();

signals:
    void view_SetGUIEnabled(bool setEnabled);
    //Inform the View to Center the Canvas around an Node.

    void view_CenterGraphML(GraphML* graphML);

    void view_SetOpacity(GraphML* graphML, qreal opacity);

    void view_SetRubberbandSelectionMode(bool on);

    void view_WriteGraphML(QString filename, QString data);

    void view_UpdateUndoList(QStringList list);
    void view_UpdateRedoList(QStringList list);

    void view_UpdateProgressBar(int percentage, QString label="");

    void view_UpdateCopyBuffer(QString data);

    void view_UpdateProjectName(QString name);

    void view_PrintErrorCode(GraphML* graphml, QString text);
    void view_UpdateStatusText(QString statusText);

    void view_DialogMessage(MESSAGE_TYPE type, QString message);

    void view_ConstructNodeGUI(Node* node);
    void view_ConstructEdgeGUI(Edge* node);

public slots:
    void view_ImportGraphML(QStringList inputGraphML, Node *currentParent=0);
    void view_ImportGraphML(QString, Node *currentParent=0, bool linkID = false);

    void validator_HighlightError(Node* node, QString error);
    void view_ValidateModel();

    //Informants from the AttributeTableModel Class.
    void view_UpdateGraphMLData(GraphML* parent, QString keyName, QString dataValue);
    void view_ConstructGraphMLData(GraphML* parent, QString keyName);
    void view_DestructGraphMLData(GraphML* parent, QString keyName);


    void view_ConstructComponentInstance(Component* definition = 0);
    void view_ConstructComponentImpl(Component* definition = 0);

    void view_CenterComponentImpl(Node* node = 0);
    void view_CenterComponentDefinition(Node* node = 0);
    void view_CenterAggregate(Node* node = 0);

    void view_ProjectNameUpdated(GraphMLData* label);



    void view_ConstructMenu(QPoint position);

    void view_ExportGraphML(QString filename);

    void view_GraphMLSelected(GraphML* item, bool setSelected);

    void view_SetNodeSelected(Node* node, bool setSelected);
    void view_SetEdgeSelected(Edge* edge, bool setSelected);


    void view_ConstructChildNode(QPointF centerPoint);
    void view_ConstructChildNode();

    void view_ConstructEdge(Node* src, Node* dst);
    void view_ConstructEdge(Node* src, Node* dst, QVector<GraphMLData*> data, QString previousID="");
    void view_ConstructEdge(Node* src, Node* dst, QVector<QStringList> data, QString previousID="");
    void view_MoveSelectedNodes(QPointF delta);

    void view_SetChildNodeKind(QString nodeKind);

    void view_FilterNodes(QStringList filterString);

    void view_HideUnconnectableNodes(Node* node);
    void view_ShowAllNodes();


    void view_TriggerAction(QString actionName);

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


private:
    //Consolidated Methods

    //Copies the selected Nodes' GraphML representation to the Clipboard.
    //Returns true if succeeded
    bool copySelectedNodesGraphML();

    //Finds or Constructs a GraphMLKey given a Name, Type and ForType
    GraphMLKey* constructGraphMLKey(QString name, QString type, QString forString);

    //Finds or Constructs a Node Instance or Implementation inside parent of Definition.
    Node* constructNodeInstance(Node* parent, Node* definition);
    Node* constructNodeImplementation(Node* parent, Node* definition);

    //Returns a list of Kinds which can be adopted by a Node.
    QStringList getAdoptableNodeKinds(Node* parent);
    //Gets a specific Attribute from the current Element in the XML.
    //Returns "" if no Attribute found.
    QString getXMLAttribute(QXmlStreamReader& xml, QString attributeID);

     //Constructs a Node using the attached GraphMLData elements. Does not attach this Node.
    Node* constructNode(QVector<GraphMLData*> dataToAttach);
    Edge* constructEdge(Node* source, Node* destination);

    void storeGraphMLInHash(GraphML* item);
    GraphML* getGraphMLFromHash(QString ID);
    void removeGraphMLFromHash(QString ID);

    //Constructs a Node using the attached GraphMLData elements. Attachs the node to the parentNode provided.
    Node* constructNodeChild(Node* parentNode, QVector<GraphMLData*> dataToAttach);

    //Sets up an Undo state for the creation of the Node/Edge, and tells the View To construct a GUI Element.
    void constructNodeGUI(Node* node);
    void constructEdgeGUI(Edge* edge);

    //Sets up an Undo state for the deletion of the Node/Edge, and tells the View To destruct its GUI Element.
    bool destructNode(Node* node);
    bool destructEdge(Edge* edge, bool addAction = true);

    //Constructs a Vector of basic GraphMLData entities required for creating a Node.
    QVector<GraphMLData*> constructGraphMLDataVector(QString nodeKind, QPointF relativePosition);
    QVector<GraphMLData*> constructGraphMLDataVector(QString nodeKind);

    //Constructs and setups all required Entities inside the Model Node.
    void setupModel();

    //Sets up the Validation Engine.
    void setupValidator();

    //Binds matching GraphMLData elements from the Node Child, to the Node Definition.
    void bindGraphMLData(Node* definition, Node* child);

    //Setup/Teardown the node provided an Instance of the Definition. It will adopt Instances of all Definitions contained by definition and bind all GraphMLData which isn't protected.
    void setupNodeAsInstance(Node* definition, Node* node);
    void teardownNodeAsInstance(Node* definition, Node* instance);

    //Setup/Teardown the node provided an Implementation of the Definition. It will adopt Implementations of all Definitions contained by definition and bind all GraphMLData which isn't protected.
    void setupNodeAsImplementation(Node* definition, Node* node);
    void teardownNodeAsImplementation(Node* definition, Node* implementation);

    //Attaches an Aggregate Definition to an EventPort Definition.
    void attachAggregateToEventPort(EventPort* eventPort, Aggregate* aggregate);

    //Checks to see if the provided GraphML document is Valid XML.
    bool isGraphMLValid(QString inputGraphML);

    //Updates the attached views list of Undo/Redo States.
    void updateViewUndoRedoLists();

    //Gets the currently selected Node/Edge only if there is 1 Node/Edge selected.
    Node* getSelectedNode();
    Edge* getSelectedEdge();


    //Sets the Node/Edge as selected. Calls Methods in the View to visually select the item's GUI.
    void setNodeSelected(Node* node, bool setSelected = true);
    void setEdgeSelected(Edge* edge, bool setSelected = true);

    //Calls the GUI to unselect the currently Selected Node/Edges.
    void clearSelectedNodes();
    void clearSelectedEdges();

    //Deletes the Model Entities corresponding to the selected Node/Edges.
    void deleteSelectedNodes();
    void deleteSelectedEdges();

    //Returns true If a Node's parent (Recursive) is already selected.
    bool isNodesAncestorSelected(Node* node);

    //Returns true if a Node/Edge is already selected.
    bool isNodeSelected(Node* node);
    bool isEdgeSelected(Edge* edge);

    //Returns true if the Model Entities can be connected.
    bool isEdgeLegal(Node* src, Node* dst);

    //Returns true if a nodeKind has been Implemented in the Model.
    bool isNodeKindImplemented(QString nodeKind);

    //Used by Undo/Redo to reverse an ActionItem from the Stacks.
    void reverseAction(ActionItem action);

    //Adds an ActionItem to the Undo/Redo Stack depending on the State of the application.
    void addActionToStack(ActionItem action);

    //Undo's/Redo's all of the ActionItems in the Stack which have been performed since the last operation.
    void undoRedo(bool undo=true);

    //Attach GraphMLData('s) to the GraphML item.
    bool attachGraphMLData(GraphML* item, GraphMLData* data);
    bool attachGraphMLData(GraphML* item, QVector<QStringList> dataList);
    bool attachGraphMLData(GraphML* item, QVector<GraphMLData*> dataList);
    
    //Gets the GraphML/Node/Edge Item from the ID provided. Checks the Hash.
    GraphML* getGraphMLFromID(QString ID);
    Node* getNodeFromID(QString ID);
    Edge* getEdgeFromID(QString ID);



    //Stores the GraphMLKey's used by the Model.
    QVector<GraphMLKey*> keys;

    QVector<Edge *> edges;
    QVector<Node *> nodes;

    //Stores the list of nodeID's and EdgeID's inside the Hash.
    QStringList nodeIDs;
    QStringList edgeIDs;

    //Stack of ActionItems in the Undo Stack.
    QStack<ActionItem> undoActionStack;
    //Stack of ActionItems in the Redo Stack.
    QStack<ActionItem> redoActionStack;

    //Selection Lists
    QVector<Edge *> selectedEdges;
    QVector<Node *> selectedNodes;

    //Used to find old ID's which may have been deleted from the Model. Will find the replacement ID if they exist.
    QString getIDFromOldID(QString ID);

    //Links an ID of an already deleted GraphML Item to the ID of the new GraphML Item.
    void linkOldIDToID(QString oldID, QString newID);

    //Casts the GraphML as a Node/Edge, will be NULL if not a Node/Edge
    Node* getNodeFromGraphML(GraphML* item);
    Edge* getEdgeFromGraphML(GraphML* item);

    //Gets the Model Node.
    Model* getModel();


    //Provides a lookup for old IDs.
    QHash<QString, QString> IDLookupHash;
    QHash<QString, GraphML*> IDLookupGraphMLHash;

    QPointF menuPosition;

    Node* centeredNode;

    QStringList containerNodeKinds;
    QStringList constructableNodeKinds;

    QStringList viewAspects;
    QStringList protectedKeyNames;

    bool UNDOING;
    bool REDOING;

    bool KEY_CONTROL_DOWN;
    bool KEY_SHIFT_DOWN;

    qreal HIDDEN_OPACITY;

    NodeView* view;



    Model* model;
    BehaviourDefinitions* behaviourDefinitions;
    DeploymentDefinitions* deploymentDefinitions;
    InterfaceDefinitions* interfaceDefinitions;
    HardwareDefinitions* hardwareDefinitions;
    AssemblyDefinitions* assemblyDefinitions;

    int actionCount;
    QString currentAction;
    int currentActionID;

    QString childNodeKind;

    ValidationEngine* validator;
};

#endif // NEWCONTROLLER_H
