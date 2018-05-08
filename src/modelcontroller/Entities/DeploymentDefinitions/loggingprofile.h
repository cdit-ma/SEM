#ifndef LOGGINGPROFILE_H
#define LOGGINGPROFILE_H
#include "../node.h"

class EntityFactory;
class LoggingProfile : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	static void RegisterWithEntityFactory(EntityFactory& factory);
    LoggingProfile(EntityFactory& factory, bool is_temp_node);
public:
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // LOGGINGPROFILE_H
