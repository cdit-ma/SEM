#include "viewitem.h"
#include <QDebug>

ViewItem::ViewItem(EntityAdapter *entity): QObjectRegistrar()
{
    this->entity = entity;
    this->ID = entity->getID();
    tableModel = new AttributeTableModel(this);

    //Register Item to Adapter
    entity->registerObject(this);

    connect(this, SIGNAL(lastRegisteredObjectRemoved()), this, SLOT(deleteLater()));

    connect(entity, SIGNAL(dataAdded(QString,QVariant)), this, SIGNAL(dataAdded(QString,QVariant)));
    connect(entity, SIGNAL(dataChanged(QString,QVariant)), this, SIGNAL(dataChanged(QString,QVariant)));
    connect(entity, SIGNAL(dataRemoved(QString)), this, SIGNAL(dataRemoved(QString)));

    //Set the default icon.
    defaultIcon = defaultIcon;//TODO: Theme::theme()->getIconForViewItem(this);
    currentIcon = defaultIcon;
    _parent = 0 ;
}

ViewItem::~ViewItem()
{
    if(entity){
        //De-register Item from adapter
        entity->unregisterObject(this);
    }
}

int ViewItem::getID()
{
    return ID;
}

AttributeTableModel *ViewItem::getTableModel()
{
    return tableModel;
}

bool ViewItem::isNode()
{
    if(entity){
        return entity->isNodeAdapter();
    }
    return false;
}

bool ViewItem::isEdge()
{
    if(entity){
        return entity->isEdgeAdapter();
    }
    return false;
}


QVariant ViewItem::getData(QString keyName)
{
    QVariant data;
    if(entity){
        data = entity->getDataValue(keyName);
    }
    return data;
}

QStringList ViewItem::getKeys()
{
    QStringList keys;
    if(entity){
        keys = entity->getKeys();
    }
    return keys;
}

bool ViewItem::hasData(QString keyName)
{
    bool hasData = false;
    if(entity){
        hasData = entity->hasData(keyName);
    }
    return hasData;
}

bool ViewItem::isDataProtected(QString keyName)
{
    bool locked = false;
    if(entity){
        locked = entity->isDataProtected(keyName);
    }
    return locked;
}

bool ViewItem::isDataVisual(QString keyName)
{
    bool visual = false;
    if(entity){
        visual = entity->isDataVisual(keyName);
    }
    return visual;
}

void ViewItem::setDefaultIcon(QString icon_prefix, QString icon_name)
{
    //If the icon is different to what we have currently, update and send signal.
    if(defaultIcon.first != icon_prefix || defaultIcon.second != icon_name){
        defaultIcon.first = icon_prefix;
        defaultIcon.second = icon_name;
        if(currentIcon.first == "" || currentIcon.second == ""){
            setIcon(icon_prefix, icon_name);
        }
    }
}

void ViewItem::setIcon(QString icon_prefix, QString icon_name)
{
    //If the icon is different to what we have currently, update and send signal.
    if(currentIcon.first != icon_prefix || currentIcon.second != icon_name){
        currentIcon.first = icon_prefix;
        currentIcon.second = icon_name;
        emit iconChanged();
    }
}

void ViewItem::resetIcon()
{
    setIcon(defaultIcon.first, defaultIcon.second);
}

QPair<QString, QString> ViewItem::getIcon()
{
    return currentIcon;
}

void ViewItem::destruct()
{
    if(hasRegisteredObjects()){
        emit destructing();
    }else{
        deleteLater();
    }
}


void ViewItem::addChild(ViewItem *child)
{
    if(!children.contains(child)){
        children.append(child);
        child->setParentViewItem(this);
    }
}

void ViewItem::removeChild(ViewItem *child)
{
    if(children.contains(child)){
        children.removeAll(child);
    }
}

QList<ViewItem *> ViewItem::getChildren()
{
    QList<ViewItem*> _children;

    foreach(ViewItem* child, children){
        _children.append(child);
        _children.append(child->getChildren());
    }
    return _children;
}

ViewItem *ViewItem::getParentItem()
{
    return _parent;
}

void ViewItem::setParentViewItem(ViewItem *item)
{
    _parent = item;
}


