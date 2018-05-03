#include "serverrequest.h"
#include "../containernode.h"
const NODE_KIND node_kind = NODE_KIND::SERVER_REQUEST;
const QString kind_string = "ServerRequest";

MEDEA::ServerRequest::ServerRequest(EntityFactory* factory) : Node(factory, node_kind, kind_string){
    //Allow reordering
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "index", QVariant::Int, false);
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new ServerRequest();});
};

MEDEA::ServerRequest::ServerRequest(): Node(node_kind)
{
    addImplsDefinitionKind(NODE_KIND::CLIENT_PORT);
    setNodeType(NODE_TYPE::BEHAVIOUR_ELEMENT);

    //Allow links from within things like InEventPortImpls back to the
    //SetEdgeRuleActive(EdgeRule::MIRROR_PARENT_DEFINITION_HIERARCHY, false);
    SetEdgeRuleActive(Node::EdgeRule::ALWAYS_CHECK_VALID_DEFINITIONS, true);

    setAcceptsNodeKind(NODE_KIND::INPUT_PARAMETER_GROUP_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::RETURN_PARAMETER_GROUP_INSTANCE);
}


bool MEDEA::ServerRequest::canAdoptChild(Node* child)
{
   auto child_node_kind = child->getNodeKind();

    switch(child_node_kind){
        case NODE_KIND::INPUT_PARAMETER_GROUP_INSTANCE:
        case NODE_KIND::RETURN_PARAMETER_GROUP_INSTANCE:{
            if(!getChildrenOfKind(child->getNodeKind(), 0).isEmpty()){
                return false;
            }
            break;
        }
        default:
            break;
    }
    return Node::canAdoptChild(child);
}

bool MEDEA::ServerRequest::canAcceptEdge(EDGE_KIND edge_kind, Node * dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
        return false;
    }

    switch(edge_kind){
    case EDGE_KIND::DEFINITION:{
        //Can only connect a definition edge to an Aggregate/AggregateInstance..
        if(dst->getNodeKind() != NODE_KIND::CLIENT_PORT){
            return false;
        }
        break;
    }
    default:
        break;
    }
    return Node::canAcceptEdge(edge_kind, dst);
}

QSet<Node*> MEDEA::ServerRequest::getParentNodesForValidDefinition(){
    QSet<Node*> parents;
    //Need to look at The Component's defintion
    auto component = getTopBehaviourContainer();
    if(component){
        auto component_definition = component->getDefinition(true);
        if(component_definition){
            parents << component_definition;
        }
    }
    return parents;
}

Node* MEDEA::ServerRequest::getTopBehaviourContainer(){
    if(!top_behaviour_calculated){
        top_behaviour_container = ContainerNode::getTopBehaviourContainer(this);
        top_behaviour_calculated = true;
    }
    return top_behaviour_container;
}