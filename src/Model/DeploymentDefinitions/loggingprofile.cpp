#include "loggingprofile.h"

#include "../entityfactory.h"
LoggingProfile::LoggingProfile(EntityFactory* factory) : Node(factory, NODE_KIND::LOGGINGPROFILE, "LoggingProfile"){
	auto node_kind = NODE_KIND::LOGGINGPROFILE;
	QString kind_string = "LoggingProfile";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new LoggingProfile();});

    RegisterDefaultData(factory, node_kind, "processes_to_log", QVariant::String);
    RegisterDefaultData(factory, node_kind, "mode", QVariant::String);
    RegisterDefaultData(factory, node_kind, "frequency", QVariant::Double);
};

LoggingProfile::LoggingProfile():Node(NODE_KIND::LOGGINGPROFILE)
{
    setNodeType(NODE_TYPE::LOGGING);
    setAcceptsEdgeKind(Edge::EC_DEPLOYMENT);
    setAcceptsEdgeKind(Edge::EC_ASSEMBLY);
}

bool LoggingProfile::canAdoptChild(Node*)
{
    return false;
}

bool LoggingProfile::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    switch(edgeKind){
    case Edge::EC_ASSEMBLY:{
        //Can't connect to something that isn't an EventPortAssembly
        if(dst->getNodeKind() != NODE_KIND::LOGGINGSERVER){
            return false;
        }
        break;
    }
    default:
        break;
    }

    return Node::canAcceptEdge(edgeKind, dst);
}
