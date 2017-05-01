#include "loggingserver.h"
#include "../entityfactory.h"

LoggingServer::LoggingServer(EntityFactory* factory) : Node(factory, NODE_KIND::LOGGINGSERVER, "LoggingServer"){
	auto node_kind = NODE_KIND::LOGGINGSERVER;
	QString kind_string = "LoggingServer";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new LoggingServer();});

    RegisterDefaultData(factory, node_kind, "database", QVariant::String, false, "output.sql");
};

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
