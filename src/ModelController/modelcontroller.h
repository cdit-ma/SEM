#ifndef NEWCONTROLLER_H
#define NEWCONTROLLER_H

#include <QObject>

#include <QStack>
#include <QPointF>
#include <QXmlStreamReader>
#include <QReadWriteLock>
#include <QVariant>

#include "kinds.h"
#include "nodekinds.h"
#include "edgekinds.h"




class Entity;
class Node;
class Edge;

class Data;
class EventPort;
class Aggregate;
class WorkerProcess;
class EntityFactory;
class DataNode;

#include "../Utils/doublehash.h"


#define DANCE_EXECUTION_MANAGER "DANCE_EXECUTION_MANAGER"
#define DANCE_PLAN_LAUNCHER "DANCE_PLAN_LAUNCHER"
#define QPID_BROKER "QPID_BROKER"
#define DDS_LOGGING_SERVER "DDS_LOGGING_SERVER"

enum ACTION_TYPE {CONSTRUCTED, DESTRUCTED, MODIFIED};

struct EdgeTemp
{
    QString id;
    qint64 lineNumber;
    QString source;
    QString target;
    QList<Data *> data;
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
        GRAPHML_KIND kind;
        QString name;
        uint timestamp;
    } Action;

    int ID;
    int parentID;

    bool projectDirty;

    struct _Entity{
        QString XML;
        QString nodeKind;
        GRAPHML_KIND kind;
        EDGE_KIND edgeClass;
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

class ModelController: public QObject
{
    Q_OBJECT
public:
    static QStringList getVisualKeys();

    ModelController();
    ~ModelController();


    QString getProjectPath() const;
    void setProjectPath(QString path);

    bool isProjectSaved() const;
    void setProjectSaved(QString path="");



    void setReadOnly(QList<int> IDs, bool readOnly);
    //Public Read/Write Locked Functions

    //READ
    QString getProjectAsGraphML();
    QString getSelectionAsGraphMLSnippet(QList<int> IDs);

    QList<EDGE_KIND> getValidEdgeKindsForSelection(QList<int> IDs);
    QList<EDGE_KIND> getExistingEdgeKindsForSelection(QList<int> IDs);

    QList<NODE_KIND> getAdoptableNodeKinds2(int ID);
    
    QList<QVariant> getValidKeyValues(int ID, QString keyName);
    QList<int> getConnectableNodeIDs(QList<int> srcs, EDGE_KIND edgeKind);

    QList<int> getOrderedSelectionIDs(QList<int> selection);

    QList<int> getWorkerFunctions();


    QList<int> getConstructableConnectableNodes(int parentID, NODE_KIND nodeKind, EDGE_KIND edgeClass);
    //NEW FUNCTIONS FRESHLY IMPLEMENTED WITH QREADLOCKER
    bool isNodeOfType(int ID, NODE_TYPE type);
    int getNodeParentID(int ID);
    VIEW_ASPECT getNodeViewAspect(int ID);
    int getSharedParent(int ID, int ID2);
    bool isNodeAncestor(int ID, int ID2);




    //UNSURE IF NEEDED
    QStringList getProtectedEntityKeys(int ID);
    QStringList getEntityKeys(int ID);
    QVariant getEntityDataValue(int ID, QString key_name);
    
private:
    void loadWorkerDefinitions();
    //Gets the Model Node.
    QList<Node*> _getConnectableNodes(QList<Node*> sourceNodes, EDGE_KIND edgeKind);
    QList<Entity *> getOrderedSelection(QList<int> selection);
public:
    bool canUndo();
    bool canRedo();
    bool canLocalDeploy();


    bool canCut(QList<int> selection);
    bool canCopy(QList<int> selection);
    bool canReplicate(QList<int> selection);
    bool canRemove(QList<int> selection);
    bool canPaste(QList<int> selection);

    bool canSetReadOnly(QList<int> selection);
    bool canUnsetReadOnly(QList<int> selection);

    bool canExportSnippet(QList<int> selection);
    bool canImportSnippet(QList<int> selection);

    int getDefinition(int ID);
    int getImplementation(int ID);
    QList<int> getInstances(int ID);

    int getAggregate(int ID);
    int getDeployedHardwareID(int ID);


private:
    bool canReplicate(QList<Entity*> selection);
    bool canCut(QList<Entity*> selection);
    bool canCopy(QList<Entity*> selection);
    bool canPaste(QList<Entity*> selection);
    bool canRemove(QList<Entity *> selection);
    bool canSetReadOnly(QList<Entity* > selection);
    bool canUnsetReadOnly(QList<Entity* > selection);

    bool canExportSnippet(QList<Entity*> selection);
    bool canImportSnippet(QList<Entity*> selection);









signals:
    //New SIGNAL
    void NodeConstructed(int parent_id, int id, NODE_KIND kind);
    void EdgeConstructed(int id, EDGE_KIND kind, int src_id, int dst_id);
    
    void projectModified(bool modified);
    void initiateTeardown();
    void controller_dead();

    void entityConstructed(int ID, GRAPHML_KIND eKind, QString kind, QHash<QString, QVariant> data, QHash<QString, QVariant> properties);
    void entityDestructed(int ID, GRAPHML_KIND eKind, QString kind);


    void dataChanged(int ID, QString keyName, QVariant data);
    void dataRemoved(int ID, QString keyName);
    void propertyChanged(int ID, QString propertyName, QVariant data);
    void propertyRemoved(int ID, QString propertyName);


    void controller_ProjectFileChanged(QString);
    void controller_ProjectNameChanged(QString);

    void undoRedoChanged();
    void controller_IsModelReady(bool ready);
    void progressChanged(int);
    void showProgress(bool, QString = "");
    void controller_ActionProgressChanged(int percent, QString action="");
    void controller_ActionFinished(bool actionSucceeded = true, QString errorCode = "");
    void controller_OpenFinished(bool success);

    void controller_AskQuestion(QString title, QString message, int ID=-1);
    void controller_GotQuestionAnswer();

    void controller_SavedProject(QString filePath, QString dat);

    void controller_ExportedProject(QString);

    void controller_ExportedSnippet(QString snippetXMLData);

    void controller_GraphMLConstructed(Entity*);

    void controller_GraphMLDestructed(int ID, GRAPHML_KIND kind);
    void test_destruct(int ID);




    void controller_ProjectRequiresSave(bool requiresSave);

    void controller_SetClipboardBuffer(QString);

    void controller_SetViewEnabled(bool);
public slots:
    void setupController();
    void setData(int parentID, QString keyName, QVariant dataValue);
    void removeData(int parentID, QString keyName);

private:
    Node* construct_temp_node(Node* parent_node, NODE_KIND kind);

    Node* construct_node(Node* parent_node, NODE_KIND kind);
    Node* construct_child_node(Node* parent_node, NODE_KIND kind, bool notify_view = true);
    Node* construct_connected_node(Node* parent_node, NODE_KIND node_kind, Node* dst, EDGE_KIND edge_kind);
    Edge* construct_edge(EDGE_KIND edge_kind, Node* source, Node* destination, bool notify_view = true);
    int GetEdgeOrderIndex(EDGE_KIND kind);
public slots:
    void constructConnectedNode(int parentID, NODE_KIND nodeKind, int dstID, EDGE_KIND edgeKind, QPointF pos=QPointF());
    void constructNode(int parentID, NODE_KIND kind, QPointF centerPoint);
    void constructWorkerProcess(int parent_id, int dst_id, QPointF centerPoint);


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


    void importSnippet(QList<int> IDs, QString fileName, QString fileData);
    void exportSnippet(QList<int> IDs);


    //Toolbar/Dock Functionality
    void clear();



    void constructEdge(QList<int> srcIDs, int dstID, EDGE_KIND edgeClass);
    void destructEdges(QList<int> srcIDs, int dstID, EDGE_KIND edgeClass);

    




    //MODEL Functionality
    void displayMessage(QString title, QString message, int ID);
    void gotQuestionAnswer(bool answer);


    void _projectNameChanged();

private:
    Node* construct_setter_node(Node* parent);
    Node* construct_dds_profile_node(Node* parent);
    Node* construct_for_condition_node(Node* parent);



    EDGE_KIND getValidEdgeClass(Node* src, Node* dst);
    QList<EDGE_KIND> getPotentialEdgeClasses(Node* src, Node* dst);
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
    QString _exportGraphMLDocument(QList<int> entityIDs, bool allEdges = false, bool GUI_USED=false, bool ignoreVisuals=false);
    QString _exportGraphMLDocument(Node* node, bool allEdges = false, bool GUI_USED=false);

    //Finds or Constructs a Node Instance or Implementation inside parent of Definition.
    int constructDependantRelative(Node* parent, Node* definition);

    //Gets a specific Attribute from the current Element in the XML.
    //Returns "" if no Attribute found.
    QString getXMLAttribute(QXmlStreamReader& xml, QString attributeID);

   
    

    //Stores/Gets/Removes items/IDs from the GraphML Hash
    
    Entity* getEntity(int ID);
    Node* getNode(int ID);
    Edge* getEdge(int ID);   
    
    void storeEntity(Entity* item);
    void removeEntity(int ID);


    bool attachChildNode(Node* parentNode, Node* childNode, bool notify_view = true);

    bool updateProgressNotification();
    QList<int> getIDs(QList<Entity*> items);
    QList<Entity*> getEntities(QList<int> IDs);


    Node* cloneNode(Node* original, Node* parent);
    //Sets up an Undo state for the creation of the Node/Edge, and tells the View To construct a GUI Element.
    void constructNodeGUI(Node* node);
    void constructEdgeGUI(Edge* edge);

    bool destructEntity(int ID);
    bool destructEntity(Entity* item);
    bool destructNode(Node* node);
    bool destructEdge(Edge* edge);


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

    bool setupDataEdgeRelationship(DataNode *outputNode, DataNode *inputNode, bool setup = true);

    //Checks to see if the provided GraphML document is Valid XML.
    bool isGraphMLValid(QString inputGraphML);



    //Used by Undo/Redo to reverse an ActionItem from the Stacks.4
    bool reverseAction(EventAction action);

    //Adds an ActionItem to the Undo/Redo Stack depending on the State of the application.
    void addActionToStack(EventAction action, bool addAction=true);

    //Undo's/Redo's all of the ActionItems in the Stack which have been performed since the last operation.
    void undoRedo(bool undo, bool updateProgess = false);


    bool canDeleteNode(Node* node);


    QPair<bool, QString> readFile(QString filePath);
    
    //Attach Data('s) to the GraphML item.
    bool _attachData(Entity* item, Data* data, bool addAction = true);
    //bool _attachData(Entity* item, QList<QStringList> dataList, bool addAction = true);
    bool _attachData(Entity* item, QList<Data*> dataList, bool addAction = true);
    bool _attachData(Entity *item, QString keyName, QVariant value, bool addAction = true);

    



    //Gets the GraphML/Node/Edge Item from the ID provided. Checks the Hash.
    Node* getFirstNodeFromList(QList<int> ID);
    

    //Used to find old ID's which may have been deleted from the Model. Will find the replacement ID if they exist.
    int get_linked_ids(int ID);
    void link_ids(int oldID, int newID);


    

    //Stack of ActionItems in the Undo/Redo Stack.
    QStack<EventAction> undoActionStack;
    QStack<EventAction> redoActionStack;

    QString getTimeStamp();
    uint getTimeStampEpoch();

    QString getDataValueFromKeyName(QList<Data*> dataList, QString keyName);
    void setDataValueFromKeyName(QList<Data*> dataList, QString keyName, QString value);

    //Provides a lookup for IDs.
    QHash<int, int> id_hash_;
    QHash<int, Entity*> entity_hash_;

    //Stores the list of nodeID's and EdgeID's inside the Hash.
    QList<int> node_ids_;
    QList<int> edge_ids_;


    QHash<int, int> readOnlyLookup;
    QHash<int, int> reverseReadOnlyLookup;

    DoubleHash<ReadOnlyState, int> readOnlyHash;
    DoubleHash<QString, int> treeHash;


    QString getWorkerProcessName(WorkerProcess* process);

    QList<NODE_KIND> snippetableParentKinds;
    QList<NODE_KIND> nonSnippetableKinds;


    //Used to tell if we are currently Undo-ing/Redo-ing in the system.
    bool UNDOING;
    bool REDOING;
    bool INITIALIZING;
    bool IMPORTING_SNIPPET;

    Node* model = 0;
    Node* workerDefinitions = 0;
    Node* behaviourDefinitions = 0;
    Node* deploymentDefinitions = 0;
    Node* interfaceDefinitions = 0;
    Node* hardwareDefinitions = 0;
    Node* assemblyDefinitions = 0;
    Node* localhostNode = 0;

    Node* check_for_existing_node(Node* parent_node, NODE_KIND node_kind);
    Node* get_hardware_by_url(Node* parent_node, NODE_KIND kind, QString url);

    //List of undeleteable nodes
    QList<Node*> protectedNodes;

    QList<Node*> getNodes();
    QList<Node*> getNodes(QList<int> IDs);


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
    int currentActionID;
    int currentActionItemID;

    bool questionAnswer;

    bool DESTRUCTING_CONTROLLER;


    QString projectPath;
    bool projectFilePathSet;

    bool projectDirty;

    QThread* controllerThread;
    QReadWriteLock lock_;

    EntityFactory* entity_factory = 0;

};
 QDataStream &operator<<(QDataStream &out, const EventAction &action);

#endif // NEWCONTROLLER_H
