#ifndef MEDEA_DEPLOYMENT_ATTRIBUTE_H
#define MEDEA_DEPLOYMENT_ATTRIBUTE_H
#include "../InterfaceDefinitions/datanode.h"

class EntityFactory;

namespace MEDEA{
    class DeploymentAttribute : public DataNode
    {
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        DeploymentAttribute(EntityFactory* factory);
        DeploymentAttribute();
    public:
        bool canAdoptChild(Node* child);
        bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
    };
};

#endif //MEDEA_DEPLOYMENT_ATTRIBUTE_H
