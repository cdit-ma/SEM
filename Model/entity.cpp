#include "entity.h"
#include "data.h"
#include "key.h"
#include <QVariant>
#include <QDebug>
/**
 * @brief Entity::Entity
 * An Entity is any item that can adopt Data.
 * @param kind - The Kind of the Entity
 */
QString Entity::getEntityKindName(const Entity::ENTITY_KIND entityKind)
{
    switch(entityKind){
        case Entity::EK_ALL:
            return "all";
        case Entity::EK_EDGE:
            return "edge";
        case Entity::EK_NODE:
            return "node";
        default:
            return "none";
    }
}

Entity::ENTITY_KIND Entity::getEntityKind(const QString entityString)
{
    QString entityStringLC = entityString.toLower();
    if(entityStringLC == "all"){
        return EK_ALL;
    }else if(entityStringLC == "edge"){
        return EK_EDGE;
    }else if(entityStringLC == "node"){
        return EK_NODE;
    }else{
        return EK_NONE;
    }
}

Entity::Entity(Entity::ENTITY_KIND kind):GraphML(GraphML::GK_ENTITY)
{
    entityKind = kind;
}

/**
 * @brief Entity::getEntityKind
 * Get the Kind of the Entity. Useful for down casting.
 * @return
 */
Entity::ENTITY_KIND Entity::getEntityKind() const
{
    return entityKind;
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
        //TODO Copy?
        //Can't have multiple data for the same key.
        return false;
    }

    ENTITY_KIND keyKind = key->getEntityKind();
    if(keyKind != EK_ALL && keyKind != getEntityKind()){
        //Can't add data which isn't for this entity kind.
        return false;
    }
    if(data->getParent()){
        //Can't add data which has a parent already.
        return false;
    }

    data->setParent(this);
    int dataID = data->getID();
    int keyID = key->getID();
    QString keyName = key->getName();

    //Add data to the hashes
    lookupDataID2Data[dataID] = data;
    lookupKeyID2DataID[keyID] = dataID;
    lookupKeyName2KeyID[key->getName()] = keyID;


    emit dataAdded(keyName, data->getValue());
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

/**
 * @brief Entity::getData
 * Gets the Data which has key with name KeyName
 * @param keyName - The name of the Data's key
 * @return The Data, or NULL
 */
Data *Entity::getData(QString keyName)
{
    int dataID = getDataIDFromKeyName(keyName);
    return getDataFromDataID(dataID);
}

/**
 * @brief Entity::getData
 * Gets the Data which has the key 'key'
 * @param key - The Data's Key
 * @return  The Data or NULL
 */
Data *Entity::getData(Key *key)
{
    int dataID = -1;
    if(key){
        dataID = getDataIDFromKeyID(key->getID());
    }
    return getDataFromDataID(dataID);
}

Data *Entity::getData(int ID)
{
    return getDataFromDataID(ID);
}

/**
 * @brief Entity::getData
 * Gets all the Data attached to this entity.
 * @return
 */
QList<Data *> Entity::getData()
{
    return lookupDataID2Data.values();
}

QList<Key *> Entity::getKeys(int)
{
    QList<Key *> keys;
  foreach(Data* data, getData()){
      Key* key = data->getKey();
      if(!keys.contains(key)){
          keys.append(key);
      }
  }
  return keys;
}

bool Entity::hasData(QString keyName)
{
    if(keyName != ""){
        return lookupKeyName2KeyID.contains(keyName);
    }
    return !lookupKeyName2KeyID.isEmpty();
}

QStringList Entity::getKeyNames()
{
    return lookupKeyName2KeyID.keys();
}

bool Entity::isNode()
{
    return entityKind == EK_NODE;
}

bool Entity::isEdge()
{
    return entityKind == EK_EDGE;
}

bool Entity::isReadOnly()
{
    Data* readOnlyData = getData("readOnly");
    if(readOnlyData){
        return readOnlyData->getValue().toBool();
    }
    return false;
}

QVariant Entity::getDataValue(QString keyName)
{
    Data* data = getData(keyName);
    if(data){
        return data->getValue();
    }
    return QVariant();
}

void Entity::setDataValue(QString keyName, QVariant value)
{
     Data* data = getData(keyName);
     if(data){
         data->setValue(value);
     }
}

QString Entity::getEntityName()
{
    return getEntityKindName(entityKind);
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
        if(data->getParent() != this){
            //Don't remove data which isn't owned by this.
            return false;
        }
        int dataID = data->getID();
        Key* key = data->getKey();

        if(lookupDataID2Data.contains(dataID)){
            //Remove data from Hash
            lookupDataID2Data.remove(dataID);
        }
        if(!key){
            //Can't remove data which doesn't have a valid key.
            return false;
        }

        int keyID = key->getID();
        QString keyName = key->getName();
        if(lookupKeyID2DataID.contains(keyID)){
            //Remove keyID lookup
            lookupKeyID2DataID.remove(keyID);
        }
        if(lookupKeyName2KeyID.contains(keyName)){
            //Remove keyName lookup
            lookupKeyName2KeyID.remove(keyName);
        }

        emit dataRemoved(keyName);
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
    int keyID = getKeyIDFromKeyName(keyName);
    int dataID = getDataIDFromKeyID(keyID);

    if(dataID == -1){
        return false;
    }

    if(lookupDataID2Data.contains(dataID)){
        //Remove data from Hash
        lookupDataID2Data.remove(dataID);
    }

    if(lookupKeyID2DataID.contains(keyID)){
        //Remove keyID lookup
        lookupKeyID2DataID.remove(keyID);
    }

    if(lookupKeyName2KeyID.contains(keyName)){
        //Remove keyName lookup
        lookupKeyName2KeyID.remove(keyName);
    }
    return true;
}

void Entity::dataChanged(int, QString keyName, QVariant data)
{
    emit dataChanged(keyName, data);
}

/**
 * @brief Entity::getDataIDFromKeyName
 * Gets the ID of the data which has Key with name keyName
 * @param keyName - The name of the Key
 * @return the ID or -1 if failed.
 */
int Entity::getDataIDFromKeyName(QString keyName)
{
    int keyID = getKeyIDFromKeyName(keyName);
    return getDataIDFromKeyID(keyID);
}

/**
 * @brief Entity::getKeyIDFromKeyName
 * Gets the ID of the key which has name keyName
 * @param keyName - The name of the key
 * @return the ID or -1 if failed.
 */
int Entity::getKeyIDFromKeyName(QString keyName)
{
    if(keyName != "" && lookupKeyName2KeyID.contains(keyName)){
        return lookupKeyName2KeyID[keyName];
    }
    return -1;
}

/**
 * @brief Entity::getDataIDFromKeyID
 * Gets the ID of the data which has a key with ID keyID
 * @param keyID - The ID of the key
 * @return  the ID or -1 if failed.
 */
int Entity::getDataIDFromKeyID(int keyID)
{
    int dataID = -1;

    if(keyID != -1 && lookupKeyID2DataID.contains(keyID)){
        dataID = lookupKeyID2DataID[keyID];
    }

    return dataID;
}

/**
 * @brief Entity::getDataFromDataID
 * Gets the Data contained by this Entity, by it's ID
 * @param dataID The ID of the Data
 * @return the Data or NULL
 */
Data *Entity::getDataFromDataID(int dataID)
{
    if(dataID != -1 && lookupDataID2Data.contains(dataID)){
        return lookupDataID2Data[dataID];
    }
    return 0;
}
