#include "entity.h"
#include "data.h"
#include "key.h"
#include <QVariant>
#include <QDebug>

Entity::Entity(GRAPHML_KIND kind):GraphML(kind)
{
}

Entity::~Entity()
{
    disconnect(this);
    for(auto data : dataLookup){
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

    if(!keyLookup.contains(keyName)){
        keyLookup.insert(keyName, key);
    }

    if(!dataLookup.contains(key)){
        dataLookup.insert(key, data);
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
            emit dataChanged(getID(), data->getKeyName(), value);
        }
    }
}

void Entity::_dataRemoved(Data *data)
{
    if(data){
        auto key_name = data->getKeyName();
        keyLookup.remove(key_name);
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
    if(keyLookup.contains(keyName)){
        return keyLookup[keyName];
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
    if(key && dataLookup.contains(key)){
        return dataLookup[key];
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
    return dataLookup.values();
}

QList<Key *> Entity::getKeys() const
{
    return keyLookup.values();
}

QStringList Entity::getKeyNames() const
{
    return keyLookup.keys();
}

bool Entity::gotData(QString keyName) const
{
    if(keyName.isEmpty()){
        return !dataLookup.isEmpty();
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

bool Entity::isSnippetReadOnly() const
{
    if(isReadOnly()){
        if(!getData("snippetID")){
            return false;
        }
        if(!getData("snippetMAC")){
            return false;
        }
        if(!getData("snippetTime")){
            return false;
        }
        return true;
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
    foreach(Data* data, getData()){
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
        if(!dataLookup.values().contains(data)){
            return false;

        }
        Key* key = data->getKey();
        if(!key){
            //Can't remove data which doesn't have a valid key.
            return false;
        }
        int count = 0;
        count += dataLookup.remove(key);
        count += keyLookup.remove(key->getName());

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

QString Entity::toString()
{
    return QString("[%1]%2 - %3").arg(QString::number(getID()), getDataValue("kind").toString() ,getDataValue("label").toString());
}


