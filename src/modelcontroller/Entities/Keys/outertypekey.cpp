#include "outertypekey.h"
#include "../data.h"
#include "../node.h"
#include <QDebug>

OuterTypeKey::OuterTypeKey(EntityFactoryBroker& broker): Key(broker, "outer_type", QVariant::String){

}

bool OuterTypeKey::setData(Data* data, QVariant data_value){
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