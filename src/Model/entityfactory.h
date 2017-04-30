#ifndef EntityFactory_H
#define EntityFactory_H

#include <QList>
#include <QHash>

#include "node.h"
#include "edge.h"

class Key;

class ModelController;
class EntityFactory
{
private:
    friend class ModelController;
    struct NodeLookupStruct{
        NODE_KIND kind;
        QString kind_str;
        std::function<Node* ()> constructor;
        Node* node = 0;
    };
    struct EdgeLookupStruct{
        Edge::EDGE_KIND kind;
        QString kind_str;
        std::function<Edge* (Node*, Node*)> constructor;
        Edge* edge = 0;
    };

public:
    static QList<NODE_KIND> getNodeKinds();
    static QList<Edge::EDGE_KIND> getEdgeKinds();

    static NODE_KIND getNodeKind(QString node_kind);
    static Edge::EDGE_KIND getEdgeKind(QString edge_kind);

    static QString getNodeKindString(NODE_KIND node_kind);
    static QString getEdgeKindString(Edge::EDGE_KIND edge_kind);

protected:
    static EntityFactory* globalFactory();
    
    static EntityFactory* getNewFactory();

    
    Node* createNode(QString nodeKind);
    Node* createNode(NODE_KIND nodeKind);

    Edge* createEdge(Node* source, Node* destination, Edge::EDGE_KIND edgeKind);
    Edge* createEdge(Node* source, Node* destination, QString kind);
protected:
    EntityFactory();
    ~EntityFactory();
    QList<Data*> getDefaultNodeData(NODE_KIND kind);
    QList<Data*> getDefaultEdgeData(Edge::EDGE_KIND kind);
    Key* GetKey(QString key_name);
    Key* GetKey(int key_id);
    Key* GetKey(QString key_name, QVariant::Type type);
    QList<Key*> GetKeys();
private:
    QHash<NODE_KIND, NodeLookupStruct*> nodeLookup;
    QHash<Edge::EDGE_KIND, EdgeLookupStruct*> edgeLookup;
    
    QHash<QString, Edge::EDGE_KIND> edgeKindLookup;
    QHash<QString, NODE_KIND> nodeKindLookup;

    Node* _createNode(NODE_KIND kind);
    Edge* _createEdge(Node* source, Node* destination, Edge::EDGE_KIND edge_kind);
    QList<Data*> _constructDefaultData(Entity* entity);

    Edge* getEdge(Edge::EDGE_KIND kind);
    Node* getNode(NODE_KIND kind);
private:
    void addNodeKind(NODE_KIND kind, QString kind_str, std::function<Node* ()> constructor);
    void addEdgeKind(Edge::EDGE_KIND kind, QString kind_str, std::function<Edge* (Node*, Node*)> constructor);
    void setupOnceOffKeys(Node* node);


    QHash<QString, Key*> keyLookup_;



    static EntityFactory* global_factory;
};

#endif // EntityFactory_H
