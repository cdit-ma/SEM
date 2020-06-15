#include "exportidkey.h"
#include "../data.h"
#include "../node.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include <QUuid>
#include <QCryptographicHash>
#include <QDebug>
#include <QList>

ExportIDKey::ExportIDKey(EntityFactoryBroker& broker, std::function<void (Entity*, QString, QString)> uuid_notifier): Key(broker, KeyName::UUID, QVariant::String){
    //Shouldn't be user modifyable
    setProtected(true);
    this->uuid_notifier_ = uuid_notifier;
}

QString ExportIDKey::getMD5UUID(const QString& str_val){
    QString md5_str = QString(QCryptographicHash::hash(str_val.toUtf8(), QCryptographicHash::Md5).toHex());
    //format {8-4-4-4-12}
    //https://en.wikipedia.org/wiki/Universally_unique_identifier
    QList<int> seperator_indices = {20, 16, 12, 8};

    for(auto i : seperator_indices){
        md5_str.insert(i, '-');
    }
    md5_str.insert(0, '{');
    md5_str += '}';


    return md5_str;
}

QString ExportIDKey::GetUUIDOfValue(const QString& str_val){
    //Check if the data value is a valid uuid
    auto uuid = QUuid(str_val);
    
    if(uuid.isNull()){
        if(str_val != ""){
            uuid = QUuid(getMD5UUID(str_val));
        }else{
            //If it's null lo
            uuid = QUuid::createUuid();
        }
    }
    return uuid.toString();
}

bool ExportIDKey::setData(Data* data, QVariant data_value){
    auto old_value = data->getValue().toString();
    //Change the data
    bool data_changed = Key::setData(data, data_value);
    auto entity = data->getParent();
    
    auto new_value = data->getValue().toString();
    if(uuid_notifier_){
        //Update the Factory
        uuid_notifier_(entity, old_value, new_value);
    }
    return data_changed;
}


QVariant ExportIDKey::validateDataChange(Data* data, QVariant data_value){
    return GetUUIDOfValue(data_value.toString());
}