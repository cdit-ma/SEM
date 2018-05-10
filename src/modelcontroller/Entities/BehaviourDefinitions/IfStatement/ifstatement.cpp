#include "ifstatement.h"
#include "../../../entityfactorybroker.h"
#include "../../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::IF_STATEMENT;
const QString kind_string = "IfStatement";

void MEDEA::IfStatement::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::IfStatement(broker, is_temp_node);
        });
}

MEDEA::IfStatement::IfStatement(::EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
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