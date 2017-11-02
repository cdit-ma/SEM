#ifndef MODELCONTROLLER_H
#define MODELCONTROLLER_H

#include <QSet>
#include <QHash>
#include <QObject>
#include <QStack>
#include <QPointF>
#include <QReadWriteLock>
#include <QVariant>

#include "kinds.h"
#include "nodekinds.h"
#include "edgekinds.h"

class Entity;
class Node;
class Edge;
class Data;
class EntityFactory;

enum class MODEL_SEVERITY{ERROR, WARNING, INFO};
enum class ACTION_TYPE {CONSTRUCTED, DESTRUCTED, MODIFIED};
enum class SELECTION_PROPERTIES{CAN_CUT, CAN_COPY, CAN_PASTE, CAN_REPLICATE, CAN_REMOVE, CAN_RENAME, GOT_IMPLEMENTATION, GOT_DEFINITION, GOT_INSTANCES, GOT_EDGES};
enum class MODEL_ACTION{NONE, OPEN, IMPORT, REPLICATE, PASTE, UNDO, REDO};

struct HistoryAction{
    struct Action{
        int id;
        int action_id;
        ACTION_TYPE type;
        GRAPHML_KIND kind;
        QString name;
    } Action;

    int entity_id;
    int parent_id;

    QString xml;

    struct Data{
        QString key_name;
        QVariant old_value;
        QVariant new_value;
    } Data;
};

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
    int getProjectActionCount();
    void setProjectSaved(QString path="");

    QString getProjectAsGraphML(bool functional_export = false);

    QList<EDGE_KIND> getValidEdgeKindsForSelection(QList<int> IDs);
    QList<EDGE_KIND> getExistingEdgeKindsForSelection(QList<int> IDs);
    QSet<NODE_KIND> getAdoptableNodeKinds(int ID);
    
    QList<QVariant> getValidKeyValues(int ID, QString keyName);

    QList<int> getConnectableNodeIDs(QList<int> srcs, EDGE_KIND edgeKind);
    QList<int> getConstructableConnectableNodes(int parentID, NODE_KIND nodeKind, EDGE_KIND edgeClass);
    QMap<EDGE_DIRECTION, int> getConnectableNodeIds2(QList<int> src, EDGE_KIND kind);
    QList<int> getWorkerFunctions();
    

    bool isNodeOfType(int ID, NODE_TYPE type);
    int getNodeParentID(int ID);
    VIEW_ASPECT getNodeViewAspect(int ID);
    int getSharedParent(int ID, int ID2);
    bool isNodeAncestor(int ID, int ID2);

    QStringList getProtectedEntityKeys(int ID);
    QStringList getEntityKeys(int ID);
    QVariant getEntityDataValue(int ID, QString key_name);

    QSet<SELECTION_PROPERTIES> getSelectionProperties(int active_id, QList<int> ids);

    bool canUndo();
    bool canRedo();

    int getDefinition(int ID);
    int getImplementation(int ID);
    QList<int> getInstances(int ID);
    int getAggregate(int ID);
    int getDeployedHardwareID(int ID);
public slots:
    bool SetupController(QString file_path="");

    //Clipboard functionality
    void cut(QList<int> ids);
    void copy(QList<int> ids);
    void paste(QList<int> ids, QString xml);
    void replicate(QList<int> ids);
    void remove(QList<int> ids);
    void undo();
    void redo();

    bool importProjects(QStringList xmlDataList);

    //Model Functionality
    void setData(int id, QString key_name, QVariant value);
    void removeData(int id, QString key_name);

    //Node/Edge Functionality    
    void constructNode(int parentID, NODE_KIND kind, QPointF centerPoint);
    void constructEdge(QList<int> srcIDs, int dstID, EDGE_KIND edgeClass);
    void constructEdges(QList<int> src, QList<int> dst, EDGE_KIND edge_kind);
    
    void constructWorkerProcess(int parent_id, int dst_id, QPointF centerPoint);
    void constructConnectedNode(int parentID, NODE_KIND nodeKind, int dstID, EDGE_KIND edgeKind, QPointF pos=QPointF());

    void destructEdges(QList<int> srcIDs, int dstID, EDGE_KIND edgeClass);
    void destructAllEdges(QList<int> srcIDs, EDGE_KIND edgeClass);

    void triggerAction(QString actionName);
private slots:
    void ModelNameChanged();
signals:
    void highlight(QList<int> ids);
    void ActionProcessing(bool running, bool success = false, QString error_string= "");
    //New SIGNAL
    void NodeConstructed(int parent_id, int id, NODE_KIND kind);
    void EdgeConstructed(int id, EDGE_KIND kind, int src_id, int dst_id);
    void EntityDestructed(int ID, GRAPHML_KIND kind);
    
    void InitiateTeardown();
    void ProjectModified(bool modified);

    void DataChanged(int ID, QString keyName, QVariant data);
    void DataRemoved(int ID, QString keyName);
    
    void ProjectFileChanged(QString);
    void ProjectNameChanged(QString);
    void UndoRedoUpdated();
    void ModelReady(bool ready);
    void ProgressChanged(int);
    void ShowProgress(bool, QString = "");
    void ActionFinished();

    void SetClipboardData(QString);
    void Notification(MODEL_SEVERITY severity, QString title, QString description="", int entity_id = -1);
private:
    QSet<NODE_KIND> getGUINodeKinds();
    bool canReplicate(QList<Entity*> selection);
    bool canCut(QList<Entity*> selection);
    bool canCopy(QList<Entity*> selection);
    bool canPaste(QList<Entity*> selection);
    bool canRemove(QList<Entity *> selection);
    bool canDeleteNode(Node* node);

    void loadWorkerDefinitions();
    QList<Entity *> getOrderedEntities(QList<int> ids);
    QList<Entity *> getOrderedEntities(QList<Entity *> entities);
    QList<Entity*> getUnorderedEntities(QList<int> IDs);

    QList<Node*> _getConnectableNodes(QList<Node*> sourceNodes, EDGE_KIND edgeKind);
    QMap<EDGE_DIRECTION, Node*> _getConnectableNodes2(QList<Node*> sourceNodes, EDGE_KIND kind);

    Node* construct_temp_node(Node* parent_node, NODE_KIND kind);
    Node* construct_node(Node* parent_node, NODE_KIND kind, int id = -1);
    Node* construct_child_node(Node* parent_node, NODE_KIND kind, bool notify_view = true);
    Node* construct_connected_node(Node* parent_node, NODE_KIND node_kind, Node* dst, EDGE_KIND edge_kind);
    Edge* construct_edge(EDGE_KIND edge_kind, Node* source, Node* destination, int id = -1, bool notify_view = true);
    int GetEdgeOrderIndex(EDGE_KIND kind);
    QList<EDGE_KIND> GetEdgeOrderIndexes();

    void clearHistory();
    bool isDataVisual(Data* data);
    static bool isKeyNameVisual(QString key_name);

    Node* construct_setter_node(Node* parent);
    Node* construct_dds_profile_node(Node* parent);
    Node* construct_for_condition_node(Node* parent);
    Node* construct_component_node(Node* parent);

    EDGE_KIND getValidEdgeClass(Node* src, Node* dst);
    QList<EDGE_KIND> getPotentialEdgeClasses(Node* src, Node* dst);


    //Helper functions.
    QString _copy(QList<Entity*> selection);
    bool _paste(Node* node, QString xmlData);
    bool _clear();
    bool _replicate(QList<Entity*> items);


    bool setData_(Entity* parent, QString keyName, QVariant dataValue, bool addAction = true);
    
    //Checks to see if the provided GraphML document is Valid XML.
    bool isGraphMLValid(QString inputGraphML);

    bool importGraphML(QString document, Node* parent = 0);
    //Exports a Selection of Containers to export into GraphML
    QString exportGraphML(QList<int> ids, bool all_edges = false);
    QString exportGraphML(QList<Entity*> entities, bool all_edges = false, bool functional_export = false);
    QString exportGraphML(Entity* entity);


    HistoryAction getNewAction(GRAPHML_KIND kind);

    void setProjectModified(bool modified);
    
    Node* getImplementation(Node* node);
    Node* getDefinition(Node* node);
    QList<Node*> getInstances(Node* node);

    //Finds or Constructs a Node Instance or Implementation inside parent of Definition.
    int constructDependantRelative(Node* parent, Node* definition);

   

    bool attachChildNode(Node* parentNode, Node* childNode, bool notify_view = true);

    QList<int> getIDs(QList<Entity*> items);
    QList<int> getIDs(QList<Node*> items);
    


    Node* cloneNode(Node* original, Node* parent);
    
    //Sets up an Undo state for the creation of the Node/Edge, and tells the View To construct a GUI Element.
    void storeNode(Node* node);
    void storeEdge(Edge* edge);
    void storeEntity(Entity* item);
    void removeEntity(Entity* item);

    bool destructEntity(int ID);
    bool destructEntity(Entity* item);
    bool destructEntities(QList<Entity*> entities);
    
    void destructEdge_(Edge* edge);
    void destructNode_(Node* node);
    bool destructData_(Entity* parent, QString key_name);

    QString getNodeInstanceKind(Node* definition);
    QString getNodeImplKind(Node* definition);

    //Constructs and setups all required Entities inside the Model Node.
    void setupModel();

    //Binds matching Data elements from the Node Child, to the Node Definition.
    void bindData(Node* definition, Node* instance);
    void unbindData(Node* definition, Node* instance);

    bool bindData_(Node* src, QString src_key, Node* dst, QString dst_key);
    bool unbindData_(Node* src, QString src_key, Node* dst, QString dst_key);

    //Setup/Teardown the node provided an Instance of the Definition. It will adopt Instances of all Definitions contained by definition and bind all Data which isn't protected.
    bool setupDefinitionRelationship(Node* src, Node* dst, bool setup=true);
    bool setupAggregateRelationship(Node* src, Node* dst, bool setup);
    bool setupDataRelationship(Node *src, Node *dst, bool setup = true);

    //Used by Undo/Redo to reverse an ActionItem from the Stacks.4
    bool reverseAction(HistoryAction action);

    //Adds an ActionItem to the Undo/Redo Stack depending on the State of the application.
    void addActionToStack(HistoryAction action, bool addAction=true);

    //Undo's/Redo's all of the ActionItems in the Stack which have been performed since the last operation.
    bool undoRedo();

    void setModelAction(MODEL_ACTION action);
    bool isModelAction(MODEL_ACTION action);
    void unsetModelAction(MODEL_ACTION action);
    bool isUserAction();
    
    Node* check_for_existing_node(Node* parent_node, NODE_KIND node_kind);
    
    QList<Node*> getNodes(QList<int> IDs);
private:
    void ProgressChanged_(int ID);
    void ProgressUpdated_(QString description);
    QString last_description;
    int last_progress = -2;
    //List of undeleteable nodes
    QList<Node*> protected_nodes;

    //Stores a list of node/edge ids.
    QSet<int> node_ids_;
    QSet<int> edge_ids_;

    //Stack of ActionItems in the Undo/Redo Stack.
    QStack<HistoryAction> undo_stack;
    QStack<HistoryAction> redo_stack;

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

#endif // MODELCONTROLLER_H