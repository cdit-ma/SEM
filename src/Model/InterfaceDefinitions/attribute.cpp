#include "attribute.h"
#include "../entityfactory.h"

Attribute::Attribute(EntityFactory* factory) : Node(factory, NODE_KIND::ATTRIBUTE, "Attribute"){
	auto node_kind = NODE_KIND::ATTRIBUTE;
	QString kind_string = "Attribute";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Attribute();});

    RegisterDefaultData(factory, node_kind, "type", QVariant::String, false, "String");
    /*
    auto key = Key::GetKey("type", QVariant::String);
    if(!key->gotValidValues(kind)){
        auto kinds = QStringList();
        kinds << "Attribute";
        kinds << "Member";
        kinds << "Variable";
        kinds << "VariableParameter";

        auto values = QStringList();
        values << "String";
        values << "Boolean";
        values << "Integer";
        values << "Double";
        values << "Float";
        values << "Character";
        key->addValidValues(values, kinds);*/
};

Attribute::Attribute():Node(NODE_KIND::ATTRIBUTE)
{
    setNodeType(NODE_TYPE::DEFINITION);
}

bool Attribute::canAdoptChild(Node*)
{
    return false;
}

bool Attribute::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}