#ifndef ENTITYFACTORY_REGISTERBROKER_H
#define ENTITYFACTORY_REGISTERBROKER_H

#include <functional>
#include "nodekinds.h"
#include "edgekinds.h"

class Node;
class Edge;
class EntityFactory;
class EntityFactoryBroker;

class EntityFactoryRegistryBroker
{
    friend class EntityFactory;
    protected:
        EntityFactoryRegistryBroker(EntityFactory& factory);
    public:
        void RegisterWithEntityFactory(const NODE_KIND& node_kind, const QString& kind_string, std::function<Node* (EntityFactoryBroker&, bool)> constructor);
        void RegisterWithEntityFactory(const EDGE_KIND& edge_kind, const QString& kind_string, std::function<Edge* (EntityFactoryBroker&, Node*, Node*)> constructor);
    private:
        EntityFactory& factory_;
};

#endif // ENTITYFACTORY_REGISTERBROKER_H
