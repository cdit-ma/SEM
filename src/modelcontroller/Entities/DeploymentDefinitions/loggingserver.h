#ifndef LOGGINGSERVER_H
#define LOGGINGSERVER_H
#include "../node.h"

class EntityFactory;
class LoggingServer : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	LoggingServer(EntityFactory* factory);
	LoggingServer();
};

#endif // LOGGINGSERVER_H
