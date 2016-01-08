#ifndef NEWCONTROLLER_H
#define NEWCONTROLLER_H
#include "../Model/model.h"
#include "../Model/workerdefinitions.h"
#include "entityadapter.h"


#include <QStack>
#include <QFile>
#include <QPointF>
#include <QXmlStreamReader>

#include "../Model/Edges/definitionedge.h"
#include "../Model/Edges/workflowedge.h"
#include "../Model/Edges/dataedge.h"
#include "../Model/Edges/assemblyedge.h"
#include "../Model/Edges/aggregateedge.h"
#include "../Model/data.h"


enum ACTION_TYPE {CONSTRUCTED, DESTRUCTED, MODIFIED};
enum MESSAGE_TYPE{CRITICAL, WARNING, MESSAGE, MODEL};

struct EdgeTemp
{
    QString id;
    qint64 lineNumber;
    QString source;
    QString target;
    QList<Data *> data;
};

struct ViewSignal{
    int id;
    GraphML::GRAPHML_KIND kind;
    bool constructSignal;
};


struct EventAction{
    struct _Action{
        int ID;
        int actionID;
        ACTION_TYPE type;
        GraphML::GRAPHML_KIND kind;
        QString name;
        QString timestamp;
    } Action;

    int ID;
    int parentID;

    struct _Entity{
        QString XML;
        QString nodeKind;
        Entity::ENTITY_KIND kind;
    } Entity;

    struct _Key{
        QString name;
        QVariant::Type type;
        Entity::ENTITY_KIND kind;
    } Key;

    struct _Data{
        QString keyName;
        QVariant oldValue;
        QVariant newValue;
        bool isProtected;
    } Data;
};


class NodeView;
class GraphMLView;
class NewController: public QObject
{
    Q_OBJECT
public:
    NewController();
    ~NewController();

    void setExternalWorkerDefinitionPath(QString path);
    void loadWorkerDefinitions();

    void initializeModel();
    void connectView(NodeView* view);

    //Gets the Model Node.
    Model* getModel();
    WorkerDefinitions* getWorkerDefinitions();




    //Get a list of all View Aspects
    QStringList getViewAspects();

    QStringList getAllNodeKinds();
    QStringList getGUIConstructableNodeKinds();
    //Returns a list of Kinds which can be adopted by a Node.
    QStringList getAdoptableNodeKinds(int ID);
    QList<int> getFunctionIDList();
    //QStringList getAdoptableNodeKinds(Node* parent);

    QList<int> getConnectableNodes(int srcID);
    QList<int> getConnectedNodes(int ID);

    QStringList getValidKeyValues(QString keyName, int ID =-1);
    QList<int> getNodesOfKind(QString kind, int ID=-1, int depth=-1);

    QString getData(int ID, QString key);

    bool isInModel(int ID);
    bool canCopy(QList<int> selection);
    bool canGetCPP(QList<int> selection);
    bool canReplicate(QList<int> selection);
    bool canCut(QList<int> selection);
    bool canDelete(QList<int> selection);
    bool canPaste(QList<int> selection);
    bool canExportSnippet(QList<int> selection);
    bool canImportSnippet(QList<int> selection);
    bool canSetReadOnly(QList<int> selection);
    bool canUnsetReadOnly(QList<int> selection);
    bool canUndo();
    bool canRedo();
    bool canLocalDeploy();

    bool isNodeAncestor(int ID, int ID2);

    bool areIDsInSameBranch(int mainID, int newID);

    int getDefinition(int ID);
    int getImplementation(int ID);
    QList<int> getInstances(int ID);
    int getAggregate(int ID);
    int getDeployedHardwareID(int ID);

    int getSharedParent(int ID1, int ID2);

    int getContainedAspect(int ID);


signals:
    void controller_CanUndo(bool ok);
    void controller_CanRedo(bool ok);
    void controller_ModelReady();
    void controller_ActionProgressChanged(int percent, QString action="");
    void controller_ActionFinished(bool actionSucceeded = true, QString errorCode = "");

    void controller_AskQuestion(MESSAGE_TYPE, QString title, QString message, int ID=-1);
    void controller_GotQuestionAnswer();
    void controller_DisplayMessage(MESSAGE_TYPE, QString title, QString message, int ID=-1);

    void controller_ExportedProject(QString);
    void controller_ExportedSnippet(QString parentName, QString snippetXMLData);

    void controller_GraphMLConstructed(Entity*);
    void controller_EntityConstructed(EntityAdapter*);
    void controller_EntityDestructed(EntityAdapter*);

    void controller_GraphMLDestructed(int ID, GraphML::GRAPHML_KIND kind);

    void controller_ProjectNameChanged(QString);


    void controller_ProjectRequiresSave(bool requiresSave);

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
    void setReadOnly(QList<int> IDs, bool readOnly);

    void importSnippet(QList<int> IDs, QString fileName, QString fileData);
    void exportSnippet(QList<int> IDs);

    //Toolbar/Dock Functionality
    void clear();

    void undo();
    void redo();

    void save();
    void saveAs();
    void open();

    void constructNode(int parentID, QString nodeKind, QPointF centerPoint);
    void constructWorkerProcessNode(int parentID,QString workerName, QString operationName, QPointF position);
    void constructEdge(int srcID, int dstID, bool reverseOkay = false);
    void destructEdge(int srcID, int dstID);
    void constructConnectedNode(int parentID, int connectedID, QString kind, QPointF relativePos);
    void setData(int parentID, QString keyName, QVariant dataValue);

    void constructDestructMultipleEdges(QList<int> srcIDs, int dstID);
    void constructDestructEdges(QList<int> destruct_srcIDs, QList<int> destruct_dstIDs, QList<int> construct_srcIDs, int dstID);

    void triggerAction(QString actionName);


    void importProjects(QStringList xmlDataList);
    void exportProject();


    void gotQuestionAnswer(bool answer);


    //MODEL Functionality
    void displayMessage(QString title, QString message, int ID);

    void clearHistory();

    void modelLabelChanged();

private:
    //Helper functions.
    bool _paste(int ID, QString xmlData, bool addAction = true);
    bool _cut(QList<int> IDs, bool addAction = true);
    bool _clear();
    bool _copy(QList<int> IDs);
    bool _remove(QList<int> IDs, bool addAction = true);
    bool _remove(int ID, bool addAction = true);
    bool _replicate(QList<int> IDs, bool addAction = true);
    bool _importProjects(QStringList xmlDataList, bool addAction = true);
    bool _importSnippet(QList<int> IDs, QString fileName, QString fileData, bool addAction = true);
    bool _exportSnippet(QList<int> IDs);
    bool _exportProject();


private:
    void attachData(Entity* parent, Data* data, bool addAction = true);
    bool destructData(Entity* parent, QString keyName, bool addAction = true);


private:
    void setViewSignalsEnabled(bool enabled, bool sendQueuedSignals = true);
    void updateUndoRedoState();
    void setData(Entity* parent, QString keyName, QVariant dataValue, bool addAction = true);
    void clearUndoHistory();

    bool askQuestion(MESSAGE_TYPE type, QString questionTitle, QString question, int ID=-1);
    Node* getSingleNode(QList<int> IDs);

    bool _importGraphMLXML(QString document, Node* parent = 0, bool linkID=false, bool resetPos=false);



    void projectModified();
    void projectSaved();

    Node* getSharedParent(QList<int> IDs);



    //Exports a Selection of Containers to export into GraphML
    QString _exportGraphMLDocument(QList<int> nodeIDs, bool allEdges = false, bool GUI_USED=false);
    QString _exportGraphMLDocument(Node* node, bool allEdges = false, bool GUI_USED=false);

    //Finds or Constructs a GraphMLKey given a Name, Type and ForType
    Key* constructKey(QString name, QVariant::Type type,  Entity::ENTITY_KIND entityKind);
    bool destructKey(QString name);
    Key* getKeyFromName(QString name);
    Key* getKeyFromID(int ID);

    //Finds or Constructs a Node Instance or Implementation inside parent of Definition.
    int constructDefinitionRelative(Node* parent, Node* definition, bool instance = true);

    //Gets a specific Attribute from the current Element in the XML.
    //Returns "" if no Attribute found.
    QString getXMLAttribute(QXmlStreamReader& xml, QString attributeID);

    Edge* _constructEdge(Node* source, Node* destination);
    Edge* constructEdgeWithData(Node* source, Node* destination, QList<Data*> data = QList<Data*>(), int previousID=-1);
    Edge* constructEdgeWithStrData(Node* source, Node* destination, QList<QStringList> data = QList<QStringList>(), int previousID=-1);

    //Stores/Gets/Removes items/IDs from the GraphML Hash
    void storeGraphMLInHash(Entity*item);
    Entity*getGraphMLFromHash(int ID);
    void removeGraphMLFromHash(int ID);

    //Constructs a Node using the attached Data elements. Attachs the node to the parentNode provided.
    Node* constructChildNode(Node* parentNode, QList<Data*> dataToAttach);

    Node* constructNode(QList<Data*> data);




    QList<Data*> cloneNodesData(Node* original, bool ignoreVisuals=true);
    Node* cloneNode(Node* original, Node* parent, bool ignoreVisuals=true);
    //Sets up an Undo state for the creation of the Node/Edge, and tells the View To construct a GUI Element.
    void constructNodeGUI(Node* node);
    void constructEdgeGUI(Edge* edge);

    bool destructEntity(int ID);
    bool destructNode(Node* node);
    bool destructEdge(Edge* edge);


    //Constructs a Vector of basic Data entities required for creating a Node.
    QList<Data*> constructDataVector(QString nodeKind, QPointF relativePosition = QPointF(-1,-1));
    QList<Data*> constructPositionDataVector(QPointF point);
    QString getNodeInstanceKind(Node* definition);
    QString getNodeImplKind(Node* definition);

    //Constructs and setups all required Entities inside the Model Node.
    void setupModel();

    //Binds matching Data elements from the Node Child, to the Node Definition.
    void bindData(Node* definition, Node* instance);
    void unbindData(Node* definition, Node* instance);

    //Setup/Teardown the node provided an Instance of the Definition. It will adopt Instances of all Definitions contained by definition and bind all Data which isn't protected.
    bool setupDefinitionRelationship(Node* definition, Node* node, bool instance);
    bool teardownDefinitionRelationship(Node* definition, Node* node, bool instance);



    //Attaches an Aggregate Definition to an EventPort Definition.
    bool setupEventPortAggregateRelationship(EventPort* eventPort, Aggregate* aggregate);
    bool teardownEventPortAggregateRelationship(EventPort* EventPort, Aggregate* aggregate);


    bool setupAggregateRelationship(Node* node, Aggregate* aggregate);
    bool teardownAggregateRelationship(Node* node, Aggregate* aggregate);

    bool setupDataEdgeRelationship(BehaviourNode* outputNode, BehaviourNode* inputNode, bool setup = true);
    bool teardownDataEdgeRelationship(BehaviourNode* outputNode, BehaviourNode* inputNode);

    bool setupParameterRelationship(Parameter* parameter, Node* data);
    bool teardownParameterRelationship(Parameter* parameter, Node* data);

    //Checks to see if the provided GraphML document is Valid XML.
    bool isGraphMLValid(QString inputGraphML);


    void setupManagementComponents();
    void setupLocalNode();

    void enforceUniqueLabel(Node* node, QString newLabel = "");
    bool requiresUniqueLabel(Node* node);
    //int getUniqueSortOrder(Node* node, )
    //void enforceUniqueLabel(Node* node, QString newLabel = "");
    void enforceUniqueSortOrder(Node* node, int newSortPos = -1);


    //Returns true if the Model Entities can be connected.
    bool isEdgeLegal(Node* src, Node* dst);

    //Returns true if a nodeKind has been Implemented in the Model.
    bool isNodeKindImplemented(QString nodeKind);

    //Used by Undo/Redo to reverse an ActionItem from the Stacks.
    bool reverseAction(EventAction action);

    //Adds an ActionItem to the Undo/Redo Stack depending on the State of the application.
    void addActionToStack(EventAction action, bool addAction=true);

    //Undo's/Redo's all of the ActionItems in the Stack which have been performed since the last operation.
    void undoRedo(bool undo=true);

    void logAction(EventAction item);

    bool canDeleteNode(Node* node);


    QPair<bool, QString> readFile(QString filePath);
    Node* constructTypedNode(QString nodeKind, bool isTemporary = false, QString nodeType="", QString nodeLabel="");
    Edge* constructTypedEdge(Node* src, Node* dst, Edge::EDGE_CLASS edgeClass);

    //Attach Data('s) to the GraphML item.
    bool _attachData(Entity* item, Data* data, bool addAction = true);
    bool _attachData(Entity* item, QList<QStringList> dataList, bool addAction = true);
    bool _attachData(Entity* item, QList<Data*> dataList, bool addAction = true);
    bool _attachData(Entity *item, QString keyName, QVariant value, bool addAction = true);
    


    Process* getWorkerProcess(QString workerName, QString operationName);

    //Gets the GraphML/Node/Edge Item from the ID provided. Checks the Hash.
    Entity*getGraphMLFromID(int ID);
    Node* getNodeFromID(int ID);
    Node* getFirstNodeFromList(QList<int> ID);
    Edge* getEdgeFromID(int ID);

    //Used to find old ID's which may have been deleted from the Model. Will find the replacement ID if they exist.
    int getIDFromOldID(int ID);

    int getIntFromQString(QString ID);
    //Links an ID of an already deleted GraphML Item to the ID of the new GraphML Item.
    void linkOldIDToID(int oldID, int newID);

    //Casts the GraphML as a Node/Edge, will be NULL if not a Node/Edge
    Node* getNodeFromGraphML(Entity* item);
    Edge* getEdgeFromGraphML(Entity* item);

    bool _isInModel(Entity* item);
    bool _isInWorkerDefinitions(Entity* item);

    QString getSysOS();
    QString getSysArch();
    QString getSysOSVersion();

    //Stores the GraphMLKey's used by the Model.
    QList<Key*> keys;

    //Stores the list of nodeID's and EdgeID's inside the Hash.
    QList<int> nodeIDs;
    QList<int> edgeIDs;

    //Stack of ActionItems in the Undo/Redo Stack.
    QStack<EventAction> undoActionStack;
    QStack<EventAction> redoActionStack;

    QString getTimeStamp();
    QString getDataValueFromKeyName(QList<Data*> dataList, QString keyName);
    void setDataValueFromKeyName(QList<Data*> dataList, QString keyName, QString value);

    //Provides a lookup for IDs.
    QHash<int, int> IDLookupHash;
    QHash<int, Entity*> IDLookupGraphMLHash;
    QHash<int, EntityAdapter*> ID2AdapterHash;
    QHash<int, QString> reverseKindLookup;

    QHash<QString, QList<int> > kindLookup;

    QHash<int, int> readOnlyLookup;
    QHash<int, int> reverseReadOnlyLookup;

    QHash<QString, int> treeLookup;
    QHash<int, QString> reverseTreeLookup;


    QString getProcessName(Process* process);

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
    QStringList visualKeyNames;

    //Used to tell if we are currently Undo-ing/Redo-ing in the system.
    bool UNDOING;
    bool REDOING;
    bool INITIALIZING;
    bool USE_LOGGING;
    bool IMPORTING_SNIPPET;

    Model* model;
    Node* workerDefinitions;

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
    QHash<QString, Process*> workerProcesses;

    int previousUndos;

    QString externalWorkerDefPath;

    QList<int> connectedLinkedIDs;

    bool CUT_USED;
    int actionCount;
    QString currentAction;
    QFile* logFile;
    int currentActionID;
    int currentActionItemID;

    QList<ViewSignal> viewSignalsList;
    bool viewSignalsEnabled;
    bool questionAnswer;

    bool DESTRUCTING_CONTROLLER;



    bool projectDirty;

};
 QDataStream &operator<<(QDataStream &out, const EventAction &action);

#endif // NEWCONTROLLER_H
