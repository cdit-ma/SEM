#ifndef NEWCONTROLLER_H
#define NEWCONTROLLER_H
#include <QStack>
#include "../GUI/nodeedge.h"
#include "../Model/model.h"
#include <QStandardItemModel>
#include "../Model/node.h"

//Include Validation Engine
#include "../ValidationEngine/validationengine.h"
#include "../ValidationEngine/Plugins/interfacedefinitionplugin.h"

enum ACTION_TYPE {CONSTRUCTED, DESTRUCTED, MODIFIED};
enum MESSAGE_TYPE{CRITICAL, WARNING, MESSAGE};

struct EdgeTemp
{
    QString id;
    qint64 lineNumber;
    QString source;
    QString target;
    QList<GraphMLData *> data;
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
    QList<QStringList> dataValues;
    //In the form ID
    QList<QStringList> boundDataIDs;
    QList<QString> parentDataID;

    QString boundDataID;
    QString actionName;
    int actionID;
};


class NodeView;
class GraphMLView;
class NewController: public QObject
{
    Q_OBJECT
public:
    NewController();
    ~NewController();

    void initializeModel();
    void connectView(NodeView* view);


   // void connectView(GraphMLView* view);

    //Get a list of all Node Kinds
    QStringList getNodeKinds();
    //Get a list of all View Aspects
    QStringList getViewAspects();

    //Returns a list of Kinds which can be adopted by a Node.
    QStringList getAdoptableNodeKinds(Node* parent = 0);
    //QStringList getAdoptableNodeKinds(Node* parent);

    Node* getSelectedNode();


signals:
    //Triggers the View to Enable/Disable the GUI
    void view_SetGUIEnabled(bool setEnabled);

    //Triggers the View to Center the Canvas around a GraphML Entity.
    void view_CenterGraphML(GraphML* graphML);

    //Triggers the View to adjust the Opacity of the GraphML Entity.
    void view_SetOpacity(GraphML* graphML, qreal opacity);

    //Called when a GUI Item is triggered to be selected.
    void view_SetGraphMLSelected(GraphML* item, bool setSelected=true);

    //Triggers the View to write the GraphML XML data specified to the filename specified.
    void view_WriteGraphML(QString filename, QString data);

    //Triggers the View to update the Undo/Redo History with the lists provided.
    void view_UpdateUndoList(QStringList list);
    void view_UpdateRedoList(QStringList list);

    //Triggers the view to update a progress bar for the current Action.
    void view_UpdateProgressBar(int percentage, QString label="");

    //Triggers the View to update the Copy Buffer with the data provided.
    void view_UpdateCopyBuffer(QString data);

    //Triggers the View to update the Visual Project Name.
    void view_UpdateProjectName(QString name);

    //Triggers the View that an error was found related to the GraphML entity, and error text provided.
    void view_PrintErrorCode(GraphML* graphml, QString text);

    //Triggers the View to Update the status of the current operation.
    void view_UpdateStatusText(QString statusText);

    //Triggers the View to Print a Dialog Box with the message and type provided.
    void view_DialogMessage(MESSAGE_TYPE type, QString message);

    //Tells the View to construct a New GUI Item for the GraphMLItem
    void view_ConstructGraphMLGUI(GraphML* item);
    void view_DestructGraphMLGUIFromID(QString ID);

    void view_SortNode(Node* item);

    void view_FitToScreen();
    void centreNode(Node* node);

    void setLegalNodesList(QList<Node*>* nodes);

public slots:
    void centerNode(QString nodeLabel);
    void view_SelectModel();

    //UNUSED


    void view_ValidateModel();
    void validator_HighlightError(Node* node, QString error);

    //Used to Import a GraphML XML document from the GUI/Paste/Undo/Redo
    void view_ImportGraphML(QStringList inputGraphML, Node *currentParent = 0);
    void view_ImportGraphML(QString, Node *currentParent=0, bool linkID = false);


    //Used to Export a GraphML XML document representation of the Model.
    void view_ExportGraphML(QString filename);

    //Informants from the AttributeTableModel Class.
    void view_ConstructGraphMLData(GraphML* parent, QString keyName);
    void view_DestructGraphMLData(GraphML* parent, QString keyName);
    void view_UpdateGraphMLData(GraphML* parent, QString keyName, QString dataValue);

    //Used by the Right Click Menu
    void view_CenterComponentImpl(Node* node = 0);
    void view_CenterComponentDefinition(Node* node = 0);
    void view_CenterAggregate(Node* node = 0);

    //Called when the User Renames the GraphMLData* Label attached to the Model.
    void view_ProjectNameUpdated(GraphMLData* label);

    //Called when a GUI Item is triggered to be selected.
    void view_GraphMLSelected(GraphML* item, bool setSelected);

    //Called by the QAction from the Menu
    void view_ConstructNode(QString kind, QPointF centerPoint);

    void view_ConstructComponentInstanceInAssembly(Component* definition, ComponentAssembly* assembly = 0);

    //Clears the Model
    void view_ClearModel();

    //Constructs an Edge with no data between Source and Destination Nodes.
    void view_ConstructEdge(Node* source, Node* destination);

    //Moves all Nodes that are selected by delta.
    void view_MoveSelectedNodes(QPointF delta);
    void view_SelectFromID(QString ID);

    //Hides all Nodes which don't match the List of Filters provided.
    void view_FilterNodes(QStringList filterString);

    void view_SortModel();
    void view_SortDeployment();

    //Hides all Nodes which can't be connected to Node
    void view_ShowLegalEdgesForNode(Node* node);

    //Shows aLL nodes.
    void view_ShowAllNodes();

    //Used to trigger an Action to add an item into the Undo/Redo Stack.
    void view_TriggerAction(QString actionName);

    //Clears the Undo/Redo Action Stacks.
    void view_ClearUndoRedoStacks();

    //Keyboard Actions
    void view_ControlPressed(bool isDown);
    void view_ShiftPressed(bool isDown);
    void view_DeletePressed(bool isDown=true);
    void view_ClearKeyModifiers();

    //GUI Functionality
    void view_Undo();
    void view_Redo();
    void view_Copy();
    void view_Cut();
    void view_Paste(QString xmlData);

    //Selection Actions.
    void view_SelectAll();
    void view_UncenterGraphML();
    void view_ClearSelection();

    void getLegalNodesList(Node* src);
    void constructLegalEdge(Node *src, Node *dst);

    void constructComponentInstance(Node* definition, QPointF center);

private:
    //Copies the selected Nodes' GraphML representation to the Clipboard.
    //Returns true if succeeded
    bool copySelectedNodesGraphML();

    //Get a list of all NodsortModele ID's
    QStringList getNodeIDS();

    //Exports a Selection of Containers to export into GraphML
    QString exportGraphML(QStringList nodeIDs);
    QString exportGraphML(Node* node);

    //Finds or Constructs a GraphMLKey given a Name, Type and ForType
    GraphMLKey* constructGraphMLKey(QString name, QString type, QString forString);

    //Finds or Constructs a Node Instance or Implementation inside parent of Definition.
    Node *constructDefinitionRelative(Node* parent, Node* definition, bool instance = true);

    QList<GraphMLData*> getDefinitionData(Node* definition, bool instance = true);

    //Gets a specific Attribute from the current Element in the XML.
    //Returns "" if no Attribute found.
    QString getXMLAttribute(QXmlStreamReader& xml, QString attributeID);

     //Constructs a Node using the attached GraphMLData elements. Does not attach this Node.
    Node* constructNode(QList<GraphMLData*> dataToAttach, bool readOnly = false);
    Edge* constructEdge(Node* source, Node* destination);
    Edge* constructEdgeWithData(Node* source, Node* destination, QList<GraphMLData*> data, QString previousID="");
    Edge* constructEdgeWithData(Node* source, Node* destination, QList<QStringList> data, QString previousID="");

    //Stores/Gets/Removes items/IDs from the GraphML Hash
    void storeGraphMLInHash(GraphML* item);
    GraphML* getGraphMLFromHash(QString ID);
    void removeGraphMLFromHash(QString ID);

    //Constructs a Node using the attached GraphMLData elements. Attachs the node to the parentNode provided.
    Node* constructChildNode(Node* parentNode, QList<GraphMLData*> dataToAttach);


    //Sets up an Undo state for the creation of the Node/Edge, and tells the View To construct a GUI Element.
    void constructNodeGUI(Node* node);
    void constructEdgeGUI(Edge* edge);

    //Sets up an Undo state for the deletion of the Node/Edge, and tells the View To destruct its GUI Element.
    bool destructNode(Node* node, bool addAction = true);
    bool destructEdge(Edge* edge, bool addAction = true);

    //Constructs a Vector of basic GraphMLData entities required for creating a Node.
    QList<GraphMLData*> constructGraphMLDataVector(QString nodeKind, QPointF relativePosition);
    QList<GraphMLData*> constructGraphMLDataVector(QString nodeKind);

    //Constructs and setups all required Entities inside the Model Node.
    void setupModel();

    //Sets up the Validation Engine.
    void setupValidator();

    //Binds matching GraphMLData elements from the Node Child, to the Node Definition.
    void bindGraphMLData(Node* definition, Node* node);

    //Setup/Teardown the node provided an Instance of the Definition. It will adopt Instances of all Definitions contained by definition and bind all GraphMLData which isn't protected.
    void setupDefinitionRelationship(Node* definition, Node* node, bool instance);
    void teardpwmDefinitionRelationship(Node* definition, Node* node, bool instance);

    //Attaches an Aggregate Definition to an EventPort Definition.
    void attachAggregateToEventPort(EventPort* eventPort, Aggregate* aggregate);

    //Checks to see if the provided GraphML document is Valid XML.
    bool isGraphMLValid(QString inputGraphML);

    //Updates the attached views list of Undo/Redo States.
    void updateViewUndoRedoLists();

    void setupManagementComponents();

    //Gets the currently selected Node/Edge only if there is 1 Node/Edge selected.
    //Node* getSelectedNode();
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
    bool attachGraphMLData(GraphML* item, GraphMLData* data, bool addAction = true);
    bool attachGraphMLData(GraphML* item, QList<QStringList> dataList, bool addAction = true);
    bool attachGraphMLData(GraphML* item, QList<GraphMLData*> dataList, bool addAction = true);
    
    //Gets the GraphML/Node/Edge Item from the ID provided. Checks the Hash.
    GraphML* getGraphMLFromID(QString ID);
    Node* getNodeFromID(QString ID);
    Edge* getEdgeFromID(QString ID);

    //Used to find old ID's which may have been deleted from the Model. Will find the replacement ID if they exist.
    QString getIDFromOldID(QString ID);

    //Links an ID of an already deleted GraphML Item to the ID of the new GraphML Item.
    void linkOldIDToID(QString oldID, QString newID);

    //Casts the GraphML as a Node/Edge, will be NULL if not a Node/Edge
    Node* getNodeFromGraphML(GraphML* item);
    Edge* getEdgeFromGraphML(GraphML* item);

    bool isGraphMLInModel(GraphML* item);

    //Gets the Model Node.
    Model* getModel();

    //Stores the GraphMLKey's used by the Model.
    QList<GraphMLKey*> keys;

    //Stores the list of nodeID's and EdgeID's inside the Hash.
    QStringList nodeIDs;
    QStringList edgeIDs;

    //Selection Lists
    QStringList selectedNodeIDs;
    QStringList selectedEdgeIDs;

    //Stack of ActionItems in the Undo/Redo Stack.
    QStack<ActionItem> undoActionStack;
    QStack<ActionItem> redoActionStack;

    //Provides a lookup for old IDs.
    QHash<QString, QString> IDLookupHash;
    QHash<QString, GraphML*> IDLookupGraphMLHash;

    //The Element which is currently Centered
    GraphML* centeredGraphML;

    //A list of Node's which are considered Containers, and aren't part of constructable Nodes.
    QStringList containerNodeKinds;
    //A List of Node's which are elements in the Model, can be constructed.
    QStringList constructableNodeKinds;

    //A list of View Aspects present in the model.
    QStringList viewAspects;
    //A list of KeyNames to be protected.
    QStringList protectedKeyNames;

    //Used to tell if we are currently Undo-ing/Redo-ing in the system.
    bool UNDOING;
    bool REDOING;

    bool KEY_CONTROL_DOWN;
    bool KEY_SHIFT_DOWN;

    qreal HIDDEN_OPACITY;

    Model* model;
    BehaviourDefinitions* behaviourDefinitions;
    DeploymentDefinitions* deploymentDefinitions;
    InterfaceDefinitions* interfaceDefinitions;
    HardwareDefinitions* hardwareDefinitions;
    AssemblyDefinitions* assemblyDefinitions;

    //ManagementNodes
    QHash<QString, ManagementComponent*> managementComponents;
    QHash<QString, HardwareNode*> hardwareNodes;
    QHash<QString, HardwareCluster*> hardwareClusters;



    bool DELETING;
    bool CUT_USED;
    int actionCount;
    QString currentAction;
    int currentActionID;


    ValidationEngine* validator;
    };

#endif // NEWCONTROLLER_H
