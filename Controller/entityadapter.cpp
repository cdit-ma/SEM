#include "entityadapter.h"
#include <QObject>
#include "../Model/data.h"
#include <QDebug>

EntityAdapter::EntityAdapter(Entity *entity): QObject(0)
{
    _isValid = true;
    _entity = entity;
    _isNode = entity->getEntityKind() == Entity::EK_NODE;
    _ID = entity->getID();

    //Connect the Signals.
    connect(entity, SIGNAL(dataChanged(QString,QVariant)), this, SIGNAL(dataChanged(QString,QVariant)));
    connect(entity, SIGNAL(dataAdded(QString,QVariant)), this, SIGNAL(dataAdded(QString,QVariant)));
    connect(entity, SIGNAL(dataRemoved(QString)), this, SIGNAL(dataRemoved(QString)));
}

EntityAdapter::~EntityAdapter()
{
    //qCritical() << "REMOVING EntityAdapter" << getID();
}

int EntityAdapter::getID()
{
    return _ID;
}

bool EntityAdapter::isNodeAdapter()
{
    return _isNode;
}

bool EntityAdapter::isEdgeAdapter()
{
    return !_isNode;
}

bool EntityAdapter::isReadOnly()
{
    if(isValid()){
        return _entity->isReadOnly();
    }
    return false;
}

const QVariant EntityAdapter::getDataValue(QString keyName)
{
    QVariant value;
    if(isValid()){
        value = _entity->getDataValue(keyName);
    }
    return value;
}

bool EntityAdapter::isDataProtected(QString keyName)
{
    if(isValid()){
        Data* data = _entity->getData(keyName);
        if(data){
            if(data->isVisualData()){
                return false;
            }else{
                return data->isProtected();
            }
        }
    }
    return false;
}

QStringList EntityAdapter::getKeys()
{
    QStringList list;
    if(isValid()){
        list = _entity->getKeyNames();
    }
    return list;
}

bool EntityAdapter::hasData(QString keyName)
{
    if(isValid()){
        return _entity->hasData(keyName);
    }
    return false;
}

QVariant::Type EntityAdapter::getKeyType(QString keyName)
{
    if(isValid()){
        Data* data = _entity->getData(keyName);
        if(data){
            Key* key = data->getKey();
            if(key){
                return key->getType();
            }
        }
    }
    return QVariant::Invalid;

}

bool EntityAdapter::isValid()
{
    return _isValid;
}

void EntityAdapter::addListener(QObject *object)
{
    if(!_listeners.contains(object)){
        _listeners.append(object);
    }
}

void EntityAdapter::removeListener(QObject *object)
{
   if(_listeners.contains(object)){
        _listeners.removeAll(object);

        if(_listeners.isEmpty()){
            deleteLater();
        }
   }

}

void EntityAdapter::invalidate()
{
    _isValid = false;
}
