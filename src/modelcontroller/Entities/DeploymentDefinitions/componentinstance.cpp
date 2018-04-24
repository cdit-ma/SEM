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
}

bool ComponentInstance::canAdoptChild(Node *child)
{

    switch(child->getNodeKind()){
    case NODE_KIND::ATTRIBUTE_INSTANCE:
    case NODE_KIND::INEVENTPORT_INSTANCE:
    case NODE_KIND::OUTEVENTPORT_INSTANCE:
    case NODE_KIND::CLASS_INSTANCE:
    case NODE_KIND::PERIODICEVENT:
    case NODE_KIND::SERVER_PORT_INSTANCE:
    case NODE_KIND::CLIENT_PORT_INSTANCE:
        break;
    default:
        return false;
    }
    return Node::canAdoptChild(child);
}

#include <QDebug>

QList<Node*> ComponentInstance::getAdoptableNodes(Node* definition){
    //Get the base nodes first
    QList<Node*> adoptable_nodes = Node::getAdoptableNodes(definition);

    //ComponentInstance should adopt from the definitions Implementations
    if(definition){
        for(auto impl : definition->getImplementations()){
            for(auto child : impl->getChildren(0)){
                if(child->isDefinition()){
                    adoptable_nodes << child;
                }
            }
        }
    }

    return adoptable_nodes;
}
