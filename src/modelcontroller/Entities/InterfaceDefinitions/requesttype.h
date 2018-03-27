#ifndef REQUESTTYPE_H
#define REQUESTTYPE_H
#include "eventport.h"

class EntityFactory;
class RequestType : public EventPort
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	RequestType(EntityFactory* factory);
	RequestType();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // RequestType_H
