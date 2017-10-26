#include "exportidkey.h"
#include "../data.h"
#include "../node.h"
#include "../../entityfactory.h"
#include <QUuid>
#include <QCryptographicHash>
#include <QDebug>
#include <QList>

ExportIDKey::ExportIDKey(): Key("uuid", QVariant::String){
    //Shouldn't be user modifyable
    setProtected(true);
}

QString ExportIDKey::getMD5UUID(const QString str_val){
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

QString ExportIDKey::GetUUIDOfValue(const QString str_val){
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

QVariant ExportIDKey::validateDataChange(Data* data, QVariant data_value){
    Node* node = 0;
    auto str_val = data_value.toString();

    auto uuid_str = GetUUIDOfValue(str_val);

    auto entity_factory = getFactory();

    if(entity_factory){
        if(data->getParent()){
            entity_factory->EntityUUIDChanged(data->getParent(), uuid_str);
        }
    }
    return uuid_str;
}