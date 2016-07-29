#ifndef VIEWITEM_H
#define VIEWITEM_H
#include <QObject>

#include "../Controller/entityadapter.h"
#include "Table/attributetablemodel.h"
#include "../Controller/qobjectregistrar.h"

//Forward declaration.
class ViewController;

class ViewItem: public QObjectRegistrar
{
    friend class ViewController;

    Q_OBJECT
public:
    ViewItem(EntityAdapter* entity);
    ~ViewItem();

    int getID();
    AttributeTableModel* getTableModel();

    bool isNode();
    bool isEdge();

    QVariant getData(QString keyName);
    bool hasData(QString keyName);

    void setDefaultIcon(QString icon_prefix, QString icon_name);
    void setIcon(QString icon_prefix, QString icon_name);
    void resetIcon();
    QPair<QString, QString> getIcon();

    void addChild(ViewItem* child);
    void removeChild(ViewItem* child);
    QList<ViewItem *> getChildren();
    ViewItem* getParentItem();
    void setParentViewItem(ViewItem* item);

signals:
    void dataChanged(QString key_name, QVariant data);
    void destructing();
    void iconChanged();


private:
    void destruct();

    QList<ViewItem*> children;
    ViewItem* _parent;
    EntityAdapter* entity;
    int ID;
    AttributeTableModel* tableModel;
    QPair<QString, QString> defaultIcon;
    QPair<QString, QString> currentIcon;

    QVector<QObject*> _listeners;

};
#endif // VIEWITEM_H
