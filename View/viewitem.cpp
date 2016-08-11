#include "viewitem.h"
#include <QDebug>

ViewItem::ViewItem(int ID, ENTITY_KIND entityKind, QString kind, QHash<QString, QVariant> dataList, QHash<QString, QVariant> properties)
{
    this->ID = ID;
    this->kind = kind;
    this->entityKind = entityKind;
    _data = dataList;
    _properties = properties;
    connect(this, SIGNAL(lastRegisteredObjectRemoved()), this, SLOT(deleteLater()));

    //Set the default icon.
    defaultIcon = defaultIcon;//TODO: Theme::theme()->getIconForViewItem(this);
    currentIcon = defaultIcon;
    _parent = 0 ;
    tableModel = new AttributeTableModel(this);
}

ViewItem::ViewItem()
{
    this->ID = -2;
    _parent = 0 ;
    tableModel = 0;
}

ViewItem::~ViewItem()
{
}

int ViewItem::getID() const
{
    return ID;
}

AttributeTableModel *ViewItem::getTableModel()
{
    return tableModel;
}

ENTITY_KIND ViewItem::getEntityKind() const
{
    return entityKind;
}

bool ViewItem::isNode() const
{
    return getEntityKind() == EK_NODE;
}

bool ViewItem::isEdge() const
{
    return getEntityKind() == EK_EDGE;
}

QVariant ViewItem::getData(QString keyName) const
{
    if(_data.contains(keyName)){
        return _data[keyName];
    }
    return QVariant();
}

QVariant ViewItem::getProperty(QString propertyName) const
{
    if(_properties.contains(propertyName)){
        return _properties[propertyName];
    }
    return QVariant();
}

QStringList ViewItem::getKeys() const
{
    return _data.keys();
}

QStringList ViewItem::getProperties() const
{
    return _properties.keys();
}

bool ViewItem::hasData(QString keyName) const
{
    return _data.contains(keyName);
}

bool ViewItem::hasProperty(QString propertyName) const
{
    return _properties.contains(propertyName);
}

bool ViewItem::isDataProtected(QString keyName) const
{
    if(isReadOnly()){
        return true;
    }else{
        return getProtectedKeys().contains(keyName);
    }
}

bool ViewItem::isDataVisual(QString keyName) const
{
    return false;
}

bool ViewItem::isReadOnly() const
{
    bool readOnly = false;
    if(hasData("readOnly")){
        readOnly = getData("readOnly").toBool();
    }
    return readOnly;
}
void ViewItem::setDefaultIcon(QString iconPrefix, QString iconName)
{
    //If the icon is different to what we have currently, update and send signal.
    if(defaultIcon.first != iconPrefix || defaultIcon.second != iconName){
        defaultIcon.first = iconPrefix;
        defaultIcon.second = iconName;
        if(currentIcon.first == "" || currentIcon.second == ""){
            setIcon(iconPrefix, iconName);
        }
    }
}

void ViewItem::setIcon(QString iconPrefix, QString iconName)
{
    //If the icon is different to what we have currently, update and send signal.
    if(currentIcon.first != iconPrefix || currentIcon.second != iconName){
        currentIcon.first = iconPrefix;
        currentIcon.second = iconName;
        emit iconChanged();
    }
}

void ViewItem::resetIcon()
{
    setIcon(defaultIcon.first, defaultIcon.second);
}

QPair<QString, QString> ViewItem::getIcon() const
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

QList<int> ViewItem::getChildrenID()
{
    QList<int> children;

    foreach(ViewItem* child, getChildren()){
        children.append(child->getID());
    }
    return children;
}

ViewItem *ViewItem::getParentItem()
{
    return _parent;
}

void ViewItem::setParentViewItem(ViewItem *item)
{
    _parent = item;
}

QStringList ViewItem::getProtectedKeys() const
{
    return getProperty("protectedKeys").toStringList();
}

QStringList ViewItem::getValidValuesForKey(QString keyName) const
{
    //TODO
    QStringList data;
    //if(entity){
    //    data = entity->getValidValuesForKey(keyName);
    //}
    return data;
}

void ViewItem::changeData(QString keyName, QVariant data)
{
    bool addedData = !_data.contains(keyName);
    _data[keyName] = data;

    if(addedData){
        emit dataAdded(keyName, data);
    }else{
        emit dataChanged(keyName, data);
    }

    if(keyName == "label"){
        emit labelChanged(data.toString());
    }
}

void ViewItem::removeData(QString keyName)
{
    if(_data.contains(keyName)){
        _data.remove(keyName);
        emit dataRemoved(keyName);
    }
}

void ViewItem::changeProperty(QString propertyName, QVariant data)
{
    bool addedProperty = !_properties.contains(propertyName);
    _properties[propertyName] = data;

    if(addedProperty){
        emit propertyAdded(propertyName, data);
    }else{
        emit propertyChanged(propertyName, data);
    }
}

void ViewItem::removeProperty(QString propertyName)
{
    if(_properties.contains(propertyName)){
        _properties.remove(propertyName);
        emit propertyRemoved(propertyName);
    }
}


