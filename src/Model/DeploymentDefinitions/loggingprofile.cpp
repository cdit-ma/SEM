#include "loggingprofile.h"
#include "../key.h"
#include "../data.h"
#include <QDebug>
LoggingProfile::LoggingProfile():Node(NK_LOGGINGPROFILE)
{
    setNodeType(NT_LOGGING);
    setAcceptsEdgeKind(Edge::EC_DEPLOYMENT);
    setAcceptsEdgeKind(Edge::EC_ASSEMBLY);
}

QList<Data *> LoggingProfile::getDefaultData()
{
    QString kind_str = getNodeKindStr();

    auto data_list = Node::getDefaultData();
    {
        //Process
        auto key = Key::GetKey("processes_to_log", QVariant::String);
        data_list.append(new Data(key, ""));
    }
    {
        //Frequency
        auto key = Key::GetKey("frequency", QVariant::Double);
        data_list.append(new Data(key, 1.0));
    }
    {
        //Live Mode
        auto key = Key::GetKey("mode", QVariant::String);
        //Setting Mode
        if(!key->gotValidValues(kind_str)){
            QStringList keysValues = QStringList() << kind_str;
            QStringList validValues = QStringList() << "CACHED" << "OFF" << "LIVE";
            key->addValidValues(validValues, keysValues);
        }

        data_list.append(new Data(key, "CACHED"));
    }
    return data_list;
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
        if(dst->getNodeKind() != Node::NK_LOGGINGSERVER){
            return false;
        }
        break;
    }
    }

    return Node::canAcceptEdge(edgeKind, dst);
}
