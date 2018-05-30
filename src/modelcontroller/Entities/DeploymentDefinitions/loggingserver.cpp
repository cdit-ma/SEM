#include "loggingserver.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::LOGGINGSERVER;
const QString kind_string = "Logging Server";

void LoggingServer::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new LoggingServer(broker, is_temp_node);
    });
}

LoggingServer::LoggingServer(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::LOGGING);
    setAcceptsEdgeKind(EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::TARGET);
    setAcceptsEdgeKind(EDGE_KIND::DEPLOYMENT, EDGE_DIRECTION::SOURCE);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }
    
    broker.AttachData(this, "database", QVariant::String, ProtectedState::UNPROTECTED,  "output.sql");
}