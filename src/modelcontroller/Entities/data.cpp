#include "data.h"
#include <QDebug>

Data::Data(Key *key, QVariant value, bool protect):GraphML(GRAPHML_KIND::DATA)
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

    for(auto child_data : child_datas){
        child_data->removeParentData(this);
    }
    child_datas.clear();

    auto parent_data_copy = parent_datas;

    for(auto parent_data : parent_data_copy){
        removeParentData(parent_data);
    }
}

Data *Data::clone(Data *data)
{
    if(data){
        return new Data(data->getKey(), data->getValue(), data->isProtected());
    }
    return 0;
}

void Data::registerParent(Entity* parent){
    //Unregister the current parent
    if(this->parent){
        this->parent->_dataRemoved(this);
    }

    if(parent){
        setParent(parent);
        //Call the parent datachanged
        parent->_dataChanged(this);
        //Revalidate
        revalidateData();
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
    is_protected = protect;
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
        _setData(value);
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

QString Data::toGraphML(int indentDepth, bool functional_export)
{
    bool should_export = !functional_export || !getKey()->isVisual();

    QString xml;

    if(should_export){
        QString tabSpace;
        tabSpace.fill('\t', indentDepth);

        QString dataString = value.toString();

        dataString.replace( "&", "&amp;" );
        dataString.replace( ">", "&gt;" );
        dataString.replace( "<", "&lt;" );
        dataString.replace( "\"", "&quot;" );
        dataString.replace( "\'", "&apos;" );

        if(getKey()->getName() == "processes_to_log"){
            dataString.replace("\n", ",");
        }
        xml += tabSpace;
        xml += QString("<data key=\"%1\">%2</data>\n").arg(QString::number(getKey()->getID()), dataString);
    }
    return xml;
}

QString Data::toString()
{
    return QString("[" + QString::number(getID()) + "] Data " + getKeyName() + ": " + getValue().toString());
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
        parent_datas.insert(data);
        if(parent_datas.size() == 1){
            store_value();
        }

        connect(data, &Data::dataChanged, this, &Data::setValue);
        setValue(data->getValue());
    }
}

void Data::removeParentData(Data* data){
    if(data && parent_datas.contains(data)){
        parent_datas.remove(data);
        disconnect(data, &Data::dataChanged, this, &Data::setValue);
        if(parent_datas.isEmpty()){
            restore_value();
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

void Data::revalidateData(){
    setValue(getValue());
}

void Data::updateChildren(bool changed)
{
    //Send a signal saying the data changed, regardless of whether it did.
    /*if(changed){
        for(auto data: child_data){
            data->setValue(value);
        }
    }*/
    if(parent){
        parent->_dataChanged(this);
    }

    if(value.isValid()){
        emit dataChanged(value);
    }
}

void Data::store_value(){
    old_value = value;
}

void Data::restore_value(){
    //qCritical() << toString() << ": RESTORING VALUE: " << old_value;
    setValue(old_value);
}