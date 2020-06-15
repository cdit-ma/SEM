#include "replicatecountkey.h"
#include "../data.h"
#include "../node.h"

ReplicateCountKey::ReplicateCountKey(EntityFactoryBroker& broker): Key(broker, KeyName::ReplicateCount, QVariant::Int){
}

QVariant ReplicateCountKey::validateDataChange(Data* data, QVariant data_value){    
    
    int new_index = data_value.toInt();

    //Don't set Below 0
    if(new_index < 1){
        new_index = 1;
    }

    //Return the index
    return new_index;
}