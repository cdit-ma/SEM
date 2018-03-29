#ifndef CONTAINERNODE_H
#define CONTAINERNODE_H
#include "../node.h"

class EntityFactory;
class ContainerNode : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
    ContainerNode(EntityFactory* factory, NODE_KIND kind, QString kind_str);
    ContainerNode(NODE_KIND kind);
public:
    virtual bool canAdoptChild(Node* child);
};

#endif // CONTAINERNODE_H
