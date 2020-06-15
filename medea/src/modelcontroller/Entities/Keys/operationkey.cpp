#include "operationkey.h"
#include "labelkey.h"

OperationKey::OperationKey(EntityFactoryBroker& broker): Key(broker, KeyName::Operation, QVariant::String){
}   

QVariant OperationKey::validateDataChange(Data* data, QVariant data_value){
    return LabelKey::ValidateSystemLabel(data, data_value);
}