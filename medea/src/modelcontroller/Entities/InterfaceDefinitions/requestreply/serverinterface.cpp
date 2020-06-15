#include "serverinterface.h"
#include "../../../entityfactorybroker.h"
#include "../../../entityfactoryregistrybroker.h"
#include "../../Keys/typekey.h"

const NODE_KIND node_kind = NODE_KIND::SERVER_INTERFACE;
const QString kind_string = "Server Interface";

void MEDEA::ServerInterface::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
	broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::ServerInterface(broker, is_temp_node);
        });
};


MEDEA::ServerInterface::ServerInterface(::EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setAcceptsNodeKind(NODE_KIND::INPUT_PARAMETER_GROUP);
    setAcceptsNodeKind(NODE_KIND::RETURN_PARAMETER_GROUP);
    setAcceptsEdgeKind(EDGE_KIND::AGGREGATE, EDGE_DIRECTION::TARGET);
    setLabelFunctional(false);
    
    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Children
    auto input_params = broker.ConstructChildNode(*this, NODE_KIND::INPUT_PARAMETER_GROUP);
    auto return_params = broker.ConstructChildNode(*this, NODE_KIND::RETURN_PARAMETER_GROUP);

    broker.AttachData(this, KeyName::Namespace, QVariant::String, ProtectedState::PROTECTED);
    auto data_function_name = broker.AttachData(this, KeyName::FunctionName, QVariant::String, ProtectedState::UNPROTECTED, "send");
    auto data_interface_name = broker.AttachData(this, KeyName::InterfaceName, QVariant::String, ProtectedState::UNPROTECTED, "");
    broker.AttachData(this, KeyName::Label, QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, KeyName::Type, QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(input_params, KeyName::Label, QVariant::String, ProtectedState::PROTECTED, "Request Type");
    broker.AttachData(return_params, KeyName::Label, QVariant::String, ProtectedState::PROTECTED, "Reply Type");
    TypeKey::BindNamespaceAndLabelToType(this, true);

    connect(data_function_name, &Data::dataChanged, this, &MEDEA::ServerInterface::updateLabel);
    connect(data_interface_name, &Data::dataChanged, this, &MEDEA::ServerInterface::updateLabel);
}


bool MEDEA::ServerInterface::canAdoptChild(Node* child)
{
    auto child_kind = child->getNodeKind();
    switch(child_kind){
    case NODE_KIND::INPUT_PARAMETER_GROUP:
    case NODE_KIND::RETURN_PARAMETER_GROUP:{
        if(getChildrenOfKindCount(child_kind) > 0){
        return false;
    }
        break;
    }
    default:
        return false;
    };
    return Node::canAdoptChild(child);
}


void MEDEA::ServerInterface::updateLabel()
{
    QString new_label;
    auto interface_name = getDataValue(KeyName::InterfaceName).toString();
    auto function_name = getDataValue(KeyName::FunctionName).toString();

    if (interface_name.size()) {
        new_label += interface_name + "_";
    }
    
    new_label += function_name;
    setDataValue(KeyName::Label, new_label);
}