#include "columnkey.h"
#include "../data.h"
#include "../node.h"
#include <QDebug>

ColumnKey::ColumnKey(EntityFactoryBroker& broker): Key(broker, KeyName::Column, QVariant::Int){

}

QVariant ColumnKey::validateDataChange(Data* data, QVariant data_value){
    int new_row = data_value.toInt();
    return new_row;
}

bool ColumnKey::setData(Data* data, QVariant data_value){
    bool data_changed = Key::setData(data, data_value);
    auto entity = data->getParent();

    //If our row has been changed, we should update the index such that it ends up last in the column it's been moved to
    if(entity && entity->isNode()){
        auto node = (Node*) entity;
        auto node_index = node->getData(KeyName::Index);
        if(node_index){
            node_index->setValue(-1);
        }
    }
    return data_changed;
}