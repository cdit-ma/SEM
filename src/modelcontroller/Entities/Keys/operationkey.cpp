#include "operationkey.h"
#include "labelkey.h"

OperationKey::OperationKey(EntityFactoryBroker& broker): Key(broker, "operation", QVariant::String){
}   

QVariant OperationKey::validateDataChange(Data* data, QVariant data_value){
    return LabelKey::ValidateSystemLabel(data, data_value);
}