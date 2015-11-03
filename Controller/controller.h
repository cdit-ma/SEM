#ifndef NEWCONTROLLER_H
#define NEWCONTROLLER_H
#include <QStack>
#include "../Model/model.h"
#include "../View/GraphicsItems/edgeitem.h"
#include <QFile>


enum ACTION_TYPE {CONSTRUCTED, DESTRUCTED, MODIFIED};
enum MESSAGE_TYPE{CRITICAL, WARNING, MESSAGE, MODEL};

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
    int parentID;
    QString itemLabel;
    QString itemKind;
    int srcID;
    int dstID;
    int ID;
    QString keyName;
    QString dataValue;
    QString removedXML;

    int actionItemID;
    QString timestamp;

    //In the form KeyName, KeyType, KeyFor, Data Value, isProtected.
    QList<QStringList> dataValues;
    //In the form ID
    QList<QList<int> > boundDataIDs;
    QList<int> parentDataID;

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

    //Gets the Model Node.
    Model* getModel();



    //Get a list of all Node Kinds
    QStringList getNodeKinds(Node* parent);
    //Get a list of all View Aspects
    QStringList getViewAspects();

    QStringList getAllNodeKinds();
    QStringList getGUIConstructableNodeKinds();
    //Returns a list of Kinds which can be adopted by a Node.
    QStringList getAdoptableNodeKinds(int ID);
    //QStringList getAdoptableNodeKinds(Node* parent);

    QList<int> getConnectableNodes(int srcID);
    QList<int> getConnectedNodes(int ID);

    QStringList getValidKeyValues(QString keyName, int ID =-1);
    QList<int> getNodesOfKind(QString kind, int ID=-1, int depth=-1);

    bool canCopy(QList<int> selection);
    bool canReplicate(QList<int> selection);
    bool canCut(QList<int> selection);
    bool canDelete(QList<int> selection);
    bool canPaste(QList<int> selection);
    bool canExportSnippet(QList<int> selection);
    bool canImportSnippet(QList<int> selection);
    bool canUndo();
    bool canRedo();
    bool canLocalDeploy();

    int getDefinition(int ID);
    int getImplementation(int ID);
    QList<int> getInstances(int ID);
    int getAggregate(int ID);
    int getDeployedHardwareID(int ID);

    int getSharedParent(int ID1, int ID2);

    int getContainedAspect(int ID);


signals:
    void controller_ModelReady();
    void controller_ActionProgressChanged(int percent, QString action="");
    void controller_ActionFinished();

    void controller_AskQuestion(MESSAGE_TYPE, QString title, QString message, int ID=-1);
    void controller_GotQuestionAnswer();
    void controller_DisplayMessage(MESSAGE_TYPE, QString title, QString message, int ID=-1);

    void controller_ExportedProject(QString);
    void controller_ExportedSnippet(QString parentName, QString snippetXMLData);

    void controller_GraphMLConstructed(GraphML*);
    void controller_GraphMLDestructed(int ID, GraphML::KIND kind);

    void controller_ProjectNameChanged(QString);


    void controller_SetClipboardBuffer(QString);

    void controller_SetViewEnabled(bool);
private slots:
    void enableDebugLogging(bool logMode, QString applicationPath="");

    void connectViewAndSetupModel(NodeView* view);
    //Clipboard functionality
    void cut(QList<int> IDs);
    void copy(QList<int> IDs);
    void paste(int ID, QString xmlData);
    void replicate(QList<int> IDs);
    void remove(QList<int> IDs);

    void importSnippet(QList<int> IDs, QString fileName, QString fileData);
    void exportSnippet(QList<int> IDs);

    //Toolbar/Dock Functionality
    void clear();

    void undo();
    void redo();

    void constructNode(int parentID, QString nodeKind, QPointF centerPoint);
    void constructEdge(int srcID, int dstID, bool reverseOkay = false);
    void destructEdge(int srcID, int dstID);
    void constructConnectedNode(int parentID, int connectedID, QString kind, QPointF relativePos);
    void setGraphMLData(int parentID, QString keyName, QString dataValue);

    void constructDestructMultipleEdges(QList<int> srcIDs, int dstID);
    void constructDestructEdges(QList<int> destruct_srcIDs, QList<int> destruct_dstIDs, QList<int> construct_srcIDs, int dstID);

    void triggerAction(QString actionName);


    void importProjects(QStringList xmlDataList);
    void exportProject();


    void gotQuestionAnswer(bool answer);


    //MODEL Functionality
    void displayMessage(QString title, QString message, int ID);

        void clearHistory();

private:
    //Helper functions.
    bool _paste(int ID, QString xmlData, bool addAction = true);
    bool _cut(QList<int> IDs, bool addAction = true);
    bool _clear();
    bool _copy(QList<int> IDs);
    bool _remove(QList<int> IDs, bool addAction = true);
    bool _replicate(QList<int> IDs, bool addAction = true);
    bool _importProjects(QStringList xmlDataList, bool addAction = true);
    bool _importSnippet(QList<int> IDs, QString fileName, QString fileData, bool addAction = true);
    bool _exportSnippet(QList<int> IDs);
    bool _exportProject();


private:
    void attachGraphMLData(GraphML* parent, GraphMLData* data, bool addAction = true);
    void destructGraphMLData(GraphML* parent, QString keyName, bool addAction = true);






private:
    void setupParameters();
    void setGraphMLData(GraphML* parent, QString keyName, QString dataValue, bool addAction = true);
    void clearUndoHistory();

    bool askQuestion(MESSAGE_TYPE, QString questionTitle, QString question, int ID=-1);
    Node* getSingleNode(QList<int> IDs);
    bool _importGraphMLXML(QString document, Node* parent = 0, bool linkID=false, bool resetPos=false);



    Node* getSharedParent(QList<int> IDs);



    //Exports a Selection of Containers to export into GraphML
    QString _exportGraphMLDocument(QList<int> nodeIDs, bool allEdges = false, bool GUI_USED=false);
    QString _exportGraphMLDocument(Node* node, bool allEdges = false, bool GUI_USED=false);

    //Finds or Constructs a GraphMLKey given a Name, Type and ForType
    GraphMLKey* constructGraphMLKey(QString name, QString type, QString forString);
    GraphMLKey* getGraphMLKeyFromName(QString name);

    //Finds or Constructs a Node Instance or Implementation inside parent of Definition.
    int constructDefinitionRelative(Node* parent, Node* definition, bool instance = true);

    //Gets a specific Attribute from the current Element in the XML.
    //Returns "" if no Attribute found.
    QString getXMLAttribute(QXmlStreamReader& xml, QString attributeID);

    Edge* _constructEdge(Node* source, Node* destination);
    Edge* constructEdgeWithGraphMLData(Node* source, Node* destination, QList<GraphMLData*> data = QList<GraphMLData*>(), int previousID=-1);
    Edge* constructEdgeWithData(Node* source, Node* destination, QList<QStringList> data = QList<QStringList>(), int previousID=-1);

    //Stores/Gets/Removes items/IDs from the GraphML Hash
    void storeGraphMLInHash(GraphML* item);
    GraphML* getGraphMLFromHash(int ID);
    void removeGraphMLFromHash(int ID);

    //Constructs a Node using the attached GraphMLData elements. Attachs the node to the parentNode provided.
    Node* constructChildNode(Node* parentNode, QList<GraphMLData*> dataToAttach);


    //Sets up an Undo state for the creation of the Node/Edge, and tells the View To construct a GUI Element.
    void constructNodeGUI(Node* node);
    void constructEdgeGUI(Edge* edge);

    //Sets up an Undo state for the deletion of the Node/Edge, and tells the View To destruct its GUI Element.
    bool destructNode(Node* node, bool addAction = true);
    bool destructEdge(Edge* edge, bool addAction = true);


    //Constructs a Vector of basic GraphMLData entities required for creating a Node.
    QList<GraphMLData*> constructGraphMLDataVector(QString nodeKind, QPointF relativePosition = QPointF(-1,-1));

    QString getNodeInstanceKind(Node* definition);
    QString getNodeImplKind(Node* definition);

    //Constructs and setups all required Entities inside the Model Node.
    void setupModel();

    //Binds matching GraphMLData elements from the Node Child, to the Node Definition.
    void bindGraphMLData(Node* definition, Node* instance);
    void unbindGraphMLData(Node* definition, Node* instance);

    //Setup/Teardown the node provided an Instance of the Definition. It will adopt Instances of all Definitions contained by definition and bind all GraphMLData which isn't protected.
    bool setupDefinitionRelationship(Node* definition, Node* node, bool instance);
    bool teardownDefinitionRelationship(Node* definition, Node* node, bool instance);

    //Attaches an Aggregate Definition to an EventPort Definition.
    bool setupAggregateRelationship(EventPort* eventPort, Aggregate* aggregate);
    bool teardownAggregateRelationship(EventPort* EventPort, Aggregate* aggregate);

    bool setupVectorRelationship(Vector* vector, Aggregate* aggregate);
    bool teardownVectorRelationship(Vector* vector, Aggregate* aggregate);


    //Checks to see if the provided GraphML document is Valid XML.
    bool isGraphMLValid(QString inputGraphML);


    void setupManagementComponents();
    void setupLocalNode();

    void enforceUniqueLabel(Node* node, QString newLabel = "");
    void enforceUniqueSortOrder(Node* node, int position =-1);


    //Returns true if the Model Entities can be connected.
    bool isEdgeLegal(Node* src, Node* dst);

    //Returns true if a nodeKind has been Implemented in the Model.
    bool isNodeKindImplemented(QString nodeKind);

    //Used by Undo/Redo to reverse an ActionItem from the Stacks.
    bool reverseAction(ActionItem action);

    //Adds an ActionItem to the Undo/Redo Stack depending on the State of the application.
    void addActionToStack(ActionItem action, bool addAction=true);

    //Undo's/Redo's all of the ActionItems in the Stack which have been performed since the last operation.
    void undoRedo(bool undo=true);

    void logAction(ActionItem item);

    bool canDeleteNode(Node* node);


    Node* constructTypedNode(QString nodeKind, QString nodeType="", QString nodeLabel="");


    //Attach GraphMLData('s) to the GraphML item.
    bool _attachGraphMLData(GraphML* item, GraphMLData* data, bool addAction = true);
    bool _attachGraphMLData(GraphML* item, QList<QStringList> dataList, bool addAction = true);
    bool _attachGraphMLData(GraphML* item, QList<GraphMLData*> dataList, bool addAction = true);
    
    //Gets the GraphML/Node/Edge Item from the ID provided. Checks the Hash.
    GraphML* getGraphMLFromID(int ID);
    Node* getNodeFromID(int ID);
    Node* getFirstNodeFromList(QList<int> ID);
    Edge* getEdgeFromID(int ID);

    //Used to find old ID's which may have been deleted from the Model. Will find the replacement ID if they exist.
    int getIDFromOldID(int ID);

    int getIntFromQString(QString ID);
    //Links an ID of an already deleted GraphML Item to the ID of the new GraphML Item.
    void linkOldIDToID(int oldID, int newID);

    //Casts the GraphML as a Node/Edge, will be NULL if not a Node/Edge
    Node* getNodeFromGraphML(GraphML* item);
    Edge* getEdgeFromGraphML(GraphML* item);

    bool isGraphMLInModel(GraphML* item);


    QString getSysOS();
    QString getSysArch();
    QString getSysOSVersion();

    //Stores the GraphMLKey's used by the Model.
    QList<GraphMLKey*> keys;

    //Stores the list of nodeID's and EdgeID's inside the Hash.
    QList<int> nodeIDs;
    QList<int> edgeIDs;

    //Stack of ActionItems in the Undo/Redo Stack.
    QStack<ActionItem> undoActionStack;
    QStack<ActionItem> redoActionStack;

    QString getTimeStamp();
    QString getDataValueFromKeyName(QList<GraphMLData*> dataList, QString keyName);

    //Provides a lookup for IDs.
    QHash<int, int> IDLookupHash;
    QHash<int, GraphML*> IDLookupGraphMLHash;
    QHash<int, QString> reverseKindLookup;

    QHash<QString, QList<int> > kindLookup;

    QHash<QString, int> treeLookup;
    QHash<int, QString> reverseTreeLookup;


    //A list of Node's which are considered Containers, and aren't part of constructable Nodes.
    QStringList containerNodeKinds;
    //A List of Node's which are elements in the Model, can be constructed.
    QStringList constructableNodeKinds;
    QStringList guiConstructableNodeKinds;

    QStringList snippetableParentKinds;
    QStringList nonSnippetableKinds;

    QStringList behaviourNodeKinds;
    QStringList definitionNodeKinds;

    //A list of View Aspects present in the model.
    QStringList viewAspects;
    //A list of KeyNames to be protected.
    QStringList protectedKeyNames;

    //Used to tell if we are currently Undo-ing/Redo-ing in the system.
    bool UNDOING;
    bool REDOING;
    bool USE_LOGGING;
    bool IMPORTING_SNIPPET;

    Model* model;

    Node* behaviourDefinitions;
    Node* deploymentDefinitions;
    Node* interfaceDefinitions;
    Node* hardwareDefinitions;
    Node* assemblyDefinitions;
    Node* localhostNode;

    //List of undeleteable nodes
    QList<Node*> protectedNodes;


    QHash<QString, ManagementComponent*> managementComponents;
    QHash<QString, HardwareNode*> hardwareNodes;
    QHash<QString, HardwareCluster*> hardwareClusters;

    int previousUndos;

    QList<int> connectedLinkedIDs;

    bool CUT_USED;
    int actionCount;
    QString currentAction;
    QFile* logFile;
    int currentActionID;
    int currentActionItemID;

    bool questionAnswer;

    bool DELETING;


};

#endif // NEWCONTROLLER_H
