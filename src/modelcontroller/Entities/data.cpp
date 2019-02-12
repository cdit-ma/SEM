#include "data.h"
#include "../entityfactorybroker.h"
#include <QDebug>


bool Data::Data::LinkData(Entity* source, const QString &source_key, Entity* destination, const QString &destination_key, bool setup){
    auto source_data = source->getData(source_key);
    auto destination_data = destination->getData(destination_key);

    if(source_data && destination_data){
        return source_data->linkData(destination_data, setup);
    }
    return false;
}

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
        revalidateData();
        updateChildren(true);
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

bool Data::_setValue(QVariant new_value, bool validate){
    if(key && validate){
        return key->setData(this, new_value);
    }else{
        return _setData(new_value);
    }
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

bool Data::CompareData(const Data* a, const Data* b){
    if(a && b){
        return a->getValue() == b->getValue();
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
        
        setValue("");
        if(parent){
            parent->_dataChanged(this);
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

    for(auto data : child_datas){
        data->setValue(value);
    }

    if(changed){
        emit dataChanged();
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

QList<QVariant> Data::getValidValues(){
    return valid_values_;
}