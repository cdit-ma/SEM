#include "componentimpl.h"

#include "../../edgekinds.h"



ComponentImpl::ComponentImpl(EntityFactory* factory) : Node(factory, NODE_KIND::COMPONENT_IMPL, "ComponentImpl"){
	auto node_kind = NODE_KIND::COMPONENT_IMPL;
	QString kind_string = "ComponentImpl";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new ComponentImpl();});

    //Register DefaultData
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
};

ComponentImpl::ComponentImpl():Node(NODE_KIND::COMPONENT_IMPL){
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);
    
    addImplsDefinitionKind(NODE_KIND::COMPONENT);
}

bool ComponentImpl::canAdoptChild(Node *child)
{
    switch(child->getNodeKind()){
        case NODE_KIND::ATTRIBUTE_IMPL:
        case NODE_KIND::INEVENTPORT_IMPL:
        case NODE_KIND::PERIODICEVENT:
        case NODE_KIND::VARIABLE:
        case NODE_KIND::HEADER:
        case NODE_KIND::FUNCTION:
        case NODE_KIND::CLASS_INSTANCE:
        case NODE_KIND::SERVER_PORT_IMPL:
            break;
    default:
        return false;
    }
    return Node::canAdoptChild(child);
}

bool ComponentImpl::canAcceptEdge(EDGE_KIND edge_kind, Node *dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
        return false;
    }

    switch(edge_kind){
    case EDGE_KIND::DEFINITION:{
        if(!dst->getImplementations().isEmpty()){
            return false;
        }
        if(dst->getNodeKind() != NODE_KIND::COMPONENT){
            return false;
        }
        break;
    }
    default:
        break;
    }
    return Node::canAcceptEdge(edge_kind, dst);
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
