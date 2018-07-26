#include "indexkey.h"
#include "../data.h"
#include "../node.h"
#include <QDebug>

IndexKey::IndexKey(EntityFactoryBroker& broker): Key(broker, KeyName::Index, QVariant::Int){

}

QVariant IndexKey::validateDataChange(Data* data, QVariant data_value){
    int new_index = data_value.toInt();
    int old_index = data->getValue().toInt();
    

    Node* node = 0;
    Node* parent_node = 0;

    //Get the node this data is attached to
    auto entity = data->getParent();
    if(entity && entity->isNode()){
        node = (Node*) entity;
        parent_node = node->getParentNode();
    }
    
    if(!node || !parent_node){
        return new_index;
    }
    

    //Get all our children
    auto siblings = parent_node->getChildren(0);
    //Get the max index
    int max_index = siblings.count();
    
    
    
    //Don't set Below 0
    if(new_index < -1){
        new_index = 0;
    }
    
    //If the new_index is -1 or is bigger than the max_index, put it last
    if(new_index == -1 || new_index > max_index){
        new_index = max_index;
    }

    //If our old index is -1 we need to push to the end of what we colide with
    if(old_index < 0){
        old_index = max_index;
    }


    //Remove our self
    siblings.removeAll(node);

    QHash<const Node*, int> desired_index;

    //Re-insert at the desired location
    siblings.insert(new_index, node);
  
    //Force all children to be index ordered starting at 0. This should fix any gaps due to deleted elements
    for(int i = 0; i < siblings.count(); i++){
        auto sibling = siblings[i];
        desired_index[sibling] = i;
    }

    //Sort the List by Row (Lowest to Highest) and also by Index (Lowest to Highest)
    std::sort(siblings.begin(), siblings.end(), [=](const Node* node1, const Node* node2){
        if(node1 == node2){
            return false;
        }
        
        int node1_row = node1->getDataValue(KeyName::Row).toInt();
        int node2_row = node2->getDataValue(KeyName::Row).toInt();
        if(node1_row == node2_row){
            auto node1_subgroup = node1->getDataValue(KeyName::Column).toInt();
            auto node2_subgroup = node2->getDataValue(KeyName::Column).toInt();
            
            if(node1_subgroup == node2_subgroup){
                auto node1_index = desired_index[node1];
                auto node2_index = desired_index[node2];
                if(node1_index == node2_index){
                    qCritical() << "Got Invalid index' which are the same";
                }else{
                    //Sort Lowest to Highest Index
                    return node1_index < node2_index;
                }
            }else{
                return node1_subgroup < node2_subgroup;
            }
        }
        //Sort Lowest to Highest Row
        return node1_row < node2_row;
    });



    //Force the siblings to be reset
    for(int i = 0; i < siblings.count(); i++){
        auto sibling = siblings[i];
        auto sibling_data = sibling->getData(KeyName::Index);
        
        if(sibling_data){
            //Get the index the sibling node
            int current_index = sibling_data->getValue().toInt();
            if(sibling == node){
                
                new_index = i;
            }else if(current_index != i){
                //Force the value as to not end up in recursion
                forceDataValue(sibling_data, i);
            }
        }
    }
    //Return the index
    return new_index;
}

void IndexKey::RevalidateChildrenIndex(Node* parent){
    if(parent){
        auto children = parent->getChildren(0);

        if(children.size()){
            auto child = children.last();
            child->setDataValue(KeyName::Index, child->getDataValue(KeyName::Index));
        }
    }
}