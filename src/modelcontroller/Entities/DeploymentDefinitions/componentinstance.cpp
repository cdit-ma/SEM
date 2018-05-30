#include "componentinstance.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::COMPONENT_INSTANCE;
const QString kind_string = "Component Instance";

void ComponentInstance::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new ComponentInstance(broker, is_temp_node);
        });
}

ComponentInstance::ComponentInstance(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    addInstancesDefinitionKind(NODE_KIND::COMPONENT);

    setAcceptsEdgeKind(EDGE_KIND::QOS, EDGE_DIRECTION::SOURCE);
    setAcceptsEdgeKind(EDGE_KIND::DEPLOYMENT, EDGE_DIRECTION::SOURCE);

    setAcceptsNodeKind(NODE_KIND::ATTRIBUTE_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::INEVENTPORT_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::OUTEVENTPORT_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::CLASS_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::PERIODICEVENT_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::SERVER_PORT_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::CLIENT_PORT_INSTANCE);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }
    
    //Setup Data
    broker.AttachData(this, "type", QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, "comment", QVariant::String, ProtectedState::UNPROTECTED);
}

QSet<Node*> ComponentInstance::getListOfValidAncestorsForChildrenDefinitions(){
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

