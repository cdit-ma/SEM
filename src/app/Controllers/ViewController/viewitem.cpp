#include "viewitem.h"
#include "viewcontroller.h"
#include "../../../modelcontroller/modelcontroller.h"


#include <QDebug>
#include <QStack>
#include <QQueue>

bool ViewItem::SortByLabel(const ViewItem *a, const ViewItem *b){
    auto a_kind = a->getData("label").toString();
    auto b_kind = b->getData("label").toString();
    return a_kind < b_kind;
}

bool ViewItem::SortByKind(const ViewItem *a, const ViewItem *b){
    auto a_kind = a->getData("kind").toString();
    auto b_kind = b->getData("kind").toString();
    if(a_kind == b_kind){
        return SortByLabel(a, b);
    }else{
        return a_kind < b_kind;
    }
}

ViewItem::ViewItem(ViewController *controller, GRAPHML_KIND entity_kind)
{
    this->controller = controller;
    this->ID = -2;
    this->entityKind = entity_kind;
}

ViewItem::ViewItem(ViewController* controller, int ID, GRAPHML_KIND entity_kind)
{
    this->controller = controller;
    this->ID = ID;
    this->entityKind = entity_kind;
    //Add X/Y

    permanent_protected_keys.insert("ID");
    changeData("ID", ID);
    
    connect(this, SIGNAL(lastRegisteredObjectRemoved()), this, SLOT(deleteLater()));
}

ViewItem::~ViewItem()
{
}

VIEW_ASPECT ViewItem::getViewAspect()
{
    if(aspect == VIEW_ASPECT::NONE){
        aspect = getController()->getNodeViewAspect(getID());
    }
    return aspect;
}

int ViewItem::getID() const
{
    return ID;
}

DataTableModel *ViewItem::getTableModel()
{
    if(!table_model_){
        table_model_ = new DataTableModel(this);
    }
    return table_model_;
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


QVariant ViewItem::getData(const QString& key_name) const
{
    return _data.value(key_name);
}

QStringList ViewItem::getKeys() const
{
    return _data.keys();
}

bool ViewItem::hasData(const QString& keyName) const
{
    bool has_local = _data.contains(keyName);
    return has_local;
}

bool ViewItem::isDataProtected(const QString& key_name) const
{
    if(isReadOnly()){
        return true;
    }else{
        return permanent_protected_keys.contains(key_name) || protected_keys.contains(key_name);
    }
}

bool ViewItem::isDataVisual(const QString&) const
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

bool ViewItem::setDefaultIcon(const QString& prefix, const QString& name)
{
    if(default_icon.first != prefix){ 
        default_icon.first = prefix;
    }
    if(default_icon.second != name){
        default_icon.second = name;
    }

    
    default_valid = Theme::theme()->gotImage(default_icon);
    emit iconChanged();
    return default_valid;
}

bool ViewItem::setIcon(const QString& prefix, const QString& name)
{
    if(current_icon.first != prefix){
        current_icon.first = prefix;
    }
    if(current_icon.second != name){
        current_icon.second = name;
    }
    
    //Check with the theme?
    current_valid = Theme::theme()->gotImage(current_icon);
    emit iconChanged();
    return current_valid;
}


const IconPair& ViewItem::getIcon() const
{
    if(current_valid){
        return current_icon;
    }else if(default_valid){
        return default_icon;
    }else{
        static IconPair invalid_icon({"Icons", "circleQuestionDark"});
        return invalid_icon;
    }
}

void ViewItem::destruct()
{
    auto parent = getParentItem();
    if(parent){
        parent->removeChild(this);
    }
    
    if(hasRegisteredObjects()){
        emit destructing(ID);
    }else{
        deleteLater();
    }
}

void ViewItem::childAdded(ViewItem* child){
}
void ViewItem::childRemoved(ViewItem* child){
}

void ViewItem::addChild(ViewItem *child)
{
    if(child){
        auto& set = child->isNode() ? child_nodes_ : child_edges_;
        
        auto pre_size = set.size();
        set.insert(child);

        if(set.size() > pre_size){
            child->setParentViewItem(this);
            connect(child, &ViewItem::notificationsChanged, this, &ViewItem::nestedNotificationsChanged);
            childAdded(child);
        }
    }
}

void ViewItem::removeChild(ViewItem *child)
{
    if(child){
        auto& set = child->isNode() ? child_nodes_ : child_edges_;
        
        auto pre_size = set.size();
        set.remove(child);

        if(set.size() < pre_size){
            childRemoved(child);
        }
    }
}

QList<ViewItem *> ViewItem::getDirectChildren() const
{
    return child_nodes_.toList() + child_edges_.toList();
}

QList<ViewItem* > ViewItem::getNestedChildren(){
    QList<ViewItem*> nodes;
    QList<ViewItem*> edges;

    QQueue<ViewItem*> process_queue;
    process_queue.enqueue(this);

    while(!process_queue.isEmpty()){
        auto item = process_queue.dequeue();
        for(auto child : item->getDirectChildren()){
            if(child->isNode()){
                process_queue.enqueue(child);
            }else{
                edges << child;
            }
        }

        if(item != this){
            nodes << item;
        }
    }

    return nodes + edges;
}

ViewItem *ViewItem::getParentItem() const
{
    return _parent;
}

void ViewItem::setParentViewItem(ViewItem *item)
{
    _parent = item;
}

ViewController* ViewItem::getController(){
    return controller;
}


QList<QVariant> ViewItem::getValidValuesForKey(const QString& keyName) const
{
    QList<QVariant> valid_values;
    if(controller){
        valid_values = controller->getValidValuesForKey(ID, keyName);
    }
    return valid_values;
}

void ViewItem::changeData(const QString& keyName, QVariant data, bool is_protected)
{
    bool addedData = !_data.contains(keyName);
    _data[keyName] = data;
    
    if(is_protected){
        protected_keys.insert(keyName);
    }else{
        protected_keys.remove(keyName);
    }


    if(addedData){
        emit dataAdded(keyName, data);
    }else{
        emit dataChanged(keyName, data);
    }

    if(keyName == "label"){
        emit labelChanged(data.toString());
    }else if(keyName == "icon" || keyName == "icon_prefix"){
        updateIcon();
    }
}

void ViewItem::updateIcon(){
    auto icon = _data.value("icon", "").toString();
    auto icon_prefix = _data.value("icon_prefix", "").toString();
    setIcon(icon_prefix, icon);
}

void ViewItem::removeData(const QString& keyName)
{
    if(_data.contains(keyName)){
        _data.remove(keyName);
        emit dataRemoved(keyName);
    }
}


void ViewItem::updateProtectedKeys(const QList<QString>& protected_keys){
    this->protected_keys = protected_keys.toSet();
}


void ViewItem::updateProtectedKey(const QString& key_name, bool is_protected){
    if(is_protected){
        protected_keys.insert(key_name);
    }else{
        protected_keys.remove(key_name);
    }
}

void ViewItem::addNotification(QSharedPointer<NotificationObject> notification){
    notifications_.insert(notification);
    connect(notification.data(), &NotificationObject::notificationChanged, [=](QSharedPointer<NotificationObject>){emit notificationsChanged();});
    emit notificationsChanged();
}

void ViewItem::removeNotification(QSharedPointer<NotificationObject> notification){
    notifications_.remove(notification);
    emit notificationsChanged();
}

QSet<QSharedPointer<NotificationObject> > ViewItem::getNotifications(){
    return notifications_;
}

QSet<QSharedPointer<NotificationObject> > ViewItem::getNestedNotifications(){
    QSet<QSharedPointer<NotificationObject> > notifications;
    
    notifications += getNotifications();

    for(auto child : getDirectChildren()){
        notifications += child->getNestedNotifications();
    }
    return notifications;
}