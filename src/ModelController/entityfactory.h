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

    //Constructors
    Edge* CreateEdge(Node* source, Node* destination, EDGE_KIND edgeKind, int id = -1);
    Node* CreateNode(NODE_KIND node_kind, int id = -1);
    Node* CreateTempNode(NODE_KIND node_kind);
    
    Data* CreateData(Key* key, QVariant value = QVariant(), bool is_protected = false);
    
    //Destructor
    void DestructEntity(GraphML* entity);

    //Getters
    Entity* GetEntity(int id);
    Node* GetNode(int id);
    Edge* GetEdge(int id);
    Data* GetData(int id);
    
    //Key getters
    Key* GetKey(int id);
    Key* GetKey(QString key_name);
    Key* GetKey(QString key_name, QVariant::Type type);
    QList<Key*> GetKeys();

    Entity* GetEntityByUUID(QString uuid);

    //Called by secondary constructors of Node/Edge subclasses
    void RegisterNodeKind(NODE_KIND kind, QString kind_string, std::function<Node* ()> constructor);
    void RegisterEdgeKind(EDGE_KIND kind, QString kind_string, std::function<Edge* (Node*, Node*)> constructor);
    void RegisterDefaultData(EDGE_KIND kind, QString key_name, QVariant::Type type, bool is_protected = false, QVariant value = QVariant());
    void RegisterDefaultData(NODE_KIND kind, QString key_name, QVariant::Type type, bool is_protected = false, QVariant value = QVariant());
    void RegisterValidDataValues(NODE_KIND kind, QString key_name, QVariant::Type type, QList<QVariant> values);

    //Called after a GraphML has an EntityFactory Set    
    void RegisterEntity(GraphML* graphml, int id = -1);
    //Called during the destructor of a GraphML entity
    void DeregisterEntity(GraphML* graphml);

    void EntityUUIDChanged(Entity* entity, QString uuid);
private:
    void addNodeKind(NODE_KIND kind, QString kind_str, std::function<Node* ()> constructor);
    void addEdgeKind(EDGE_KIND kind, QString kind_str, std::function<Edge* (Node*, Node*)> constructor);

    QList<Data*> getDefaultNodeData(NODE_KIND kind);
    QList<Data*> getDefaultEdgeData(EDGE_KIND kind);
    QList<Data*> getDefaultData(QList<DefaultDataStruct*> data);
    
    GraphML* getGraphML(int id);
    void StoreEntity(GraphML* graphml, int id = -1);

    
    Node* _createNode(NODE_KIND kind, bool is_temporary = false, int id = -1);
    Edge* _createEdge(Node* source, Node* destination, EDGE_KIND edge_kind, int id = -1);

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
    
    QHash<int, GraphML*> hash_;
    int id_counter_ = 0;

    static EntityFactory* global_factory;
};

#endif // EntityFactory_H
