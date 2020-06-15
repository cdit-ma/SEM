#ifndef ENTITYFACTORYBROKER_H
#define ENTITYFACTORYBROKER_H

#include "nodekinds.h"
#include "edgekinds.h"

#include "Entities/data.h"
#include "Entities/key.h"

class Node;
class Edge;
class EntityFactory;
enum class ProtectedState;
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
        Key* GetKey(const QString& key_name, QVariant::Type type);

        Data* AttachData(Entity* entity, Key* key, ProtectedState protected_state, QVariant value = QVariant());
        Data* AttachData(Entity* entity, const QString& key_name, QVariant::Type type, ProtectedState protected_state, QVariant value = QVariant());

        void RemoveData(Entity* entity, const QString& key_name);

        void AcceptedEdgeKindsChanged(Node* node);

        void SetAcceptsEdgeKind(Node* node, EDGE_KIND edge_kind, EDGE_DIRECTION direction, bool accept = true);


        QString GetEdgeKindString(EDGE_KIND edge_kind);
        QString GetNodeKindString(NODE_KIND node_kind);
    private:
        EntityFactory& factory_;
};

#endif // ENTITYFACTORYBROKER_H
