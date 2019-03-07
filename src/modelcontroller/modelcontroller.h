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
#include "dataupdate.h"
#include "viewcontrollerint.h"
class Entity;
class Node;
class Edge;
class Data;
class EntityFactory;
class ViewController;


enum class MODEL_SEVERITY{ERROR, WARNING, INFO};
enum class ACTION_TYPE {CONSTRUCTED, DESTRUCTED, MODIFIED};
enum class SELECTION_PROPERTIES{CAN_CUT, CAN_COPY, CAN_PASTE, CAN_REPLICATE, CAN_REMOVE, CAN_RENAME, GOT_IMPLEMENTATION, GOT_DEFINITION, GOT_INSTANCES, GOT_EDGES, CAN_CHANGE_INDEX, CAN_CHANGE_ROW, CAN_GENERATE_CODE};
enum class MODEL_ACTION{NONE, OPEN, IMPORT, REPLICATE, PASTE, UNDO, REDO, DESTRUCTING, SETUP};

struct HistoryAction{
    struct Action{
        int id;
        int action_id;
        ACTION_TYPE type;
        GRAPHML_KIND kind;
        QString name;
    } Action;

    int entity_id = -1;
    int parent_id = -1;

    QString xml;

    struct Data{
        QString key_name;
        QVariant old_value;
        QVariant new_value;
    } Data;
};

inline QString getActionTypeString(ACTION_TYPE type){
    switch(type){
        case ACTION_TYPE::CONSTRUCTED:
            return "CONSTRUCTED";
        case ACTION_TYPE::DESTRUCTED:
            return "DESTRUCTED";
        case ACTION_TYPE::MODIFIED:
            return "MODIFIED";
    }
};

class ModelController: public QObject
{
    Q_OBJECT

public:
    void ConnectViewController(ViewControllerInterface* view_controller);
    ModelController(const QString& application_dir);
    ~ModelController();
    QString getProjectAsGraphML(bool human_readable = false, bool functional_export = false);

    QSet<SELECTION_PROPERTIES> getSelectionProperties(int active_id, QList<int> ids);
    QList<int> getConstructablesConnectableNodes(int constructable_parent_id, NODE_KIND constructable_node_kind, EDGE_KIND connection_edge_kind);
    QHash<EDGE_DIRECTION, int> getConnectableNodes(QList<int> src, EDGE_KIND edge_kind);
    QSet<NODE_KIND> getValidNodeKinds(int ID);
    QPair<QSet<EDGE_KIND>, QSet<EDGE_KIND> > getValidEdgeKinds(QList<int> IDs);
    QSet<EDGE_KIND> getCurrentEdgeKinds(QList<int> IDs);
    QList<QVariant> getValidKeyValues(int ID, const QString& keyName);
    bool canUndo();
    bool canRedo();

    int getImplementation(int ID);
    int getDefinition(int ID);
    int getSharedParent(int ID, int ID2);
    bool isProjectSaved();
    int getProjectActionCount();
    void setProjectPath(const QString& path);
    QString getProjectPath();
    void setProjectSaved(const QString& path);
    VIEW_ASPECT getNodeViewAspect(int ID);
    bool isNodeAncestor(int ID, int ID2);
    QVariant getEntityDataValue(int ID, const QString& key_name);
    QList<DataUpdate> getEntityDataList(int ID);
    QSet<NODE_TYPE> getNodesTypes(int ID);

        //Clipboard functionality
    QString cut(QList<int> ids);
    QString copy(QList<int> ids);

public slots:
    bool SetupController(const QString& file_path);
    bool importProjects(QStringList xmlDataList);
private slots:
    void paste(QList<int> ids, const QString& xml);
    void replicate(QList<int> ids);
    void remove(QList<int> ids);
    void undo();
    void redo();


    //Model Functionality
    void setData(int id, const QString& key_name, QVariant value);
    void removeData(int id, const QString& key_name);

    //Node/Edge Functionality    
    void constructNodeAtPos(int parentID, NODE_KIND kind, QPointF centerPoint);
    void constructNodeAtIndex(int parentID, NODE_KIND kind, int index);

    void constructConnectedNodeAtPos(int parentID, NODE_KIND nodeKind, int dstID, EDGE_KIND edgeKind, QPointF pos);
    void constructConnectedNodeAtIndex(int parentID, NODE_KIND nodeKind, int dstID, EDGE_KIND edgeKind, int index);
    void constructDDSQOSProfile();
    void constructEdges(QList<int> src, QList<int> dst, EDGE_KIND edge_kind);
    
    void destructEdges(QList<int> srcIDs, QList<int> dstID, EDGE_KIND edgeClass);
    void destructAllEdges(QList<int> srcIDs, EDGE_KIND edge_kind, QSet<EDGE_DIRECTION> edge_direction);

    void triggerAction(const QString& actionName);

    void addDependantsToDependants(Node* parent_node, Node* dependant);
signals:
    //New SIGNAL
    void NodeConstructed(int parent_id, int id, NODE_KIND kind);
    void EdgeConstructed(int id, EDGE_KIND kind, int src_id, int dst_id);
    void EntityDestructed(int ID, GRAPHML_KIND kind);
    
    void InitiateTeardown();
    void ProjectModified(bool modified);

    void DataChanged(int ID, DataUpdate data);
    void DataRemoved(int ID, QString keyName);
    void NodeEdgeKindsChanged(int ID);
    void NodeTypesChanged(int ID);
    
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
    QSet<Node*> UpdateDefinitions(Node* definition, Node* instance);

    Node* constructNode(Node* parent_node, NODE_KIND kind, int index = -1);
    Node* constructConnectedNode(Node* parent_node, NODE_KIND node_kind, Node* dst_node, EDGE_KIND edge_kind, int index = -1);

    bool canReplicate(const QList<Entity *>& ordered_selection);
    bool canCut(const QList<Entity *>& ordered_selection);
    bool canCopy(const QList<Entity *>& ordered_selection);
    bool canPaste(const QList<Entity *>& ordered_selection);
    bool canRemove(const QList<Entity *>& ordered_selection);
    
    bool canChangeIndex(const QList<Entity *>& unordered_selection);
    bool canChangeRow(const QList<Entity *>& unordered_selection);


    bool canDeleteNode(Node* node);

    void loadWorkerDefinitions();
    QList<Entity *> getOrderedEntities(const QList<int>& ids);
    QList<Entity *> getOrderedEntities(QList<Entity *> entities);
    QList<Entity*> getUnorderedEntities(const QList<int>& ids);

    QHash<EDGE_DIRECTION, Node*> _getConnectableNodes(QList<Node*> nodes, EDGE_KIND kind);

    Node* construct_node(Node* parent_node, NODE_KIND kind, int index = -1);
    Node* construct_child_node(Node* parent_node, NODE_KIND kind, int index = -1, bool notify_view = true);
    Node* construct_connected_node(Node* parent_node, NODE_KIND node_kind, Node* dst, EDGE_KIND edge_kind, int index = -1);
    Edge* construct_edge(EDGE_KIND edge_kind, Node* source, Node* destination, bool notify_view = true);
    QList<EDGE_KIND> GetEdgeOrderIndexes();

    void clearHistory();
    static bool isKeyNameVisual(const QString& key_name);

    Node* construct_component_node(Node* parent, int index);

    EDGE_KIND getValidEdgeClass(Node* src, Node* dst);
    QList<EDGE_KIND> getPotentialEdgeClasses(Node* src, Node* dst);


    //Helper functions.
    QString _copy(QList<Entity*> selection);
    bool _paste(Node* node, const QString& xmlData);
    bool _clear();
    bool _replicate(QList<Entity*> items);


    bool setData_(Entity* parent, const QString& keyName, QVariant dataValue, bool addAction = true);
    
    //Checks to see if the provided GraphML document is Valid XML.
    bool isGraphMLValid(const QString& inputGraphML);

    bool importGraphML(const QString& document, Node* parent = 0);


    HistoryAction getNewAction(GRAPHML_KIND kind);

    void setProjectModified(bool modified);
    
    bool gotImplementation(Node* node);
    bool gotInstances(Node* node);

    Node* getDefinition(Node* node);

    bool attachChildNode(Node* parentNode, Node* childNode, bool notify_view = true);

    QList<int> getIDs(QList<Entity*> items);
    QList<int> getIDs(QList<Node*> items);
    


    
    //Sets up an Undo state for the creation of the Node/Edge, and tells the View To construct a GUI Element.
    bool storeNode(Node* node, int desired_id = -1, bool store_children = true, bool add_action = true);
    bool storeEdge(Edge* edge, int desired_id = -1);
    bool storeEntity(Entity* item, int desired_id);
    void removeEntity(Entity* item);

    void destructEntity(int ID);
    void destructEntity(Entity* item);
    void destructEntities(QList<Entity*> entities);
    
    void destructEdge_(Edge* edge);
    void destructNode_(Node* node);
    bool destructData_(Entity* parent, const QString& key_name);

    QString getNodeInstanceKind(Node* definition);
    QString getNodeImplKind(Node* definition);

    //Constructs and setups all required Entities inside the Model Node.
    void setupModel();

    

    //Setup/Teardown the node provided an Instance of the Definition. It will adopt Instances of all Definitions contained by definition and bind all Data which isn't protected.
    bool setupDefinitionRelationship(Node* src, Node* dst, bool setup=true);
    
    bool setupAggregateRelationship(Node* src, Node* dst, bool setup);


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
    
    Node* get_persistent_node(NODE_KIND node_kind);
    void set_persistent_node(Node* node);
    QList<Node*> get_matching_dependant_of_definition(Node* parent_node, Node* definition);
    
    QList<Node*> getNodes(QList<int> IDs);
private:
    void ProgressChanged_(int ID);
    void ProgressUpdated_(const QString& description);
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
    QString application_dir;
   
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

