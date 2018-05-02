#include "functioncall.h"
#include <QDebug>
#include "parameter.h"


const NODE_KIND node_kind = NODE_KIND::FUNCTION_CALL;
const QString kind_string = "FunctionCall";

FunctionCall::FunctionCall():Node(node_kind)
{
    addInstancesDefinitionKind(NODE_KIND::FUNCTION);

    //Disable rules which break
    SetEdgeRuleActive(EdgeRule::REQUIRE_NO_DEFINITION, false);
    SetEdgeRuleActive(EdgeRule::MIRROR_PARENT_DEFINITION_HIERARCHY, false);
}

FunctionCall::FunctionCall(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new FunctionCall();});

    //Register DefaultData
    RegisterDefaultData(factory, node_kind, "icon", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "icon_prefix", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "class", QVariant::String, true);

    setAcceptsNodeKind(NODE_KIND::INPUT_PARAMETER_GROUP_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::RETURN_PARAMETER_GROUP_INSTANCE);
}


bool FunctionCall::canAcceptEdge(EDGE_KIND edge_kind, Node *dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
        return false;
    }

    switch(edge_kind){
        case EDGE_KIND::DEFINITION:{
            // Definition edge must link to a WorkerFunction
            if(dst->getNodeKind() != NODE_KIND::FUNCTION){
                return false;
            }
            
            // The FunctionCall must exist within a ComponentImpl
            auto parent_node = dst->getParentNode();
            if(parent_node){
                auto parent_node_kind = parent_node->getNodeKind();

                QSet<NODE_KIND> valid_parent_kinds = {NODE_KIND::COMPONENT_IMPL, NODE_KIND::CLASS_INSTANCE};

                if(!valid_parent_kinds.contains(parent_node_kind)){
                    return false;
                }

                auto comm_anc = getCommonAncestor(parent_node);
                auto common_ancestor_kind = comm_anc ? comm_anc->getNodeKind() : NODE_KIND::NONE;
                QSet<NODE_KIND> valid_ancestor_kinds = {NODE_KIND::COMPONENT_IMPL, NODE_KIND::CLASS};

                if(!valid_ancestor_kinds.contains(common_ancestor_kind)){
                    return false;
                }
            }
            
            break;
        }
        default:
            break;
    }
    return Node::canAcceptEdge(edge_kind, dst);
}

bool FunctionCall::canAdoptChild(Node* child)
{
    auto child_kind = child->getNodeKind();
    switch(child_kind){
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

QSet<Node*> FunctionCall::getListOfValidAncestorsForChildrenDefinitions(){
    QSet<Node*> valid_ancestors;
    
    //We can create definition edges back to the recursive Definition only, because of the fact the instances are sparse
    auto definition = getDefinition(true);
    if(definition){
        valid_ancestors << definition;
    }

    return valid_ancestors;
}
