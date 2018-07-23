#include "data.h"
#include "../entityfactorybroker.h"
#include <QDebug>

Data::Data(EntityFactoryBroker& broker, Key *key, QVariant value, bool protect) : GraphML(broker, GRAPHML_KIND::DATA)
{
    this->key = key;
    this->key_name = key->getName();
    setProtected(key->isProtected());

    if(value.isValid()){
        setValue(value);
    }

    if(protect){
        setProtected(protect);
    }
}

Data::~Data()
{
    //Unset the parent
    if(parent){
        parent->removeData(this);
    }

    while(parent_datas.size()){
        auto top = *parent_datas.begin();
        top->unlinkData(this);
    }

    while(child_datas.size()){
        auto top = *child_datas.begin();
        unlinkData(top);
    }
}

void Data::registerParent(Entity* parent){
    //Unregister the current parent
    if(this->parent){
        this->parent->_dataRemoved(this);
    }

    if(parent){
        setParent(parent);
        //Revalidate
        if(!revalidateData()){
            //Fake the validity
            updateChildren(true);
        }
    }
};

void Data::setParent(Entity *parent)
{
    this->parent = parent;
}

Entity *Data::getParent()
{
    return parent;
}

void Data::setProtected(bool protect)
{
    if(is_protected != protect){
        is_protected = protect;
        if(parent){
            parent->_dataChanged(this);
        }
    }
}


bool Data::isProtected() const
{
    return parent_datas.size() || is_protected;
}

bool Data::_setValue(QVariant value, bool validate){
    bool success = true;
    if(key && validate){
        success = key->setData(this, value);
    }else{
        success = _setData(value);
    }
    return success;
}

bool Data::_setData(QVariant new_value){
    //Check if the data changed
    bool data_changed = new_value != value;
    this->value = new_value;
    updateChildren(data_changed);
    return data_changed;
}


bool Data::forceValue(QVariant value){
    return _setValue(value, false);
}

bool Data::setValue(QVariant value)
{
    return _setValue(value, true);
}


bool Data::isParentData(Data* data)
{
    return parent_datas.contains(data);
}


void Data::clearValue()
{
    setValue("");
    updateChildren();
}

bool Data::CompareData(const Data* a, const Data* b){
    if(a && b){
        return a->getValue() == b->getValue();
    }
    return false;
}

bool Data::compare(const Data *data) const
{
    if(data){
        return value == data->getValue();
    }
    return false;
}


Key *Data::getKey() const
{
    return key;
}

QString Data::getKeyName() const
{
    return key_name;
}

bool Data::SortByKey(const Data* a, const Data* b){
    return GraphML::SortByID(a->getKey(), b->getKey());
}

QVariant Data::getValue() const
{
    return value;
}

QString SanitizeString(const QString& str){
    const static QString str_illegal_and("&");
    const static QString str_safe_and("&amp;");
    const static QString str_illegal_gt(">");
    const static QString str_safe_gt("&gt;");
    const static QString str_illegal_lt("<");
    const static QString str_safe_lt("&lt;");
    const static QString str_illegal_dq("\"");
    const static QString str_safe_dq("&quot;");
    const static QString str_illegal_sq("\'");
    const static QString str_safe_sq("&apos;");

    QString sanitized_string = str;
    sanitized_string.replace(str_illegal_and, str_safe_and);
    sanitized_string.replace(str_illegal_gt, str_safe_gt);
    sanitized_string.replace(str_illegal_lt, str_safe_lt);
    sanitized_string.replace(str_illegal_dq, str_safe_dq);
    sanitized_string.replace(str_illegal_sq, str_safe_sq);
    return sanitized_string;
}

void Data::ToGraphmlStream(QTextStream& stream, int indent_depth){
    auto data_string = SanitizeString(value.toString());

    const static QString process_key("processes_to_log");
    if(getKey()->getName() == process_key){
        const static QString search_str("\n");
        const static QString replace_str(",");
        data_string.replace(search_str, replace_str);
    }
    stream << QString("\t").repeated(indent_depth);
    stream << "<data key=\"";
    stream << getKey()->getID();
    stream << "\">";
    stream << data_string;
    stream << "</data>\n";
}

QString Data::toString() const
{
    auto id = getID();
    if(this->parent){
        id = parent->getID();
    }
    return QString("[" + QString::number(id) + "] Data Key: '" + getKeyName() + "' = '" + getValue().toString() + "'");
}

bool Data::linkData(Data* data, bool setup_bind){
    if(setup_bind){
        return linkData(data);
    }else{
        return unlinkData(data);
    }
}

void Data::addParentData(Data* data){
    if(data && !parent_datas.contains(data)){
        bool was_protected = isProtected();
        parent_datas += data;
        store_value();

        connect(data, &Data::dataChanged, this, &Data::setValue);
        
        const auto& new_value = data->getValue();
        
        if(new_value != getValue()){
            //If the new value is different, change it
            setValue(new_value);
        }else if(parent && !was_protected){
            //Notify that our data has changed protected state
            parent->_dataChanged(this);
        }
    }
}

void Data::removeParentData(Data* data){
    if(data && parent_datas.contains(data)){
        parent_datas.remove(data);
        disconnect(data, &Data::dataChanged, this, &Data::setValue);
        
        if(parent_datas.empty()){
            restore_value();
            if(parent){
                parent->_dataChanged(this);
            }
        }
    }
}

bool Data::linkData(Data* data){
    if(data && !child_datas.contains(data)){
        child_datas.insert(data);
        data->addParentData(this);
        return true;
    }
    return false;
}

bool Data::unlinkData(Data* data){
    if(data && child_datas.contains(data)){
        child_datas.remove(data);
        data->removeParentData(this);

        return true;
    }
    return false;
}

bool Data::revalidateData(){
    return setValue(getValue());
}

void Data::updateChildren(bool changed)
{
    if(parent && changed){
        parent->_dataChanged(this);
    }

    if(value.isValid()){
        emit dataChanged(value);
    }
}

void Data::store_value(){
    old_values_.push(value);
}

void Data::restore_value(){
    if(old_values_.size()){
        setValue(old_values_.pop());
    }
}



void Data::addValidValue(QVariant value){
    if(!valid_values_.contains(value)){
        valid_values_.append(value);
    }
}

void Data::addValidValues(QList<QVariant> values){
    for(auto value : values){
        addValidValue(value);
    }

    //Revalidate
    revalidateData();
}

void Data::removeValidValue(QVariant value){
    valid_values_.removeAll(value);
}

void Data::clearValidValues(){
    valid_values_.clear();
}

bool Data::gotValidValues(){
    return valid_values_.size();
}

QList<QVariant> Data::getValidValues(){
    return valid_values_;
}