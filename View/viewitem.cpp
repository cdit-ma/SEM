#include "viewitem.h"
#include <QDebug>

ViewItem::ViewItem(EntityAdapter *entity): QObjectRegistrar()
{
    this->entity = entity;
    this->ID = entity->getID();
    tableModel = new AttributeTableModel(entity);

    //Register Item to Adapter
    entity->addListener(this);

    connect(this, SIGNAL(lastRegisteredObjectRemoved()), this, SLOT(deleteLater()));

    connect(entity, SIGNAL(dataChanged(QString,QVariant)), this, SIGNAL(dataChanged(QString,QVariant)));

    //Set the default icon.
    defaultIcon = defaultIcon;//TODO: Theme::theme()->getIconForViewItem(this);
    currentIcon = defaultIcon;

}

ViewItem::~ViewItem()
{
    if(entity){
        //De-register Item from adapter
        entity->removeListener(this);
    }
}

int ViewItem::getID()
{
    return ID;
}

QAbstractItemModel *ViewItem::getTableModel()
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

bool ViewItem::hasData(QString keyName)
{
    bool hasData = false;
    if(entity){
        hasData = entity->hasData(keyName);
    }
    return hasData;
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

