#ifndef LOGGINGPROFILE_H
#define LOGGINGPROFILE_H
#include "../node.h"

class LoggingProfile : public Node
{
    Q_OBJECT
public:
    LoggingProfile();

    bool canAdoptChild(Node*);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // LOGGINGPROFILE_H
