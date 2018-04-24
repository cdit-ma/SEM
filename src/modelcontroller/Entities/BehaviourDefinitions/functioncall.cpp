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
    RegisterDefaultData(factory, node_kind, "file", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "folder", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "icon", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "icon_prefix", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "operation", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "worker", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "workerID", QVariant::String, false);
    RegisterDefaultData(factory, node_kind, "description", QVariant::String, true);
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
        // Should be replaced by parameter groups when they're ready
        case NODE_KIND::INPUT_PARAMETER:
        case NODE_KIND::RETURN_PARAMETER:
        case NODE_KIND::VARIABLE_PARAMETER:
            break;
        case NODE_KIND::INPUT_PARAMETER_GROUP_INSTANCE:
        case NODE_KIND::RETURN_PARAMETER_GROUP_INSTANCE:{
            if(!getChildrenOfKind(child->getNodeKind(), 0).isEmpty()){
                return false;
            }
            break;
        }
        default:
            return false;
    }
    return Node::canAdoptChild(child);
}


QList<Node*> FunctionCall::getAdoptableNodes(Node* definition){
    //The only things we need to adopt in a FunctionCall is all of the Parameters from the top level definition of the WorkerFunction
    QList<Node*> adoptable_nodes;
    if(definition){
        auto top_definition = definition->getDefinition(true);
        if(top_definition){
            for(auto child : top_definition->getChildren(0)){
                if(child->isNodeOfType(NODE_TYPE::PARAMETER)){
                    adoptable_nodes << child;
                }
            }
        }
    }
    return adoptable_nodes;
}