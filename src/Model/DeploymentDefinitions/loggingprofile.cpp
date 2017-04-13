#include "loggingprofile.h"
#include "../key.h"
#include "../data.h"
LoggingProfile::LoggingProfile():Node(NK_LOGGINGPROFILE)
{
    setAcceptsEdgeKind(Edge::EC_DEPLOYMENT);
}

QList<Data *> LoggingProfile::getDefaultData()
{
    auto data_list = Node::getDefaultData();
    {
        //Append extra info
        auto key = Key::GetKey("processes_to_log", QVariant::String);
        data_list.append(new Data(key, ""));
    }
    return data_list;
}

bool LoggingProfile::canAdoptChild(Node*)
{
    return false;
}

bool LoggingProfile::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    return Node::canAcceptEdge(edgeKind, dst);
}
