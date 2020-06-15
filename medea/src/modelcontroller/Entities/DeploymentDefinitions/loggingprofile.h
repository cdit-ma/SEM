#ifndef LOGGINGPROFILE_H
#define LOGGINGPROFILE_H
#include "../node.h"


class EntityFactoryRegistryBroker;
class LoggingProfile : public Node
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    LoggingProfile(EntityFactoryBroker& factory, bool is_temp_node);
public:
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // LOGGINGPROFILE_H
