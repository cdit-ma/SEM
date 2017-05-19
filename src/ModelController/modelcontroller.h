#ifndef MODELCONTROLLER_H
#define MODELCONTROLLER_H

#include <QHash>
#include <QObject>
#include <QStack>
#include <QPointF>
#include <QXmlStreamReader>
#include <QReadWriteLock>
#include <QVariant>
#include <QSet>

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
enum class MODEL_SEVERITY{ERROR, WARNING, INFO};
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

enum class SELECTION_PROPERTIES{CAN_CUT, CAN_COPY, CAN_PASTE, CAN_REPLICATE, CAN_REMOVE, CAN_RENAME, GOT_IMPLEMENTATION, GOT_DEFINITION, GOT_INSTANCES, GOT_EDGES};

enum class MODEL_ACTION{NONE, OPEN, IMPORT, REPLICATE, PASTE, UNDO, REDO};
inline uint qHash(SELECTION_PROPERTIES key, uint seed)
{
    return ::qHash(static_cast<uint>(key), seed);
};
Q_DECLARE_METATYPE(SELECTION_PROPERTIES);

inline uint qHash(MODEL_ACTION key, uint seed)
{
    return ::qHash(static_cast<uint>(key), seed);
};
Q_DECLARE_METATYPE(MODEL_ACTION);


class ModelController: public QObject
{
    Q_OBJECT
public:
    
    static QStringList getVisualKeys();

    ModelController();
    ~ModelController();


    QString getProjectPath();
    void setProjectPath(QString path);

    bool isProjectSaved();
    void setProjectSaved(QString path="");


    //READ
    QString getProjectAsGraphML();

    QList<EDGE_KIND> getValidEdgeKindsForSelection(QList<int> IDs);
    QList<EDGE_KIND> getExistingEdgeKindsForSelection(QList<int> IDs);

    QList<NODE_KIND> getAdoptableNodeKinds(int ID);
    
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
    QSet<SELECTION_PROPERTIES> getSelectionProperties(int active_id, QList<int> ids);

    bool canUndo();
    bool canRedo();

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


signals:
    //New SIGNAL
    void NodeConstructed(int parent_id, int id, NODE_KIND kind);
    void EdgeConstructed(int id, EDGE_KIND kind, int src_id, int dst_id);
    
    void projectModified(bool modified);
    void initiateTeardown();


    void entityConstructed(int ID, GRAPHML_KIND eKind, QString kind, QHash<QString, QVariant> data, QHash<QString, QVariant> properties);
    void entityDestructed(int ID, GRAPHML_KIND eKind, QString kind);


    void dataChanged(int ID, QString keyName, QVariant data);
    void dataRemoved(int ID, QString keyName);
    
    //DO I NEED?
    void propertyChanged(int ID, QString propertyName, QVariant data);
    void propertyRemoved(int ID, QString propertyName);


    void controller_ProjectFileChanged(QString);
    void controller_ProjectNameChanged(QString);
    void undoRedoChanged();
    void controller_IsModelReady(bool ready);
    void progressChanged(int);
    void showProgress(bool, QString = "");
    void controller_ActionFinished(bool actionSucceeded = true, QString errorCode = "");
    void controller_OpenFinished(bool success);

    void controller_SetClipboardBuffer(QString);

    void controller_Notification(MODEL_SEVERITY severity, QString description, int entity_id = -1);
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
    QList<EDGE_KIND> GetEdgeOrderIndexes();
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


    //Toolbar/Dock Functionality
    void clear();



    void constructEdge(QList<int> srcIDs, int dstID, EDGE_KIND edgeClass);
    void destructEdges(QList<int> srcIDs, int dstID, EDGE_KIND edgeClass);

    void destructAllEdges(QList<int> srcIDs, EDGE_KIND edgeClass);
    

    





    void _projectNameChanged();

private:
    bool isDataVisual(Data* data);
    static bool isKeyNameVisual(QString key_name);


    Node* construct_setter_node(Node* parent);
    Node* construct_dds_profile_node(Node* parent);
    Node* construct_for_condition_node(Node* parent);



    EDGE_KIND getValidEdgeClass(Node* src, Node* dst);
    QList<EDGE_KIND> getPotentialEdgeClasses(Node* src, Node* dst);
    void clearHistory();

    QString _copy(QList<Entity*> selection);

    //Helper functions.
    bool _paste(int ID, QString xmlData);
    bool _cut(QList<int> IDs);
    bool _clear();
    bool _copy(QList<int> IDs);
    bool _remove(QList<int> IDs, bool addAction = true);
    bool _replicate(QList<Entity*> items);
    bool _replicate(QList<int> IDs, bool addAction = true);

    void attachData(Entity* parent, Data* data, bool addAction = true);
    bool destructData(Entity* parent, QString keyName, bool addAction = true);

    void updateUndoRedoState();
    void _setData(Entity* parent, QString keyName, QVariant dataValue, bool addAction = true);
    void clearUndoHistory();

    bool importGraphML(MODEL_ACTION action, QString document, Node* parent = 0);
    //Exports a Selection of Containers to export into GraphML
    QString exportGraphML(QList<int> ids, bool all_edges = false);
    QString exportGraphML(QList<Entity*> entities, bool all_edges = false);
    QString exportGraphML(Entity* entity);

    

    EventAction getEventAction();

    void setProjectModified(bool modified);

    Node* getSharedParent(QList<int> IDs);





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
    void undoRedo();


    bool canDeleteNode(Node* node);


    QPair<bool, QString> readFile(QString filePath);
    
    //Attach Data('s) to the GraphML item.
    bool _attachData(Entity* item, Data* data, bool addAction = true);
    //bool _attachData(Entity* item, QList<QStringList> dataList, bool addAction = true);
    bool _attachData(Entity* item, QList<Data*> dataList, bool addAction = true);
    bool _attachData(Entity *item, QString keyName, QVariant value, bool addAction = true);

    


    void setModelAction(MODEL_ACTION action);
    bool isModelAction(MODEL_ACTION action);
    void unsetModelAction(MODEL_ACTION action);

    //Gets the GraphML/Node/Edge Item from the ID provided. Checks the Hash.
    Node* getFirstNodeFromList(QList<int> ID);
    

    //Used to find old ID's which may have been deleted from the Model. Will find the replacement ID if they exist.
    int get_linked_ids(int ID);
    void link_ids(int oldID, int newID);

    bool isUserAction();
    

  

    Entity* getEntityByUUID(QString uuid);

    QString getTimeStamp();
    uint getTimeStampEpoch();

    QString getDataValueFromKeyName(QList<Data*> dataList, QString keyName);
    void setDataValueFromKeyName(QList<Data*> dataList, QString keyName, QString value);

    Node* check_for_existing_node(Node* parent_node, NODE_KIND node_kind);

    QList<Node*> getNodes();
    QList<Node*> getNodes(QList<int> IDs);

private:
    //List of undeleteable nodes
    QList<Node*> protected_nodes;

    //Provides a lookup for IDs.
    QHash<int, int> id_hash_;
    QHash<QString, int> uuid_hash_;
    QHash<int, Entity*> entity_hash_;


    //Stores the list of nodeID's and EdgeID's inside the Hash.
    QList<int> node_ids_;
    QList<int> edge_ids_;

    //Stack of ActionItems in the Undo/Redo Stack.
    QStack<EventAction> undo_stack;
    QStack<EventAction> redo_stack;

    //Used to tell if we are currently Undo-ing/Redo-ing in the system. 
    bool INITIALIZING = true;
    bool DESTRUCTING_CONTROLLER = false;

    bool project_modified = false;   

    QString project_path;

    //Model and Aspect pointers
    Node* model = 0;
    Node* interfaceDefinitions = 0;
    Node* behaviourDefinitions = 0;
    Node* deploymentDefinitions = 0;
    Node* assemblyDefinitions = 0;
    Node* hardwareDefinitions = 0;
    Node* workerDefinitions = 0;


    int actionCount = 0;
    int currentActionID = 0;
    int currentActionItemID = 0;
    QString currentAction;
   

    QThread* controller_thread = 0;
    EntityFactory* entity_factory = 0;
    QReadWriteLock lock_;

    QSet<MODEL_ACTION> model_actions;
};

#endif // MODELCONTROLLER_H