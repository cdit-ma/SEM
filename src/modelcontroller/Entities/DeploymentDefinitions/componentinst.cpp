#include "componentinst.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::COMPONENT_INST;
const QString kind_string = "Component Instance";

void ComponentInst::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new ComponentInst(broker, is_temp_node);
        });
}

ComponentInst::ComponentInst(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    addInstancesDefinitionKind(NODE_KIND::COMPONENT);

    setAcceptsEdgeKind(EDGE_KIND::QOS, EDGE_DIRECTION::SOURCE);
    setAcceptsEdgeKind(EDGE_KIND::DEPLOYMENT, EDGE_DIRECTION::SOURCE);

    setAcceptsNodeKind(NODE_KIND::ATTRIBUTE_INST);
    setAcceptsNodeKind(NODE_KIND::PORT_SUBSCRIBER_INST);
    setAcceptsNodeKind(NODE_KIND::PORT_PUBLISHER_INST);
    setAcceptsNodeKind(NODE_KIND::CLASS_INST);
    setAcceptsNodeKind(NODE_KIND::PORT_PERIODIC_INST);
    setAcceptsNodeKind(NODE_KIND::PORT_REPLIER_INST);
    setAcceptsNodeKind(NODE_KIND::PORT_REQUESTER_INST);
    setAcceptsNodeKind(NODE_KIND::TRIGGER_INST);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }
    
    //Setup Data
    broker.AttachData(this, "type", QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, "comment", QVariant::String, ProtectedState::UNPROTECTED);
}

QSet<Node*> ComponentInst::getListOfValidAncestorsForChildrenDefinitions(){
    QSet<Node*> valid_ancestors = Node::getListOfValidAncestorsForChildrenDefinitions();
    
    //We can create definition edges back to the Definition, and to things contained within our Implementation.
    auto definition = getDefinition(true);
    if(definition){
        for(auto impl : definition->getImplementations()){
            valid_ancestors << impl;
        }
    }

    return valid_ancestors;
}

