#include "rowkey.h"
#include "../data.h"
#include "../node.h"
#include <QDebug>
RowKey::RowKey(): Key("row", QVariant::Int){

}


QVariant RowKey::validateDataChange(Data* data, QVariant data_value){
    int new_row = data_value.toInt();
    int old_row = data->getValue().toInt();
    Node* node = 0;
    Node* parent_node = 0;

    //Get the node this data is attached to
    auto entity = data->getParent();
    if(entity && entity->isNode()){
        node = (Node*) entity;
        parent_node = node->getParentNode();
    }

     if(!node || !parent_node){
        return new_row;
    }
    
    int min_row = 0;

    //If our old index is -1 we need to push to the end of what we colide with
    if(new_row < min_row){
        new_row = min_row;
    }

    

    return new_row;
}
bool RowKey::setData(Data* data, QVariant data_value){
    bool data_changed = Key::setData(data, data_value);

    //Get the node this data is attached to
    auto entity = data->getParent();
    if(entity && entity->isNode()){
        auto node = (Node*) entity;
        auto node_index = node->getData("index");
        if(node_index){
            node_index->setValue(node_index->getValue());
        }
    }
    return data_changed;
}