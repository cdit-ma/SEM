#ifndef LOGGINGPROFILE_H
#define LOGGINGPROFILE_H
#include "../node.h"

class EntityFactory;
class LoggingProfile : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	LoggingProfile(EntityFactory* factory);
	LoggingProfile();
public:
    bool canAdoptChild(Node*);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // LOGGINGPROFILE_H
