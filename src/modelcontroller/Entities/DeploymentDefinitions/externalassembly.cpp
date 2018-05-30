#include "externalassembly.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::EXTERNAL_ASSEMBLY;
const QString kind_string = "External Assembly";

void MEDEA::ExternalAssembly::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new ExternalAssembly(broker, is_temp_node);
        });
}

MEDEA::ExternalAssembly::ExternalAssembly(EntityFactoryBroker& broker, bool is_temp) : EventPortAssembly(broker, node_kind, is_temp){
    //Setup State
    setAcceptsNodeKind(NODE_KIND::INEVENTPORT_DELEGATE);
    setAcceptsNodeKind(NODE_KIND::OUTEVENTPORT_DELEGATE);
    
    setAcceptsEdgeKind(EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::SOURCE, false);
    setAcceptsEdgeKind(EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::TARGET, false);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, "comment", QVariant::String, ProtectedState::UNPROTECTED);

    auto data_middleware = broker.AttachData(this, "middleware", QVariant::String, ProtectedState::UNPROTECTED);
    data_middleware->addValidValues({"ZMQ", "RTI", "OSPL", "TAO"});

    in_ = broker.ConstructChildNode(*this, NODE_KIND::INEVENTPORT_DELEGATE);
    out_ = broker.ConstructChildNode(*this, NODE_KIND::OUTEVENTPORT_DELEGATE);

    //Disable Edge kinds
    broker.SetAcceptsEdgeKind(in_, EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::SOURCE, false);
    broker.SetAcceptsEdgeKind(out_, EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::TARGET, false);

    broker.AttachData(in_, "label", QVariant::String, ProtectedState::PROTECTED, "IN");
    broker.AttachData(in_, "middleware", QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(out_, "label", QVariant::String, ProtectedState::PROTECTED,  "OUT");
    broker.AttachData(out_, "middleware", QVariant::String, ProtectedState::PROTECTED);

    LinkData(this, "middleware", in_, "middleware", true);
    LinkData(this, "middleware", out_, "middleware", true);

    connect(data_middleware, &Data::dataChanged, this, &MEDEA::ExternalAssembly::MiddlewareUpdated);
    MiddlewareUpdated();
}


bool MEDEA::ExternalAssembly::canAdoptChild(Node *node)
{
    auto node_kind = node->getNodeKind();
    if(getChildrenOfKindCount(node_kind) > 0){
        return false;
    }
    return Node::canAdoptChild(node);
}

bool MEDEA::ExternalAssembly::canAcceptEdge(EDGE_KIND edge_kind, Node *dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
        return false;
    }

    
    switch(edge_kind){
    case EDGE_KIND::AGGREGATE:{
        bool in_shared_datatype = false;

        for(auto parent : dst->getParentNodes()){
            if(parent->getNodeKind() == NODE_KIND::SHARED_DATATYPES){
                in_shared_datatype = true;
                break;
            }
        }

        if(!in_shared_datatype){
            return false;
        }
        break;
    }
    default:
        break;
    }

    return EventPortAssembly::canAcceptEdge(edge_kind, dst);;
}

void MEDEA::ExternalAssembly::MiddlewareUpdated(){
    const auto& middleware = getDataValue("middleware").toString();
    QSet<QString> topic_middlewares = {"RTI", "OSPL", "AMQP"};

    auto topic_key = getFactoryBroker().GetKey("topic_name", QVariant::String);

    if(!topic_middlewares.contains(middleware)){
        getFactoryBroker().RemoveData(this, "topic_name");
        getFactoryBroker().RemoveData(in_, "topic_name");
        getFactoryBroker().RemoveData(out_, "topic_name");
    }else{
        getFactoryBroker().AttachData(this, topic_key, ProtectedState::UNPROTECTED);
        getFactoryBroker().AttachData(in_, topic_key, ProtectedState::PROTECTED);
        getFactoryBroker().AttachData(out_, topic_key, ProtectedState::PROTECTED);

        LinkData(this, "topic_name", in_, "topic_name", true);
        LinkData(this, "topic_name", out_, "topic_name", true);
    }
}