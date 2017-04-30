#include "loggingserver.h"

LoggingServer::LoggingServer():Node(NODE_KIND::LOGGINGSERVER)
{
    setNodeType(NODE_TYPE::LOGGING);
    setAcceptsEdgeKind(Edge::EC_ASSEMBLY);
    setAcceptsEdgeKind(Edge::EC_DEPLOYMENT);

    updateDefaultData("database", QVariant::String, "output.sql");
}


bool LoggingServer::canAdoptChild(Node*)
{
    return false;
}

bool LoggingServer::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    switch(edgeKind){
    case Edge::EC_ASSEMBLY:{
        //Don't allow assembly edges
        return false;
    }
    default:
        break;
    }


    return Node::canAcceptEdge(edgeKind, dst);
}
