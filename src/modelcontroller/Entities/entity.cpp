#include "entity.h"
#include "data.h"
#include "key.h"
#include <QVariant>
#include <QDebug>

Entity::Entity(EntityFactoryBroker& broker, GRAPHML_KIND kind):GraphML(broker, kind)
{
}

Entity::~Entity()
{
    disconnect(this);
    for(auto data : data_map_){
        //Unregister the data so we don't bother calling back into this class
        data->setParent(0);
        delete data;
    }
}

/**
 * @brief Entity::addData
 * Attaches the Data to this Entity.
 * @param data The data to attach.
 * @return success.
 */
bool Entity::addData(Data *data)
{
    if(!data){
        //Can't add Null Data.
        return false;
    }

    Key* key = data->getKey();

    if(!key){
        //Can't add Data with Null key.
        return false;
    }


    if(getData(key)){
        //Can't have multiple data for the same key.
        return false;
    }

    if(data->getParent()){
        //Can't add data which has a parent already.
        return false;
    }

    QString keyName = key->getName();

    if(!key_lookup_.contains(keyName)){
        key_lookup_.insert(keyName, key);
    }

    if(!data_map_.contains(key)){
        data_map_.insert(key, data);
    }

    //Attach this.
    data->registerParent(this);
    DataAdded(data);
    return true;
}

/**
 * @brief Entity::addData
 * Adds a list of data to Entity.
 * @param dataList
 * @return All added
 */
bool Entity::addData(QList<Data *> dataList)
{
    bool success = true;
    foreach(Data* data, dataList){
        if(!addData(data)){
            success = false;
        }
    }
    return success;
}

void Entity::_dataChanged(Data *data)
{
    if(data){
        auto value = data->getValue();
        if(value.isValid()){
            emit dataChanged(getID(), data->getKeyName(), value, data->isProtected());
        }
    }
}

void Entity::_dataRemoved(Data *data)
{
    if(data){
        auto key_name = data->getKeyName();
        key_lookup_.remove(key_name);
        emit dataRemoved(getID(), key_name);
    }
}


/**
 * @brief Entity::getData
 * Gets the Data which has key with name KeyName
 * @param keyName - The name of the Data's key
 * @return The Data, or NULL
 */
Data *Entity::getData(QString keyName) const
{
    Key* key = getKey(keyName);
    return getData(key);
}

Key *Entity::getKey(QString keyName) const
{
    if(key_lookup_.contains(keyName)){
        return key_lookup_[keyName];
    }
    return 0;
}

/**
 * @brief Entity::getData
 * Gets the Data which has the key 'key'
 * @param key - The Data's Key
 * @return  The Data or NULL
 */
Data *Entity::getData(Key *key) const
{
    if(key && data_map_.contains(key)){
        return data_map_[key];
    }
    return 0;
}

/**
 * @brief Entity::getData
 * Gets all the Data attached to this entity.
 * @return
 */
QList<Data *> Entity::getData() const
{
    return data_map_.values();
}

QList<Key *> Entity::getKeys() const
{
    return key_lookup_.values();
}

QStringList Entity::getKeyNames() const
{
    return key_lookup_.keys();
}

bool Entity::gotData(QString keyName) const
{
    if(keyName.isEmpty()){
        return !data_map_.isEmpty();
    }else{
        return getData(keyName);
    }
}

bool Entity::gotData(Key* key) const{
    if(key){
        return getData(key->getName());
    }
    return false;
}

bool Entity::isNode() const
{
    return getGraphMLKind() == GRAPHML_KIND::NODE;
}

bool Entity::isEdge() const
{
    return getGraphMLKind() == GRAPHML_KIND::EDGE;
}

bool Entity::isReadOnly() const
{
    Data* readOnlyData = getData("readOnly");
    if(readOnlyData){
        return readOnlyData->getValue().toBool();
    }
    return false;
}

QVariant Entity::getDataValue(QString keyName) const
{
    Data* data = getData(keyName);
    if(data){
        return data->getValue();
    }
    return QVariant();
}

QVariant Entity::getDataValue(Key *key) const
{
    Data* data = getData(key);
    if(data){
        return data->getValue();
    }
    return QVariant();
}

bool Entity::setDataValue(QString keyName, QVariant value)
{
    return setDataValue(getKey(keyName), value);
}

bool Entity::setDataValue(Key *key, QVariant value)
{
     Data* data = getData(key);
     if(data){
        return data->setValue(value);
     }
     return false;
}

QStringList Entity::getProtectedKeys()
{
    QStringList protectedKeys;

    for(auto data : getData()){
        if(data->isProtected()){
            protectedKeys.append(data->getKeyName());
        }
    }
    return protectedKeys;
}

bool Entity::removeData(Key *key)
{
    return removeData(getData(key));
}


/**
 * @brief Entity::removeData
 * Removes the Data, data
 * @param data The Data to remove.
 * @return data was Removed
 */
bool Entity::removeData(Data *data)
{
    if(data){
        if(!data_map_.values().contains(data)){
            return false;

        }
        Key* key = data->getKey();
        if(!key){
            //Can't remove data which doesn't have a valid key.
            return false;
        }
        int count = 0;
        count += data_map_.remove(key);
        count += key_lookup_.remove(key->getName());

        data->setParent(0);
        
        emit dataRemoved(getID(), key->getName());

        return count == 2;
    }else{
        //Can't remove null data.
        return false;
    }
    return true;
}

/**
 * @brief Entity::removeData
 * Removes the Data which Key's name is keyName
 * @param keyName the name of the key to delete
 * @return removed successfully
 */
bool Entity::removeData(QString keyName)
{
    return removeData(getData(keyName));
}

QString Entity::toString() const
{
    return QString("[%1]%2 - %3").arg(QString::number(getID()), getDataValue("kind").toString() ,getDataValue("label").toString());
}


bool Entity::SortByIndex(const Entity* a, const Entity* b){
    auto a_ind = a->getDataValue("index").toInt();
    auto b_ind = b->getDataValue("index").toInt();
    return a_ind < b_ind;
}

bool Entity::isLabelFunctional() const{
    return is_label_functional_;
}

void Entity::revalidateData(){
    for(auto data : getData()){
        data->revalidateData();
    }
}

void Entity::setLabelFunctional(bool functional){
    if(is_label_functional_ != functional){
        is_label_functional_ = functional;
        revalidateData();
    }
}

bool Entity::isImplicitlyConstructed() const{
    return is_implicitly_constructed_;
}

void Entity::setImplicitlyConstructed(bool implicitly_constructed){
    is_implicitly_constructed_ = implicitly_constructed;
}