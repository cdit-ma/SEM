#include "ifstatement.h"

const NODE_KIND node_kind = NODE_KIND::IF_STATEMENT;
const QString kind_string = "IfStatement";


MEDEA::IfStatement::IfStatement(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new IfStatement();});
};

MEDEA::IfStatement::IfStatement() : Node(node_kind){
    setNodeType(NODE_TYPE::BEHAVIOUR_ELEMENT);
    setAcceptsNodeKind(NODE_KIND::IF_CONDITION);
    setAcceptsNodeKind(NODE_KIND::ELSE_CONDITION);
    setAcceptsNodeKind(NODE_KIND::ELSEIF_CONDITION);
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

bool MEDEA::IfStatement::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    return Node::canAcceptEdge(edgeKind, dst);
}
