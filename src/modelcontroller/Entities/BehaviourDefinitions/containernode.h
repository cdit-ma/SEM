#ifndef CONTAINERNODE_H
#define CONTAINERNODE_H
#include "../node.h"

class EntityFactory;
class ContainerNode
{
protected:
    bool canAdoptChild(Node* child);
};

#endif // CONTAINERNODE_H
