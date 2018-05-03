#include "componentinstance.h"

#include "../../edgekinds.h"

ComponentInstance::ComponentInstance(EntityFactory* factory) : Node(factory, NODE_KIND::COMPONENT_INSTANCE, "ComponentInstance"){
	auto node_kind = NODE_KIND::COMPONENT_INSTANCE;
	QString kind_string = "ComponentInstance";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new ComponentInstance();});

    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "comment", QVariant::String);
};

ComponentInstance::ComponentInstance():Node(NODE_KIND::COMPONENT_INSTANCE)
{
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

