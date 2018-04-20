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
    setNodeType(NODE_TYPE::INSTANCE);
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);
    setAcceptsEdgeKind(EDGE_KIND::DEPLOYMENT);
    setAcceptsEdgeKind(EDGE_KIND::QOS);

    setDefinitionKind(NODE_KIND::COMPONENT);
}

bool ComponentInstance::canAdoptChild(Node *child)
{

    switch(child->getNodeKind()){
    case NODE_KIND::ATTRIBUTE_INSTANCE:
    case NODE_KIND::INEVENTPORT_INSTANCE:
    case NODE_KIND::OUTEVENTPORT_INSTANCE:
    case NODE_KIND::CLASS_INSTANCE:
    case NODE_KIND::PERIODICEVENT:
        break;
    default:
        return false;
    }
    return Node::canAdoptChild(child);
}

bool ComponentInstance::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }

    switch(edgeKind){
    case EDGE_KIND::DEFINITION:{
        if(dst->getNodeKind() != NODE_KIND::COMPONENT){
            return false;
        }
        break;
    }
    case EDGE_KIND::DEPLOYMENT:{
        if(gotEdgeKind(edgeKind)){
            return false;
        }
        break;
    }
    default:
        break;
    }
    return Node::canAcceptEdge(edgeKind, dst);

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