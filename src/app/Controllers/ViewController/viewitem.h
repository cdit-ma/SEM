#ifndef VIEWITEM_H
#define VIEWITEM_H
#include <QObject>

#include "../../Views/Table/datatablemodel.h"
#include "../../Utils/qobjectregistrar.h"
#include "../../../modelcontroller/nodekinds.h"
#include "../../../modelcontroller/kinds.h"
#include "../NotificationManager/notificationobject.h"


//Forward declaration.
class ViewController;

class ViewItem: public QObjectRegistrar
{
    friend class ViewController;

    Q_OBJECT
public:

    static bool SortByLabel(const ViewItem *a, const ViewItem *b);
    static bool SortByKind(const ViewItem *a, const ViewItem *b);

    ViewItem(ViewController* controller, GRAPHML_KIND entity_kind);
    ViewItem(ViewController* controller, int ID, GRAPHML_KIND entity_kind);
    ~ViewItem();

    DataTableModel* getTableModel();

    int getID() const;
    GRAPHML_KIND getEntityKind() const;
    bool isNode() const;
    bool isEdge() const;

    VIEW_ASPECT getViewAspect();

    QVariant getData(QString keyName) const;
    QStringList getKeys() const;
    bool hasData(QString keyName) const;

    bool isDataProtected(QString keyName) const;
    bool isDataVisual(QString keyName) const;

    bool isReadOnly() const;

    void updateProtectedKeys(QList<QString> protected_keys);
    void updateProtectedKey(QString key_name, bool is_protected);

    void setDefaultIcon(QString iconPrefix, QString iconName);
    void setIcon(QString iconPrefix, QString iconName);
    void resetIcon();

    QPair<QString, QString> getIcon() const;


    void addChild(ViewItem* child);

    
    void removeChild(ViewItem* child);
    QList<ViewItem *> getDirectChildren() const;
    QList<ViewItem* > getNestedChildren();

    ViewItem* getParentItem();
    void setParentViewItem(ViewItem* item);

    void addNotification(QSharedPointer<NotificationObject> obj);
    void removeNotification(QSharedPointer<NotificationObject> obj);

    QSet<QSharedPointer<NotificationObject> > getNotifications();
    QSet<QSharedPointer<NotificationObject> > getNestedNotifications();

    QList<QVariant> getValidValuesForKey(QString keyName) const;
signals:
    void labelChanged(QString label);
    void iconChanged();

    void dataAdded(QString keyName, QVariant data);
    void dataChanged(QString keyName, QVariant data);
    void dataRemoved(QString keyName);

    void notificationsChanged();
    void nestedNotificationsChanged();

    void showNotifications(Notification::Severity severity);

    void destructing(int ID);
protected:
    void changeData(QString keyName, QVariant data, bool is_protected = false);
    void removeData(QString keyName);
    virtual void childAdded(ViewItem* child);
    virtual void childRemoved(ViewItem* child);
    

    ViewController* getController();
private:
    void destruct();

private:
    ViewItem* _parent;
    DataTableModel* tableModel;
    QMultiMap<GRAPHML_KIND, ViewItem*> children;
    ViewController* controller;

    QHash<QString, QVariant> _data;
    QSet<QString> protected_keys;
    QSet<QString> permanent_protected_keys;


    int ID;
    QString kind;
    GRAPHML_KIND entityKind;

    QPair<QString, QString> defaultIcon;
    QPair<QString, QString> currentIcon;
    VIEW_ASPECT aspect = VIEW_ASPECT::NONE;
    
    QSet<QSharedPointer<NotificationObject> > notifications_;
};
#endif // VIEWITEM_H
