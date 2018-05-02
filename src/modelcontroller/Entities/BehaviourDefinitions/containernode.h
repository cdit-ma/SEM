#ifndef CONTAINERNODE_H
#define CONTAINERNODE_H
#include "../node.h"

class EntityFactory;
class ContainerNode
{
protected:
    QSet<NODE_KIND> getAcceptedNodeKinds() const;
};

#endif // CONTAINERNODE_H
