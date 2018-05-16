#ifndef ENTITYFACTORYBROKER_H
#define ENTITYFACTORYBROKER_H

#include "nodekinds.h"
#include "edgekinds.h"

#include "Entities/data.h"
#include "Entities/key.h"

class Node;
class Edge;
class EntityFactory;

class EntityFactoryBroker
{
    friend class EntityFactory;
    friend class GraphML;
    friend class Node;
    friend class Edge;
    protected:
        EntityFactoryBroker(EntityFactory& factory);
        EntityFactory& GetEntityFactory();
        void DeregisterGraphML(GraphML* graphml);
        void DeregisterNode(Node* node);
        void DeregisterEdge(Edge* edge);
    public:
        Node* ConstructChildNode(Node& parent, NODE_KIND node_kind);
        Key* GetKey(QString key_name, QVariant::Type type);

        Data* AttachData(Entity* entity, Key* key, QVariant value = QVariant(), bool is_protected = false);
        Data* AttachData(Entity* entity, QString key_name, QVariant::Type type, QVariant value = QVariant(), bool is_protected = false);
        void RemoveData(Entity* entity, QString key_name);

        void ProtectData(Entity* entity, QString key_name, bool is_protected = true);

        void AcceptedEdgeKindsChanged(Node* node);

        QString GetEdgeKindString(EDGE_KIND edge_kind);
        QString GetNodeKindString(NODE_KIND node_kind);
    private:
        EntityFactory& factory_;
};

#endif // ENTITYFACTORYBROKER_H
