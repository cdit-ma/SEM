#ifndef LOGGINGSERVER_H
#define LOGGINGSERVER_H
#include "../node.h"


class EntityFactoryRegistryBroker;
class LoggingServer : public Node
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    LoggingServer(EntityFactoryBroker& factory, bool is_temp_node);
};

#endif // LOGGINGSERVER_H
