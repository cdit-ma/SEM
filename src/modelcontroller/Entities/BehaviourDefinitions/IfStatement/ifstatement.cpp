#include "ifstatement.h"
#include "../../../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::IF_STATEMENT;
const QString kind_string = "IfStatement";

void MEDEA::IfStatement::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new MEDEA::ElseIfCondition(factory, is_temp_node);
        });
}

MEDEA::IfStatement::IfStatement(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //SetupState
    setNodeType(NODE_TYPE::BEHAVIOUR_ELEMENT);
    setAcceptsNodeKind(NODE_KIND::IF_CONDITION);
    setAcceptsNodeKind(NODE_KIND::ELSE_CONDITION);
    setAcceptsNodeKind(NODE_KIND::ELSEIF_CONDITION);

    setLabelFunctional(false);
}


bool MEDEA::IfStatement::canAdoptChild(Node *child)
{
    auto child_kind = child->getNodeKind();
    switch(child_kind){
        case NODE_KIND::IF_CONDITION:
        case NODE_KIND::ELSE_CONDITION:
            if(getChildrenOfKindCount(child_kind) > 0){
                return false;
            }
            break;
        default:
            break;
    }
    
    return Node::canAdoptChild(child);
}