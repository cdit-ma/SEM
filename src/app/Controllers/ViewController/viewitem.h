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

    QVariant getData(const QString& keyName) const;
    QStringList getKeys() const;
    bool hasData(const QString& keyName) const;

    bool isDataProtected(const QString& keyName) const;

    bool isReadOnly() const;


    bool setDefaultIcon(const QString& prefix, const QString& name);
    bool setIcon(const QString& prefix, const QString& name);

    const QPair<QString, QString>& getIcon() const;


    void addChild(ViewItem* child);

    
    void removeChild(ViewItem* child);
    QSet<ViewItem *> getDirectChildren() const;
    QList<ViewItem* > getNestedChildren();

    ViewItem* getParentItem() const;
    void setParentViewItem(ViewItem* item);

    void addNotification(QSharedPointer<NotificationObject> obj);
    void removeNotification(QSharedPointer<NotificationObject> obj);

    QSet<QSharedPointer<NotificationObject> > getNotifications();
    QSet<QSharedPointer<NotificationObject> > getNestedNotifications();

    QList<QVariant> getValidValuesForKey(const QString& keyName) const;
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
    void updateIcon();
    void changeData(const QString& keyName, QVariant data, bool is_protected = false);
    void removeData(const QString& keyName);
    virtual void childAdded(ViewItem* child);
    virtual void childRemoved(ViewItem* child);

    virtual void Disconnect();
    

    ViewController* getController();
private:
    void destruct();
private:
    ViewItem* _parent = 0;
    DataTableModel* table_model_ = 0;

    QSet<ViewItem*> child_nodes_;
    QSet<ViewItem*> child_edges_;
    
    ViewController* controller;

    QHash<QString, QVariant> _data;
    QSet<QString> protected_keys;
    QSet<QString> permanent_protected_keys;


    int ID;
    QString kind;
    GRAPHML_KIND entityKind;

    bool default_valid = false;
    QPair<QString, QString> default_icon;
    
    bool current_valid = false;
    QPair<QString, QString> current_icon;

    VIEW_ASPECT aspect = VIEW_ASPECT::NONE;
    
    QSet<QSharedPointer<NotificationObject> > notifications_;
};
#endif // VIEWITEM_H
