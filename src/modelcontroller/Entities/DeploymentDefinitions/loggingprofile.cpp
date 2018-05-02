#include "loggingprofile.h"


LoggingProfile::LoggingProfile(EntityFactory* factory) : Node(factory, NODE_KIND::LOGGINGPROFILE, "LoggingProfile"){
	auto node_kind = NODE_KIND::LOGGINGPROFILE;
	QString kind_string = "LoggingProfile";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new LoggingProfile();});

    QList<QVariant> types;
    types << "CACHED";
    types << "OFF";
    types << "LIVE";

    RegisterValidDataValues(factory, node_kind, "mode", QVariant::String, types);

    RegisterDefaultData(factory, node_kind, "processes_to_log", QVariant::String);
    RegisterDefaultData(factory, node_kind, "mode", QVariant::String, false, types.first());
    RegisterDefaultData(factory, node_kind, "frequency", QVariant::Double, false, 1);
};

LoggingProfile::LoggingProfile():Node(NODE_KIND::LOGGINGPROFILE)
{
    setNodeType(NODE_TYPE::LOGGING);
    setAcceptsEdgeKind(EDGE_KIND::DEPLOYMENT, EDGE_DIRECTION::SOURCE);
    setAcceptsEdgeKind(EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::SOURCE);
}

bool LoggingProfile::canAcceptEdge(EDGE_KIND edge_kind, Node *dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
        return false;
    }
    switch(edge_kind){
    case EDGE_KIND::ASSEMBLY:{
        //Can't connect to something that isn't an EventPortAssembly
        if(dst->getNodeKind() != NODE_KIND::LOGGINGSERVER){
            return false;
        }
        break;
    }
    default:
        break;
    }

    return Node::canAcceptEdge(edge_kind, dst);
}
