#include "setter.h"
#include "../Keys/typekey.h"


const NODE_KIND node_kind = NODE_KIND::SETTER;
const QString kind_string = "Setter";


Setter::Setter(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Setter();});


    //Register DefaultData
    RegisterDefaultData(factory, node_kind, "operator", QVariant::String, false, "=");
    QList<QVariant> operator_types = {"=", "+=", "-=", "*=", "/="};

    RegisterValidDataValues(factory, node_kind, "operator", QVariant::String, operator_types);
};

Setter::Setter() : Node(node_kind)
{
    setAcceptsNodeKind(NODE_KIND::INPUT_PARAMETER);
    setNodeType(NODE_TYPE::BEHAVIOUR_ELEMENT);
}

bool Setter::canAdoptChild(Node* child)
{
    auto node_kind = child->getNodeKind();
    switch(node_kind){
    case NODE_KIND::INPUT_PARAMETER:
        if(getChildrenOfKind(node_kind, 0).size() >= 2){
            return false;
        }
        break;
    default:
        break;
    }

    return Node::canAdoptChild(child);
}

void Setter::childAdded(Node* child){
    Node::childAdded(child);

    auto input_params = getChildrenOfKind(NODE_KIND::INPUT_PARAMETER);

    if(input_params.length() == 2){
        //Bind types
        TypeKey::BindInnerAndOuterTypes(input_params[0], input_params[1], true);
    }
}