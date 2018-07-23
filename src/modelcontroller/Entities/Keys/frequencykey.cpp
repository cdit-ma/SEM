#include "frequencykey.h"
#include "../data.h"
#include "../node.h"

FrequencyKey::FrequencyKey(EntityFactoryBroker& broker): Key(broker, KeyName::Frequency, QVariant::Double){

}   

QVariant FrequencyKey::validateDataChange(Data* data, QVariant data_value){    
    
    double new_freq = data_value.toDouble();

    double current_data = data ? data->getValue().toDouble() : 1;

    //Don't set Below 0
    if(new_freq <= 0){
        new_freq = current_data;
    }

    return new_freq;
}