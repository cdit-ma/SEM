#include "indexkey.h"
#include "../data.h"
#include "../node.h"
#include <QDebug>
IndexKey::IndexKey(): Key("index", QVariant::Int){

}   

QVariant IndexKey::validateDataChange(Data* data, QVariant data_value){    
    
    int new_index = data_value.toInt();
    Node* node = 0;

    //Get the node this data is attached to
    auto entity = data->getParent();
    if(entity && entity->isNode()){
        node = (Node*) entity;
    }
    
    //If its not attached to a node, the value is invalid
    if(!node || !node->getParentNode()){
        return new_index;
    }

    //Siblings are already in Index'd order
    auto siblings = node->getSiblings();
    int old_index = data->getValue().toInt();
    int max_index = node->getSiblings().count();

    //Don't set Below 0
    if(new_index < -1){
        new_index = 0;
    }
    
    //If the new_index is -1 or is bigger than the max_index, put it last
    if(new_index == -1 || new_index > max_index){
        new_index = max_index;
    }

    //Insert the node into the correct position
    siblings.insert(new_index, node);

    for(int i = 0; i < siblings.count(); i++){
        auto sibling = siblings[i];
        auto data = sibling->getData("index");
        if(data){
            //Get the index the sibling node
            int current_index = data->getValue().toInt();
            if(current_index != i && sibling != node){
                //Force the value as to not end up in recursion
                forceDataValue(data, i);
            }
        }
    }
    //Return the index
    return new_index;
}