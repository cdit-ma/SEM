#include "ifstatement.h"

const NODE_KIND node_kind = NODE_KIND::IF_STATEMENT;
const QString kind_string = "IfStatement";


MEDEA::IfStatement::IfStatement(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new IfStatement();});
};

MEDEA::IfStatement::IfStatement() : Node(node_kind){
}


bool MEDEA::IfStatement::canAdoptChild(Node *child)
{
    auto child_kind = child->getNodeKind();
    switch(child_kind){
        case NODE_KIND::IF_CONDITION:
        case NODE_KIND::ELSE_CONDITION:
            if(getChildrenOfKind(child_kind, 0).size()){
                return false;
            }
            break;
        case NODE_KIND::ELSEIF_CONDITION:{
            break;
        }
        default:
            return false;
    }
    
    return Node::canAdoptChild(child);
}

bool MEDEA::IfStatement::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    return Node::canAcceptEdge(edgeKind, dst);
}
