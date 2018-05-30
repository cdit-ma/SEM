#ifndef EntityFactory_H
#define EntityFactory_H

#include <QList>
#include <QHash>
#include <QVariant>
#include <QQueue>
#include <functional>

#include "nodekinds.h"
#include "edgekinds.h"

#include "Entities/key.h"
#include "Entities/data.h"
#include "entityfactorybroker.h"


class Key;
class Node;
class Data;
class Edge;
class GraphML;
class Entity;
class ModelController;
class EntityFactoryBroker;
class EntityFactoryRegistryBroker;



class EntityFactory
{
    friend class EntityFactoryRegistryBroker;
    friend class EntityFactoryBroker;

    friend class ModelController;
    friend class Node;
    friend class Edge;
    friend class GraphML;
    friend class ExportIDKey;
private:
    EntityFactory();
    ~EntityFactory();
    static EntityFactory* globalFactory();

    struct NodeLookupStruct{
        NODE_KIND kind;
        QString kind_str = "INVALID_NODE";
        QString pretty_kind_str = "INVALID_NODE";
        std::function<Node* (EntityFactoryBroker&, bool)> constructor;
    };

    struct EdgeLookupStruct{
        EDGE_KIND kind;
        QString kind_str = "INVALID_EDGE";
        QString pretty_kind_str = "INVALID_NODE";
        std::function<Edge* (EntityFactoryBroker&, Node*, Node*)> constructor;
    };


public:
    //Public Static functionality
    static QList<NODE_KIND> getNodeKinds();
    static QList<EDGE_KIND> getEdgeKinds();
    static QList<VIEW_ASPECT> getViewAspects();
    static NODE_KIND getViewAspectKind(VIEW_ASPECT aspect);


    static NODE_KIND getNodeKind(QString node_kind);
    static EDGE_KIND getEdgeKind(QString edge_kind);

    static QString getNodeKindString(NODE_KIND node_kind);
    static QString getEdgeKindString(EDGE_KIND edge_kind);

    static QString getPrettyNodeKindString(NODE_KIND node_kind);
    static QString getPrettyEdgeKindString(EDGE_KIND edge_kind);

protected:
    static EntityFactory* getNewFactory();
    QSet<Node*> GetNodesWhichAcceptEdgeKinds(EDGE_KIND edge_kind, EDGE_DIRECTION direction);
public:
    //Constructors
    Edge* CreateEdge(Node* source, Node* destination, EDGE_KIND edgeKind);
    Node* CreateNode(NODE_KIND node_kind, bool complex = true);
    Node* CreateTempNode(NODE_KIND node_kind);
    
    Data* AttachData(Entity* entity, Key* key, ProtectedState protect_state, QVariant value = QVariant());
    Data* AttachData(Entity* entity, QString key_name, QVariant::Type type, ProtectedState protect_state, QVariant value = QVariant());



    Key* GetKey(QString key_name, QVariant::Type type);
    
    int CacheEntity(GraphML* graphml, int desired_id = -1);
    
    void DestructEntity(GraphML* entity);
    
public:
    Node* ConstructChildNode(Node& parent, NODE_KIND node_kind);
protected:
    void DeregisterNode(Node* node);
    void DeregisterEdge(Edge* edge);
    void DeregisterGraphML(GraphML* graphml);
    //Getters
    Entity* GetEntity(int id);
    Node* GetNode(int id);
    Edge* GetEdge(int id);
    Data* GetData(int id);
    
    //Key getters
    Key* GetKey(int id);
    Key* GetKey(QString key_name);
    
    QList<Key*> GetKeys();

    Entity* GetEntityByUUID(QString uuid);

    //Called by secondary constructors of Node/Edge subclasses
    void RegisterNodeKind(const NODE_KIND kind, const QString& kind_string, std::function<Node* (EntityFactoryBroker&, bool)> constructor);    
    void RegisterEdgeKind(const EDGE_KIND kind, const QString& kind_string, std::function<Edge* (EntityFactoryBroker&, Node*, Node*)> constructor);

    bool RegisterEntity(GraphML* graphml, int desired_id = -1);
    bool IsEntityRegistered(GraphML* graphml);
    
    bool UnregisterTempID(GraphML* graphml);

    void EntitiesUUIDChanged(Entity* entity, QString old_uuid, QString new_uuid);
private:
    void CacheEntityAsUnregistered(GraphML* graphml);

    void AcceptedEdgeKindsChanged(Node* node);
    void clearAcceptedEdgeKinds(Node* node);
    void addNodeKind(NODE_KIND kind, QString kind_str, std::function<Node* ()> constructor);
    void addEdgeKind(EDGE_KIND kind, QString kind_str, std::function<Edge* (Node*, Node*)> constructor);

    
    GraphML* getGraphML(int id);

    
    Node* _createNode(NODE_KIND kind, bool is_temporary = false, bool complex = true);
    Edge* _createEdge(Node* source, Node* destination, EDGE_KIND edge_kind);

    bool doesNodeStructExist(NODE_KIND kind);
    bool doesEdgeStructExist(EDGE_KIND kind);
    NodeLookupStruct* getNodeStruct(NODE_KIND kind);
    EdgeLookupStruct* getEdgeStruct(EDGE_KIND kind);
private:
    int getFreeID(int preferred_id);
    int getUnregisteredFreeID();
    //Hashes
    QHash<NODE_KIND, NodeLookupStruct*> node_struct_lookup;
    QHash<EDGE_KIND, EdgeLookupStruct*> edge_struct_lookup;

    //Hashes
    QHash<QString, EDGE_KIND> edge_kind_lookup;
    QHash<QString, NODE_KIND> node_kind_lookup;


    QHash<QString, int> uuid_lookup_;
    QHash<QString, Key*> key_lookup_;

    //Edge Map
    QHash<EDGE_KIND, QSet<Node*> > accepted_source_edge_map;
    QHash<EDGE_KIND, QSet<Node*> > accepted_target_edge_map;

    QHash<int, GraphML*> hash_;
    QHash<int, GraphML*> unregistered_hash_;
    int id_counter_ = 0;

    int unregistered_id_counter_ = 0;
    QQueue<int> resuable_unregistered_ids_;

    EntityFactoryBroker factory_broker_;

    static EntityFactory* global_factory;
};

#endif // EntityFactory_H
