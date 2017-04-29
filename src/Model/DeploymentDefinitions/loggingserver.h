#ifndef LOGGINGSERVER_H
#define LOGGINGSERVER_H
#include "../node.h"

class LoggingServer : public Node
{
    Q_OBJECT
public:
    LoggingServer();
    bool canAdoptChild(Node*);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // LOGGINGSERVER_H
