#include "loggingserver.h"


LoggingServer::LoggingServer(EntityFactory* factory) : Node(factory, NODE_KIND::LOGGINGSERVER, "LoggingServer"){
	auto node_kind = NODE_KIND::LOGGINGSERVER;
	QString kind_string = "LoggingServer";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new LoggingServer();});

    RegisterDefaultData(factory, node_kind, "database", QVariant::String, false, "output.sql");
};

LoggingServer::LoggingServer():Node(NODE_KIND::LOGGINGSERVER)
{
    setNodeType(NODE_TYPE::LOGGING);
    setAcceptsEdgeKind(EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::TARGET);
    setAcceptsEdgeKind(EDGE_KIND::DEPLOYMENT, EDGE_DIRECTION::SOURCE);
}


bool LoggingServer::canAdoptChild(Node*)
{
    return false;
}