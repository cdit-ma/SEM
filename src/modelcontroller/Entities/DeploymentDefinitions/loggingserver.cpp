#include "loggingserver.h"
#include "../../entityfactory.h"

const static NODE_KIND node_kind = NODE_KIND::LOGGINGSERVER;
const static QString kind_string = "LoggingServer";

void LoggingServer::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new LoggingServer(factory, is_temp_node);
    });
}

LoggingServer::LoggingServer(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setNodeType(NODE_TYPE::LOGGING);
    setAcceptsEdgeKind(EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::TARGET);
    setAcceptsEdgeKind(EDGE_KIND::DEPLOYMENT, EDGE_DIRECTION::SOURCE);

    factory.AttachData(this, "database", QVariant::String, "output.sql", false);
}