#include "loggingprofile.h"

LoggingProfile::LoggingProfile():Node(NODE_KIND::LOGGINGPROFILE)
{
    setNodeType(NODE_TYPE::LOGGING);
    setAcceptsEdgeKind(Edge::EC_DEPLOYMENT);
    setAcceptsEdgeKind(Edge::EC_ASSEMBLY);

    updateDefaultData("processes_to_log", QVariant::String);
    updateDefaultData("mode", QVariant::String, false, "CACHED");
    updateDefaultData("frequency", QVariant::Double, false, 1);
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
