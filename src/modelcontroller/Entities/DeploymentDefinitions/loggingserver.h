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
public:
    bool canAdoptChild(Node*);
};

#endif // LOGGINGSERVER_H
