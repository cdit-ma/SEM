#include "enummember.h"
#include "enuminstance.h"
#include <QDebug>
#include "../../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::ENUM_MEMBER;
const QString kind_string = "EnumMember";

void EnumMember::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new EnumMember(factory, is_temp_node);
    });
}

EnumMember::EnumMember(EntityFactory& factory, bool is_temp) : DataNode(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setPromiscuousDataLinker(true);
    setDataProducer(true);

    //Setup Data
    factory.AttachData(this, "index", QVariant::Int, -1, true);
    factory.AttachData(this, "type", QVariant::String, "", true);
}