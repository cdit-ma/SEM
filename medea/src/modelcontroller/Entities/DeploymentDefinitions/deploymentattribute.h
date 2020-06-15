#ifndef MEDEA_DEPLOYMENT_ATTRIBUTE_H
#define MEDEA_DEPLOYMENT_ATTRIBUTE_H
#include "../InterfaceDefinitions/datanode.h"



class EntityFactoryRegistryBroker;
namespace MEDEA{
    class DeploymentAttribute : public DataNode
    {

    friend class ::EntityFactory;
    protected:
public:
        static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
        DeploymentAttribute(EntityFactoryBroker& factory, bool is_temp_node);
    public:
        bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
    };
};

#endif //MEDEA_DEPLOYMENT_ATTRIBUTE_H
