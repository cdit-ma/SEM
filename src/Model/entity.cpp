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
    //Connect to self.
}

Entity::~Entity()
{
    //Clear data.
    while(!getData().isEmpty()){
        Data* data = getData().first();
        removeData(data);
        delete data;
    }
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
    data->setParent(this);
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
        emit dataChanged(getID(), data->getKeyName(), data->getValue());
    }
}

void Entity::_dataRemoved(Data *data)
{
    if(data){
        //Remove Key.
        QString keyName = data->getKeyName();
        keyLookup.remove(keyName);
        emit dataRemoved(getID(), data->getKeyName());
    }
}

void Entity::_dataProtected(Data *data)
{
    if(data){
        emit propertyChanged(getID(), "protectedKeys", getProtectedKeys());
    }
}

void Entity::updateDefaultData(QString key_name, QVariant::Type type, bool is_protected, QVariant value){

    if(!default_data_.contains(key_name)){
        default_data_.insert(key_name, new DefaultData());
    }
    if(default_data_.contains(key_name)){
        auto data = default_data_[key_name];
        data->key_name = key_name;
        data->type = type;
        data->is_protected = is_protected;
        if(value.isValid()){
            data->value = value;
        }
    }
}

void Entity::setMoveEnabled(bool enabled){
    position_enabled = enabled;
}
void Entity::setSizeEnabled(bool enabled){
    size_enabled  = enabled;
}
void Entity::setExpandEnabled(bool enabled){
    expand_enabled = enabled;
}

bool Entity::isMoveEnabled(){
    return position_enabled;
}
bool Entity::isSizeEnabled(){
    return size_enabled;
}
bool Entity::isExpandEnabled(){
    return expand_enabled;
}

QList<DefaultData*> Entity::getDefaultDataStructs(){
    return default_data_.values();
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

bool Entity::isNode() const
{
    return entityKind == EK_NODE;
}

bool Entity::isEdge() const
{
    return entityKind == EK_EDGE;
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

QString Entity::getEntityName() const
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
