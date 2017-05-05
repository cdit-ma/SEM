#include "viewitem.h"
#include "viewcontroller.h"
#include "../../ModelController/modelcontroller.h"

#include <QDebug>
#include <QStack>

ViewItem::ViewItem(ViewController *controller)
{
    this->controller = controller;
    this->ID = -2;
    _parent = 0 ;
    tableModel = 0;
}

ViewItem::ViewItem(ViewController* controller, int ID, GRAPHML_KIND entity_kind)
{
    this->controller = controller;
    this->ID = ID;
    this->entityKind = entity_kind;
    //Add X/Y
    

    connect(this, SIGNAL(lastRegisteredObjectRemoved()), this, SLOT(deleteLater()));
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

GRAPHML_KIND ViewItem::getEntityKind() const
{
    return entityKind;
}

bool ViewItem::isNode() const
{
    return getEntityKind() == GRAPHML_KIND::NODE;
}

bool ViewItem::isEdge() const
{
    return getEntityKind() == GRAPHML_KIND::EDGE;
}

bool ViewItem::isInModel()
{
    return false;
}

QVariant ViewItem::getData(QString key_name) const
{
    return _data.value(key_name);
}

QStringList ViewItem::getKeys() const
{
    QStringList keys;
    if(controller){
        keys = controller->getEntityKeys(ID);
    }
    return keys;
}

bool ViewItem::hasData(QString keyName) const
{
    bool has_local = _data.contains(keyName);

    if(!has_local){
        if(getKeys().contains(keyName)){
            return true;
        }    
    }
    return has_local;
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
        GRAPHML_KIND ek = child->getEntityKind();
        if(!children.contains(ek, child)){
            children.insertMulti(ek, child);
            child->setParentViewItem(this);
        }
    }
}

void ViewItem::removeChild(ViewItem *child)
{
    if(child){
        GRAPHML_KIND ek = child->getEntityKind();
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


