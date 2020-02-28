#include "qobjectregistrar.h"

QObjectRegistrar::QObjectRegistrar(QObject *parent)
        : QObject(parent) {}

void QObjectRegistrar::registerObject(QObject *object)
{
    _registerObject(object);
}

void QObjectRegistrar::unregisterObject(QObject *object)
{
    _unregisterObject(object);
}

bool QObjectRegistrar::hasRegisteredObjects()
{
    return !registeredObjects.isEmpty();
}

void QObjectRegistrar::_registerObject(QObject *object)
{
    if(object && !registeredObjects.contains(object)){
        registeredObjects.append(object);
        connect(object, &QObject::destroyed, this, &QObjectRegistrar::unregisterObject);
    }
}

void QObjectRegistrar::_unregisterObject(QObject *object)
{
    if(object && registeredObjects.contains(object)){
        disconnect(object, &QObject::destroyed, this, &QObjectRegistrar::unregisterObject);
        registeredObjects.removeAll(object);
        if(registeredObjects.isEmpty()){
            emit lastRegisteredObjectRemoved();
        }
    }
}

