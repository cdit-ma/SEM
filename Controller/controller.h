#ifndef NEWCONTROLLER_H
#define NEWCONTROLLER_H


#include <QStack>
#include <QFile>
#include <QPointF>
#include <QXmlStreamReader>
#include <QNetworkInterface>
#include <QReadWriteLock>

#include "doublehash.h"
#include "viewcontroller.h"

#include "../Model/model.h"
#include "../Model/edge.h"
#include "../Model/node.h"
#include "../Model/key.h"
#include "../Model/data.h"
#include "../Model/workerdefinitions.h"

#include "../Model/InterfaceDefinitions/eventport.h"
#include "../Model/InterfaceDefinitions/aggregate.h"
#include "../Model/BehaviourDefinitions/parameter.h"
#include "../Model/BehaviourDefinitions/process.h"
#include "../Model/DeploymentDefinitions/managementcomponent.h"


#define DANCE_EXECUTION_MANAGER "DANCE_EXECUTION_MANAGER"
#define DANCE_PLAN_LAUNCHER "DANCE_PLAN_LAUNCHER"
#define QPID_BROKER "QPID_BROKER"
#define DDS_LOGGING_SERVER "DDS_LOGGING_SERVER"

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

struct ReadOnlyState{
    long long snippetMAC;
    long long snippetTime;
    long long exportTime;
    bool isDefinition;
    int snippetID;

    bool imported;

    bool operator==(const ReadOnlyState &other) const{
        return (snippetID == other.snippetID) && (snippetTime == other.snippetTime) && (snippetMAC == other.snippetMAC) && (isDefinition == other.isDefinition);
    }
    bool isValid(){
        return (snippetMAC > 0) && (snippetTime > 0) && (snippetID > 0) && (!isDefinition);
    }
    bool isOlder(const ReadOnlyState &other) const{
        if(this->operator ==(other)){
            if(exportTime < other.exportTime){
                return true;
            }
        }
        return false;
    }
    bool isSimilar(const ReadOnlyState &other) const{
        if((snippetMAC == other.snippetMAC) && (snippetTime == other.snippetTime)){
            return true;
        }
        return false;
    }
};


inline uint qHash(const ReadOnlyState& key)
{
    uint hash = (uint(key.snippetMAC ^ (key.snippetMAC >> 32))) ^ key.snippetTime ^ key.snippetID;
    return hash;
}






struct EventAction{
    struct _Action{
        int ID;
        int actionID;
        ACTION_TYPE type;
        GraphML::GRAPHML_KIND kind;
        QString name;
        uint timestamp;
    } Action;

    int ID;
    int parentID;

    bool projectDirty;

    struct _Entity{
        QString XML;
        QString nodeKind;
        Entity::ENTITY_KIND kind;
        Edge::EDGE_KIND edgeClass;
    } Entity;

    struct _Key{
        QString name;
        QVariant::Type type;
    } Key;

    struct _Data{
        QString keyName;
        QVariant oldValue;
        QVariant newValue;
        bool isProtected;
    } Data;
};

class NewController: public QObject
{
    Q_OBJECT
public:
    NewController();
    ~NewController();

    void setExternalWorkerDefinitionPath(QString path);


    void connectViewController(ViewController* view);
    void disconnectViewController(ViewController *view);

    QString getProjectPath() const;
    void setProjectPath(QString path);

    bool isProjectSaved() const;
    void setProjectSaved(QString path="");



    //Public Read/Write Locked Functions

    //READ
    QString getProjectAsGraphML();
    QString getSelectionAsGraphMLSnippet(QList<int> IDs);

    QList<Edge::EDGE_KIND> getValidEdgeKindsForSelection(QList<int> IDs);
    QList<Edge::EDGE_KIND> getExistingEdgeKindsForSelection(QList<int> IDs);

    QStringList getAdoptableNodeKinds(int ID);
    QStringList getValidKeyValues(int ID, QString keyName);
    QList<int> getConnectableNodeIDs(QList<int> srcs, Edge::EDGE_KIND edgeKind);
    QList<int> getConstructableConnectableNodes(int parentID, QString instanceNodeKind, Edge::EDGE_KIND edgeClass);

    QList<int> getOrderedSelectionIDs(QList<int> selection);

    QList<int> getWorkerFunctions();


private:
    void loadWorkerDefinitions();
    //Gets the Model Node.
    Model* getModel();
    WorkerDefinitions* getWorkerDefinitions();
    QList<Node*> _getConnectableNodes(QList<Node*> sourceNodes, Edge::EDGE_KIND edgeKind);
    QList<Entity *> getOrderedSelection(QList<int> selection);
public:

    bool canCopy(QList<int> selection);
    bool canGetCPP(QList<int> selection);
    bool canReplicate(QList<int> selection);
    bool canCut(QList<int> selection);


    bool canReplicate(QList<Entity*> selection);
    bool canCut(QList<Entity*> selection);
    bool canCopy(QList<Entity*> selection);
    bool canPaste(QList<Entity*> selection);
    bool canDelete(QList<Entity *> selection);


    bool canDelete(QList<int> selection);
    bool canPaste(QList<int> selection);
    bool canExportSnippet(QList<int> selection);
    bool canImportSnippet(QList<int> selection);
    bool canSetReadOnly(QList<int> selection);
    bool canUnsetReadOnly(QList<int> selection);
    bool canUndo();
    bool canRedo();
    bool canLocalDeploy();


    int getDefinition(int ID);
    int getImplementation(int ID);
    QList<int> getInstances(int ID);

    int getAggregate(int ID);
    int getDeployedHardwareID(int ID);




signals:
    void projectModified(bool modified);
    void initiateTeardown();
    void controller_dead();
    void entityConstructed(int ID, ENTITY_KIND eKind, QString kind, QHash<QString, QVariant> data, QHash<QString, QVariant> properties);
    void entityDestructed(int ID, ENTITY_KIND eKind, QString kind);


    void dataChanged(int ID, QString keyName, QVariant data);
    void dataRemoved(int ID, QString keyName);
    void propertyChanged(int ID, QString propertyName, QVariant data);
    void propertyRemoved(int ID, QString propertyName);


    void controller_ProjectFileChanged(QString);
    void controller_ProjectNameChanged(QString);

    void undoRedoChanged();
    void controller_CanUndo(bool ok);
    void controller_CanRedo(bool ok);
    void controller_IsModelReady(bool ready);
    void controller_ModelReady();
    void progressChanged(int);
    void showProgress(bool, QString = "");
    void controller_ActionProgressChanged(int percent, QString action="");
    void controller_ActionFinished(bool actionSucceeded = true, QString errorCode = "");
    void controller_OpenFinished(bool success);

    void controller_AskQuestion(QString title, QString message, int ID=-1);
    void controller_GotQuestionAnswer();

    void controller_DisplayMessage(MESSAGE_TYPE, QString messageString, QString messageTitle = "", QString messageIcon = "", int centerID =-1);

    void controller_SavedProject(QString filePath, QString dat);

    void controller_ExportedProject(QString);

    void controller_ExportedSnippet(QString snippetXMLData);

    void controller_GraphMLConstructed(Entity*);

    void controller_GraphMLDestructed(int ID, GraphML::GRAPHML_KIND kind);
    void test_destruct(int ID);




    void controller_ProjectRequiresSave(bool requiresSave);

    void controller_SetClipboardBuffer(QString);

    void controller_SetViewEnabled(bool);
public slots:
    void setupController();
    void setData(int parentID, QString keyName, QVariant dataValue);
    void removeData(int parentID, QString keyName);
private slots:

    void enableDebugLogging(bool logMode, QString applicationPath="");

    //Clipboard functionality
    void cut(QList<int> IDs);
    void copy(QList<int> IDs);
    void paste(QList<int> IDs, QString xmlData);
    void replicate(QList<int> IDs);
    void remove(QList<int> IDs);
    void undo();
    void redo();

    void triggerAction(QString actionName);

    void openProject(QString filepath, QString xmlData);
    void importProjects(QStringList xmlDataList);


    void setReadOnly(QList<int> IDs, bool readOnly);
    void importSnippet(QList<int> IDs, QString fileName, QString fileData);
    void exportSnippet(QList<int> IDs);


    //Toolbar/Dock Functionality
    void clear();


    void constructConnectedNode(int parentID, QString nodeKind, int dstID, Edge::EDGE_KIND edgeKind = Edge::EC_UNDEFINED, QPointF pos=QPointF());

    void constructNode(int parentID, QString kind, QPointF centerPoint);

    void constructEdge(QList<int> srcIDs, int dstID,Edge::EDGE_KIND edgeClass = Edge::EC_UNDEFINED);
    void destructEdges(QList<int> srcIDs, int dstID,Edge::EDGE_KIND edgeClass = Edge::EC_UNDEFINED);

    void constructDDSQOSProfile(int parentID, QPointF position);
    void constructWorkerProcess(int parentID, int workerProcessID, QPointF pos);




    void constructDestructMultipleEdges(QList<int> srcIDs, int dstID);
    void constructDestructEdges(QList<int> destruct_srcIDs, QList<int> destruct_dstIDs, QList<int> construct_srcIDs, int dstID);





    //MODEL Functionality
    void displayMessage(QString title, QString message, int ID);
    void gotQuestionAnswer(bool answer);


    void _projectNameChanged();

private:
    Edge::EDGE_KIND getValidEdgeClass(Node* src, Node* dst);
    QList<Edge::EDGE_KIND> getPotentialEdgeClasses(Node* src, Node* dst);
    void clearHistory();

    QString _copy(QList<Entity*> selection);

    //Helper functions.
    bool _paste(int ID, QString xmlData, bool addAction = true);
    bool _cut(QList<int> IDs, bool addAction = true);
    bool _clear();
    bool _copy(QList<int> IDs);
    bool _remove(QList<int> IDs, bool addAction = true);
    bool _remove(QList<Entity*> items);
    bool _remove(int ID, bool addAction = true);
    bool _replicate(QList<Entity*> items);
    bool _replicate(QList<int> IDs, bool addAction = true);
    bool _importProjects(QStringList xmlDataList, bool addAction = true);
    bool _importSnippet(QList<int> IDs, QString fileName, QString fileData, bool addAction = true);
    QString _exportSnippet(QList<int> IDs);

    long long getMACAddress();

    void attachData(Entity* parent, Data* data, bool addAction = true);
    bool destructData(Entity* parent, QString keyName, bool addAction = true);

    void setViewSignalsEnabled(bool enabled, bool sendQueuedSignals = true);
    void updateUndoRedoState();
    void _setData(Entity* parent, QString keyName, QVariant dataValue, bool addAction = true);
    void clearUndoHistory();

    bool askQuestion(QString questionTitle, QString question, int ID=-1);
    Node* getSingleNode(QList<int> IDs);


    bool _newImportGraphML(QString document, Node* parent = 0);


    ReadOnlyState getReadOnlyState(Node* node);

    EventAction getEventAction();

    void setProjectDirty(bool dirty);

    Node* getSharedParent(QList<int> IDs);



    //Exports a Selection of Containers to export into GraphML
    QString _exportGraphMLDocument(QList<int> nodeIDs, bool allEdges = false, bool GUI_USED=false, bool ignoreVisuals=false);
    QString _exportGraphMLDocument(Node* node, bool allEdges = false, bool GUI_USED=false);

    //Finds or Constructs a GraphMLKey given a Name, Type and ForType
    Key* constructKey(QString name, QVariant::Type type);
    bool destructKey(QString name);
    Key* getKeyFromName(QString name);
    Key* getKeyFromID(int ID);

    //Finds or Constructs a Node Instance or Implementation inside parent of Definition.
    int constructDependantRelative(Node* parent, Node* definition);

    //Gets a specific Attribute from the current Element in the XML.
    //Returns "" if no Attribute found.
    QString getXMLAttribute(QXmlStreamReader& xml, QString attributeID);

    Edge* _constructEdge(Edge::EDGE_KIND edgeClass, Node* src, Node* dst);
    Edge* constructEdgeWithData(Edge::EDGE_KIND edgeClass, Node* source, Node* destination, QList<Data*> data = QList<Data*>(), int previousID=-1);

    //Stores/Gets/Removes items/IDs from the GraphML Hash
    void storeGraphMLInHash(Entity*item);
    Entity*getGraphMLFromHash(int ID);
    void removeGraphMLFromHash(int ID);

    //Constructs a Node using the attached Data elements. Attachs the node to the parentNode provided.
    Node* constructChildNode(Node* parentNode, QList<Data*> dataToAttach);
    bool attachChildNode(Node* parentNode, Node* childNode, bool sendGUIRequest = true);

    Node* _constructNode(QList<Data*> data);

    bool updateProgressNotification();
    QList<int> getIDs(QList<Entity*> items);



    QList<Data*> cloneNodesData(Node* original, bool ignoreVisuals=true);
    Node* cloneNode(Node* original, Node* parent, bool ignoreVisuals=true);
    //Sets up an Undo state for the creation of the Node/Edge, and tells the View To construct a GUI Element.
    void constructNodeGUI(Node* node);
    void constructEdgeGUI(Edge* edge);

    bool destructEntity(int ID);
    bool destructEntity(Entity* item);
    bool destructNode(Node* node);
    bool destructEdge(Edge* edge);


    //Constructs a Vector of basic Data entities required for creating a Node.
    QList<Data*> constructDataVector(QString nodeKind, QPointF relativePosition = QPointF(-1,-1), QString nodeType="", QString nodeLabel="");
    QList<Data*> constructRequiredEdgeData(Edge::EDGE_KIND edgeClass);
    QList<Data*> constructPositionDataVector(QPointF point=QPointF(-1, -1));
    QString getNodeInstanceKind(Node* definition);
    QString getNodeImplKind(Node* definition);

    //Constructs and setups all required Entities inside the Model Node.
    void setupModel();

    //Binds matching Data elements from the Node Child, to the Node Definition.
    void bindData(Node* definition, Node* instance);
    void unbindData(Node* definition, Node* instance);

    //Setup/Teardown the node provided an Instance of the Definition. It will adopt Instances of all Definitions contained by definition and bind all Data which isn't protected.
    bool setupDependantRelationship(Node* definition, Node* node);
    bool teardownDependantRelationship(Node* definition, Node* node);




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
    void enforceUniqueSortOrder(Node* node, int newPosition = -1);



    //Returns true if a nodeKind has been Implemented in the Model.
    bool isNodeKindImplemented(QString nodeKind);

    //Used by Undo/Redo to reverse an ActionItem from the Stacks.
    bool reverseAction(EventAction action);

    //Adds an ActionItem to the Undo/Redo Stack depending on the State of the application.
    void addActionToStack(EventAction action, bool addAction=true);

    //Undo's/Redo's all of the ActionItems in the Stack which have been performed since the last operation.
    void undoRedo(bool undo, bool updateProgess = false);

    void logAction(EventAction item);

    bool canDeleteNode(Node* node);


    QPair<bool, QString> readFile(QString filePath);
    Node* constructTypedNode(QString nodeKind, bool isTemporary = false, QString nodeType="", QString nodeLabel="");
    Edge* constructTypedEdge(Node* src, Node* dst, Edge::EDGE_KIND edgeClass);

    //Attach Data('s) to the GraphML item.
    bool _attachData(Entity* item, Data* data, bool addAction = true);
    //bool _attachData(Entity* item, QList<QStringList> dataList, bool addAction = true);
    bool _attachData(Entity* item, QList<Data*> dataList, bool addAction = true);
    bool _attachData(Entity *item, QString keyName, QVariant value, bool addAction = true);

    



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
    QHash<QString, Key*> keys;

    //Stores the list of nodeID's and EdgeID's inside the Hash.
    QList<int> nodeIDs;
    QList<int> edgeIDs;

    //Stack of ActionItems in the Undo/Redo Stack.
    QStack<EventAction> undoActionStack;
    QStack<EventAction> redoActionStack;

    QString getTimeStamp();
    uint getTimeStampEpoch();

    QString getDataValueFromKeyName(QList<Data*> dataList, QString keyName);
    void setDataValueFromKeyName(QList<Data*> dataList, QString keyName, QString value);

    //Provides a lookup for IDs.
    QHash<int, int> IDLookupHash;
    QHash<int, Entity*> IDLookupGraphMLHash;
    //QHash<int, EntityAdapter*> ID2AdapterHash;



    QHash<int, QString> reverseKindLookup;

    QHash<QString, QList<int> > kindLookup;

    QHash<int, int> readOnlyLookup;
    QHash<int, int> reverseReadOnlyLookup;

    DoubleHash<ReadOnlyState, int> readOnlyHash;

    //QHash<ReadOnlyState, int> readOnlyStateLookup;
    //QHash<ReadOnlyState, int> readOnlyStateLookup;

    DoubleHash<QString, int> treeHash;

    //QHash<QString, int> treeLookup;
    //QHash<int, QString> reverseTreeLookup;


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
    QStringList dds_QosNodeKinds;

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

    QList<Node*> getAllNodes();
    QList<Node*> getNodes(QList<int> IDs);


    QHash<QString, ManagementComponent*> managementComponents;

    QHash<QString, Node*> hardwareEntities;
    QHash<QString, Process*> workerProcesses;

    int previousUndos;

    QString externalWorkerDefPath;

    //QList<int> connectedLinkedIDs;

    bool isUserAction();

    bool CUT_USED;
    bool REPLICATE_USED;
    bool OPENING_PROJECT;
    bool IMPORTING_PROJECT;
    bool PASTE_USED;
    bool IMPORTING_WORKERDEFINITIONS;
    bool CONSTRUCTING_WORKERFUNCTION;

    int actionCount;
    QString currentAction;
    QFile* logFile;
    int currentActionID;
    int currentActionItemID;

    QList<ViewSignal> viewSignalsList;
    bool viewSignalsEnabled;
    bool questionAnswer;

    bool DESTRUCTING_CONTROLLER;


    QString projectPath;
    bool projectFilePathSet;

    bool projectDirty;

    QThread* controllerThread;
    QReadWriteLock lock;

};
 QDataStream &operator<<(QDataStream &out, const EventAction &action);

#endif // NEWCONTROLLER_H
