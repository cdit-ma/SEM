#include "qobjectregistrar.h"

QObjectRegistrar::QObjectRegistrar(QObject *parent) : QObject(parent)
{
}

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

void QObjectRegistrar::unregisterObject()
{
    _registerObject(sender());
}

void QObjectRegistrar::_registerObject(QObject *object)
{
    if(object && !registeredObjects.contains(object)){
        registeredObjects.append(object);
        connect(object, SIGNAL(destroyed(QObject*)), this, SLOT(unregisterObject()));
    }
}

void QObjectRegistrar::_unregisterObject(QObject *object)
{
    if(object && registeredObjects.contains(object)){
        disconnect(object, SIGNAL(destroyed(QObject*)), this, SLOT(unregisterObject()));
        registeredObjects.removeAll(object);
        if(registeredObjects.isEmpty()){
            emit lastRegisteredObjectRemoved();
        }
    }
}

