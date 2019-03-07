#include "componentimpl.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::COMPONENT_IMPL;
const QString kind_string = "Component Impl";

void ComponentImpl::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new ComponentImpl(broker, is_temp_node);
        });
}

ComponentImpl::ComponentImpl(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    addImplsDefinitionKind(NODE_KIND::COMPONENT);
    setNodeType(NODE_TYPE::TOP_BEHAVIOUR_CONTAINER);
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);
    

    setAcceptsNodeKind(NODE_KIND::ATTRIBUTE_IMPL);
    setAcceptsNodeKind(NODE_KIND::PORT_SUBSCRIBER_IMPL);
    setAcceptsNodeKind(NODE_KIND::PORT_PERIODIC);
    setAcceptsNodeKind(NODE_KIND::VARIABLE);
    setAcceptsNodeKind(NODE_KIND::HEADER);
    setAcceptsNodeKind(NODE_KIND::FUNCTION);
    setAcceptsNodeKind(NODE_KIND::CLASS_INST);
    setAcceptsNodeKind(NODE_KIND::PORT_REPLIER_IMPL);
    setAcceptsNodeKind(NODE_KIND::TRANSITION_FUNCTION);
    setAcceptsNodeKind(NODE_KIND::CALLBACK_FNC_INST);
    
    

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    setLabelFunctional(false);

    //Setup Data
    broker.AttachData(this, "type", QVariant::String, ProtectedState::PROTECTED);
}

QSet<Node*> ComponentImpl::getDependants() const{
    auto required_nodes = Node::getDependants();
    auto definition = getDefinition(true);
    if(definition){
        for(auto d : definition->getInstances()){
            required_nodes.insert(d);
        }
    }
    return required_nodes;
}
