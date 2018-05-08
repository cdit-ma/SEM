#include "header.h"
#include "../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::HEADER;
const QString kind_string = "Header";

void Header::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new Header(factory, is_temp_node);
        });
}

Header::Header(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup Data
    factory.AttachData(this, "code", QVariant::String, "", false);
    auto header_location = factory.AttachData(this, "header_location", QVariant::String, "CPP", false);
    header_location->setValidValues({"Class Declaration", "Header", "CPP"});
}