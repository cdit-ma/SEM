#ifndef EntityFactory_H
#define EntityFactory_H

#include <QList>
#include <QHash>
#include <QVariant>
#include <functional>

enum class NODE_KIND;
enum class EDGE_KIND;

class Key;
class Node;
class Data;
class Edge;
class ModelController;

class EntityFactory
{
private:
    friend class ModelController;
    friend class Node;
    friend class Edge;

    struct DefaultDataStruct{
        QString key_name;
        QVariant::Type type;
        bool is_protected;
        QVariant value;
    };

    struct NodeLookupStruct{
        NODE_KIND kind;
        QString kind_str;
        std::function<Node* ()> constructor;
        QHash<QString, DefaultDataStruct*> default_data;
    };

    struct EdgeLookupStruct{
        EDGE_KIND kind;
        QString kind_str;
        std::function<Edge* (Node*, Node*)> constructor;
        QHash<QString, DefaultDataStruct*> default_data;
        Edge* edge = 0;
    };

public:
    static QList<NODE_KIND> getNodeKinds();
    static QList<EDGE_KIND> getEdgeKinds();

    static NODE_KIND getNodeKind(QString node_kind);
    static EDGE_KIND getEdgeKind(QString edge_kind);

    static QString getNodeKindString(NODE_KIND node_kind);
    static QString getEdgeKindString(EDGE_KIND edge_kind);

protected:
    static EntityFactory* globalFactory();
    static EntityFactory* getNewFactory();

    Node* createNode(NODE_KIND node_kind);
    Node* createTempNode(NODE_KIND node_kind);
    Edge* createEdge(Node* source, Node* destination, EDGE_KIND edgeKind);

    void destructNode(Node* node);
    void destructEdge(Edge* node);

    void RegisterNodeKind(NODE_KIND kind, QString kind_string, std::function<Node* ()> constructor);
    void RegisterEdgeKind(EDGE_KIND kind, QString kind_string, std::function<Edge* (Node*, Node*)> constructor);
    void RegisterDefaultData(EDGE_KIND kind, QString key_name, QVariant::Type type, bool is_protected = false, QVariant value = QVariant());
    void RegisterDefaultData(NODE_KIND kind, QString key_name, QVariant::Type type, bool is_protected = false, QVariant value = QVariant());

    void RegisterValidDataValues(NODE_KIND kind, QString key_name, QVariant::Type type, QList<QVariant> values);
protected:
   
    QList<Data*> getDefaultNodeData(NODE_KIND kind);
    QList<Data*> getDefaultEdgeData(EDGE_KIND kind);
    Key* GetKey(QString key_name);
    Key* GetKey(int key_id);
    Key* GetKey(QString key_name, QVariant::Type type);
    QList<Key*> GetKeys();
private:
    QList<Data*> getDefaultData(QList<DefaultDataStruct*> data);
    EntityFactory();
    ~EntityFactory();
    Node* _createNode(NODE_KIND kind, bool attach_data);
    Edge* _createEdge(Node* source, Node* destination, EDGE_KIND edge_kind, bool attach_data);
    
    NodeLookupStruct* getNodeStruct(NODE_KIND kind);
    EdgeLookupStruct* getEdgeStruct(EDGE_KIND kind);

    QHash<NODE_KIND, NodeLookupStruct*> node_struct_lookup;
    QHash<EDGE_KIND, EdgeLookupStruct*> edge_struct_lookup;
    QHash<QString, EDGE_KIND> edge_kind_lookup;
    QHash<QString, NODE_KIND> node_kind_lookup;
private:

    void addNodeKind(NODE_KIND kind, QString kind_str, std::function<Node* ()> constructor);
    void addEdgeKind(EDGE_KIND kind, QString kind_str, std::function<Edge* (Node*, Node*)> constructor);


    QHash<QString, Key*> keyLookup_;



    static EntityFactory* global_factory;
};

#endif // EntityFactory_H
