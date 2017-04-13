#include "loggingserver.h"
#include "../key.h"
#include "../data.h"
LoggingServer::LoggingServer():Node(NK_LOGGINGSERVER)
{
    setNodeType(NT_LOGGING);
    setAcceptsEdgeKind(Edge::EC_ASSEMBLY);
    setAcceptsEdgeKind(Edge::EC_DEPLOYMENT);
}

QList<Data *> LoggingServer::getDefaultData()
{
    auto data_list = Node::getDefaultData();
    {
        //Process
        auto key = Key::GetKey("database", QVariant::String);
        data_list.append(new Data(key, "output.sql"));
    }
    return data_list;
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
