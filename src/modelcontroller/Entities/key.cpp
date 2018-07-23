#include "key.h"

#include "entity.h"
#include "data.h"
#include "node.h"
#include "../nodekinds.h"

#include <QDebug>
#include <QStringBuilder>

const QString Type_Boolean("boolean");
const QString Type_Int("int");
const QString Type_LongLong("longlong");
const QString Type_Double("double");
const QString Type_String("string");


QString Key::getGraphMLTypeName(const QVariant::Type type)
{
    switch(type){
        case QVariant::Bool:
            return Type_Boolean;
        case QVariant::Int:
            return Type_Int;
        case QVariant::LongLong:
            return Type_LongLong;
        case QVariant::Double:
            return Type_Double;
        default:
            return Type_String;
    }
}

QVariant::Type Key::getTypeFromGraphML(const QString& type_string)
{
    const static QHash<QString, QVariant::Type> type_map({
        {Type_Boolean, QVariant::Bool},
        {Type_Int, QVariant::Int},
        {Type_LongLong, QVariant::LongLong},
        {Type_Double, QVariant::Double},
        {Type_String, QVariant::String}
    });

    return type_map.value(type_string, (QVariant::Type)0);
}

Key::Key(EntityFactoryBroker& broker, const QString& key_name, QVariant::Type type) : GraphML(broker, GRAPHML_KIND::KEY){
    key_name_ = key_name;
    key_type_ = type;
}

Key::~Key()
{
}

void Key::setProtected(bool protect)
{
    is_protected_ = protect;
}

bool Key::isProtected() const
{
    return is_protected_;
}

QString Key::getName() const
{
    return key_name_;
}

QVariant::Type Key::getType() const
{
    return key_type_;
}

bool Key::forceDataValue(Data* data, QVariant value){
    bool result = false;
    if(data){
        result = data->forceValue(value);
    }
    return result;
}

QVariant Key::validateDataChange(Data *data, QVariant new_value)
{
    if(!data || data->getKey() != this){
        return new_value;
    }

    Entity* entity = data->getParent();
    QVariant value = data->getValue();
    
    int ID = -1;
    if(entity){
        ID = entity->getID();
    }

    //Check if the value can be converted to this key type
    if(!new_value.canConvert(key_type_)){
        if(new_value.isValid()){
            emit validation_failed(ID, "Value cannot be converted to Key's type.");
            return value;
        }else{
            new_value = QVariant(key_type_);
        }
    }else{
        new_value.convert(key_type_);
    }

    auto valid_values = data->getValidValues();

    if(valid_values.size()){
        if(!valid_values.contains(new_value)){
            emit validation_failed(ID, "Value not in list of valid values.");
            //Use the first value
            new_value = valid_values.first();
        }
    }

    return new_value;
}

bool Key::setData(Data* data, QVariant data_value){
    bool data_changed = false;
    if(data){
        auto valid_value = validateDataChange(data, data_value);
        data_changed = data->_setData(valid_value);
    }
    return data_changed;
}


void Key::ToGraphmlStream(QTextStream& stream, int indent_depth){
    stream << QString("\t").repeated(indent_depth);
    stream << "<key";
    stream << " attr.name=\"" << getName() << "\"";
    stream << " attr.type=\"" << getGraphMLTypeName(key_type_) << "\"";
    stream << " id=\"" << getID() << "\"";
    stream << "/>\n";
}

QString Key::toString() const
{
    QString str;
    QTextStream stream(&str); 
    //Todo get actual edge type
    stream << "[" << getID() << "] Key " << getName() << " Type: " << getGraphMLTypeName(key_type_);
    return str;
}