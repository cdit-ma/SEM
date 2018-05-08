#ifndef LOGGINGSERVER_H
#define LOGGINGSERVER_H
#include "../node.h"

class EntityFactory;
class LoggingServer : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	static void RegisterWithEntityFactory(EntityFactory& factory);
    LoggingServer(EntityFactory& factory, bool is_temp_node);
};

#endif // LOGGINGSERVER_H
