#include "attribute.h"

Attribute::Attribute():Node(NODE_KIND::ATTRIBUTE)
{
    setNodeType(NODE_TYPE::DEFINITION);

    
    updateDefaultData("type", QVariant::String, false, "String");
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
    
}

bool Attribute::canAdoptChild(Node*)
{
    return false;
}

bool Attribute::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}