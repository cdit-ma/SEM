#ifndef EntityFactory_H
#define EntityFactory_H

#include <QList>
#include <QHash>
#include <QVariant>
#include <functional>

#include "nodekinds.h"
#include "edgekinds.h"

class Key;
class Node;
class Data;
class Edge;
class GraphML;
class Entity;
class ModelController;

class EntityFactory
{
    friend class ModelController;
    friend class Node;
    friend class Edge;
    friend class GraphML;
    friend class ExportIDKey;
private:
    EntityFactory();
    ~EntityFactory();
    static EntityFactory* globalFactory();

    //Structs used to store lookups
    struct DefaultDataStruct{
        QString key_name;
        QVariant::Type type;
        bool is_protected;
        QVariant value;
    };

    struct NodeLookupStruct{
        NODE_KIND kind;
        QString kind_str = "INVALID_NODE";
        std::function<Node* ()> constructor;
        std::function<Node* (EntityFactory*)> complex_constructor;
        QHash<QString, DefaultDataStruct*> default_data;
    };

    struct EdgeLookupStruct{
        EDGE_KIND kind;
        QString kind_str = "INVALID_EDGE";
        std::function<Edge* (Node*, Node*)> constructor;
        QHash<QString, DefaultDataStruct*> default_data;
        Edge* edge = 0;
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

protected:
    static EntityFactory* getNewFactory();

    QSet<Node*> GetNodesWhichAcceptEdgeKinds(EDGE_KIND edge_kind, EDGE_DIRECTION direction);

public:
    //Constructors
    Edge* CreateEdge(Node* source, Node* destination, EDGE_KIND edgeKind);
    Node* CreateNode(NODE_KIND node_kind, bool complex = true);
    Node* CreateTempNode(NODE_KIND node_kind);
    
    Data* CreateData(Key* key, QVariant value = QVariant(), bool is_protected = false);
    Data* AttachData(Entity* entity, Key* key, QVariant value = QVariant(), bool is_protected = false);

    Key* GetKey(QString key_name, QVariant::Type type);
    
    int CacheEntity(GraphML* graphml, int desired_id = -1);
    //Destructor
    void DestructEntity(GraphML* entity);
protected:
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
    void RegisterNodeKind(NODE_KIND kind, QString kind_string, std::function<Node* ()> constructor);
    void RegisterComplexNodeKind(NODE_KIND kind, std::function<Node* (EntityFactory*)> constructor);
    void RegisterEdgeKind(EDGE_KIND kind, QString kind_string, std::function<Edge* (Node*, Node*)> constructor);
    void RegisterDefaultData(EDGE_KIND kind, QString key_name, QVariant::Type type, bool is_protected = false, QVariant value = QVariant());
    void RegisterDefaultData(NODE_KIND kind, QString key_name, QVariant::Type type, bool is_protected = false, QVariant value = QVariant());
    void RegisterValidDataValues(NODE_KIND kind, QString key_name, QVariant::Type type, QList<QVariant> values);

    int RegisterEntity(GraphML* graphml, int desired_id = -1);
    void DeregisterEntity(GraphML* graphml);

    void EntityUUIDChanged(Entity* entity, QString uuid);
private:
    void CacheEntityAsUnregistered(GraphML* graphml);

    void acceptedEdgeKindsChanged(Node* node);
    void clearAcceptedEdgeKinds(Node* node);
    void addNodeKind(NODE_KIND kind, QString kind_str, std::function<Node* ()> constructor);
    void addEdgeKind(EDGE_KIND kind, QString kind_str, std::function<Edge* (Node*, Node*)> constructor);

    QList<Data*> getDefaultNodeData(NODE_KIND kind);
    QList<Data*> getDefaultEdgeData(EDGE_KIND kind);
    QList<Data*> getDefaultData(QList<DefaultDataStruct*> data);
    
    GraphML* getGraphML(int id);

    
    Node* _createNode(NODE_KIND kind, bool is_temporary = false, bool complex = true);
    Edge* _createEdge(Node* source, Node* destination, EDGE_KIND edge_kind);

    NodeLookupStruct* getNodeStruct(NODE_KIND kind);
    EdgeLookupStruct* getEdgeStruct(EDGE_KIND kind);
private:
    //Hashes
    QHash<NODE_KIND, NodeLookupStruct*> node_struct_lookup;
    QHash<EDGE_KIND, EdgeLookupStruct*> edge_struct_lookup;
    QHash<QString, EDGE_KIND> edge_kind_lookup;
    QHash<QString, NODE_KIND> node_kind_lookup;

    QHash<QString, int> uuid_lookup_;
    QHash<QString, Key*> key_lookup_;

    QHash<EDGE_KIND, QSet<Node*> > accepted_source_edge_map;
    QHash<EDGE_KIND, QSet<Node*> > accepted_target_edge_map;

    QHash<int, GraphML*> hash_;
    QHash<int, GraphML*> unregistered_hash_;
    int id_counter_ = 0;
    int unregistered_id_counter_ = 0;

    static EntityFactory* global_factory;
};

#endif // EntityFactory_H
