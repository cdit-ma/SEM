#ifndef CONTAINERNODE_H
#define CONTAINERNODE_H
#include "../node.h"


class EntityFactoryRegistryBroker;
class ContainerNode
{
    public:
        static QSet<NODE_KIND> getAcceptedNodeKinds();
        static Node* getTopBehaviourContainer(Node* node);
};

#endif // CONTAINERNODE_H
