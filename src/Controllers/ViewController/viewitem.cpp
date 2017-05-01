#include "viewitem.h"
#include "viewcontroller.h"
#include "../modelcontroller.h"

#include <QDebug>
#include <QStack>

ViewItem::ViewItem(ViewController* controller, int ID, ENTITY_KIND entityKind, QString kind, QHash<QString, QVariant> dataList, QHash<QString, QVariant> properties)
{
    this->controller = controller;
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
    tableModel = new DataTableModel(this);
}

ViewItem::ViewItem(ViewController *controller)
{
    this->controller = controller;
    this->ID = -2;
    _parent = 0 ;
    tableModel = 0;
}

ViewItem::ViewItem(ViewController* controller, int ID, ENTITY_KIND entity_kind)
{
    this->controller = controller;
    this->ID = ID;
    this->entityKind = entity_kind;
    _parent = 0 ;
    tableModel = new DataTableModel(this);;
}

ViewItem::~ViewItem()
{
}

int ViewItem::getID() const
{
    return ID;
}

DataTableModel *ViewItem::getTableModel()
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

bool ViewItem::isInModel() const
{
    if(hasProperty("inModel")){
        return getProperty("inModel").toBool();
    }
    return false;

}

QVariant ViewItem::getData(QString key_name) const
{
    return _data.value(key_name);
    /*
    QVariant data;

    if(controller){
        controller->getEntityDataValue(ID, key_name);
    }
    return data;*/
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
    QStringList keys;
    if(controller){
        keys = controller->getEntityKeys(ID);
    }
    return keys;
}

QStringList ViewItem::getProperties() const
{
    return _properties.keys();
}

bool ViewItem::hasData(QString keyName) const
{
    return true;
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

bool ViewItem::isDataVisual(QString) const
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
        setIcon(iconPrefix, iconName);
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
        emit destructing(ID);
    }else{
        deleteLater();
    }
}


void ViewItem::addChild(ViewItem *child)
{
    if(child){
        ENTITY_KIND ek = child->getEntityKind();
        if(!children.contains(ek, child)){
            children.insertMulti(ek, child);
            child->setParentViewItem(this);
        }
    }
}

void ViewItem::removeChild(ViewItem *child)
{
    if(child){
        ENTITY_KIND ek = child->getEntityKind();
        children.remove(ek, child);
    }
}

QVector<ViewItem *> ViewItem::getDirectChildren() const
{
    return children.values().toVector();
}

QList<ViewItem *> ViewItem::getNestedChildren() const
{
    //Edges Last
    QList<ViewItem*> nodes;
    QList<ViewItem*> edges;

    QStack<ViewItem*> items;
    items.append(getDirectChildren());

    while(!items.isEmpty()){
        ViewItem* child = items.takeLast();
        if(child->isNode()){
            nodes.append(child);
        }else{
            edges.append(child);
        }
        items.append(child->getDirectChildren());
    }
    return nodes + edges;
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
    QStringList keys;
    if(controller){
        keys = controller->getModelController()->getProtectedEntityKeys(ID);
    }
    return keys;
}

ViewController* ViewItem::getController(){
    return controller;
}


QList<QVariant> ViewItem::getValidValuesForKey(QString keyName) const
{
    QList<QVariant> valid_values;
    if(controller){
        valid_values = controller->getValidValuesForKey(ID, keyName);
    }
    return valid_values;
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


