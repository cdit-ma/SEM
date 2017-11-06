#ifndef OPENCLPLATFORM_H
#define OPENCLPLATFORM_H
#include "../node.h"

class EntityFactory;
class OpenCLPlatform : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	OpenCLPlatform(EntityFactory* factory);
	OpenCLPlatform();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // OPENCLPLATFORM_H
