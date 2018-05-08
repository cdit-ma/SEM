#include "innertypekey.h"
#include "../data.h"
#include "../node.h"
#include <QDebug>

InnerTypeKey::InnerTypeKey(EntityFactory& factory): Key(factory, "inner_type", QVariant::String){

}

bool InnerTypeKey::setData(Data* data, QVariant data_value){
    bool data_changed = Key::setData(data, data_value);

    auto entity = data->getParent();
    if(entity && entity->isNode()){
        auto node = (Node*) entity;
        auto node_type = node->getData("type");
        if(node_type){
            node_type->setValue(node_type->getValue());
        }
    }
    return data_changed;
}