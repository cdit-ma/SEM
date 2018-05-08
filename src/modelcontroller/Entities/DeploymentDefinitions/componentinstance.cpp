#include "componentinstance.h"
#include "../../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::COMPONENT_INSTANCE;
const QString kind_string = "ComponentInstance";

void ComponentInstance::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new ComponentInstance(factory, is_temp_node);
        });
}

ComponentInstance::ComponentInstance(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }
    
    //Setup State
    addInstancesDefinitionKind(NODE_KIND::COMPONENT);

    setAcceptsEdgeKind(EDGE_KIND::QOS, EDGE_DIRECTION::SOURCE);
    setAcceptsEdgeKind(EDGE_KIND::DEPLOYMENT, EDGE_DIRECTION::SOURCE);

    setAcceptsNodeKind(NODE_KIND::ATTRIBUTE_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::INEVENTPORT_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::OUTEVENTPORT_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::CLASS_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::PERIODICEVENT);
    setAcceptsNodeKind(NODE_KIND::SERVER_PORT_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::CLIENT_PORT_INSTANCE);

    //Setup Data
    factory.AttachData(this, "type", QVariant::String, "", true);
    factory.AttachData(this, "comment", QVariant::String, "", false);
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

