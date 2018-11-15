#ifndef DEPLOYMENTCONTAINER_H
#define DEPLOYMENTCONTAINER_H

#include "../node.h"

class EntityFactoryRegistryBroker;

namespace MEDEA{
    class DeploymentContainer : public Node {
        Q_OBJECT

    public:
        static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
        bool canAcceptEdge(EDGE_KIND edge_kind, Node *destination);

    protected:
        DeploymentContainer(EntityFactoryBroker& factory, bool is_temp_node);
        void TypeUpdated();

    };
};


#endif //DEPLOYMENTCONTAINER_H
