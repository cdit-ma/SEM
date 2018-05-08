#include "setter.h"
#include "../Keys/typekey.h"

const NODE_KIND node_kind = NODE_KIND::SETTER;
const QString kind_string = "Setter";

void Setter::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new Setter(factory, is_temp_node);
        });
}

Setter::Setter(EntityFactory& factory, bool is_temp) : Node(node_kind, is_temp){
    if(is_temp){
        return;
    }

    setNodeType(NODE_TYPE::BEHAVIOUR_ELEMENT);
    setAcceptsNodeKind(NODE_KIND::INPUT_PARAMETER);

    //TODO: Add CUstom Constructors
    //QList<QVariant> operator_types = {"=", "+=", "-=", "*=", "/="};
}

bool Setter::canAdoptChild(Node* child)
{
    auto child_kind = child->getNodeKind();
    switch(child_kind){
    case NODE_KIND::INPUT_PARAMETER:
        if(getChildrenOfKindCount(child_kind) >= 2){
                return false;
            }
        break;
    default:
        break;
    }

    return Node::canAdoptChild(child);
}