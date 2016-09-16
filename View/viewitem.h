#ifndef VIEWITEM_H
#define VIEWITEM_H
#include <QObject>

#include "../enumerations.h"
#include "Table/attributetablemodel.h"
#include "../Controller/qobjectregistrar.h"

//Forward declaration.
class ViewController;

class ViewItem: public QObjectRegistrar
{
    friend class ViewController;

    Q_OBJECT
public:
    ViewItem(ViewController* controller, int ID, ENTITY_KIND entityKind, QString kind, QHash<QString, QVariant> data, QHash<QString, QVariant> _properties);
    ViewItem(ViewController* controller);
    ~ViewItem();

    AttributeTableModel* getTableModel();

    int getID() const;
    ENTITY_KIND getEntityKind() const;
    bool isNode() const;
    bool isEdge() const;

    QVariant getData(QString keyName) const;
    QVariant getProperty(QString propertyName) const;
    QStringList getKeys() const;
    QStringList getProperties() const;
    bool hasData(QString keyName) const;
    bool hasProperty(QString propertyName) const;

    bool isDataProtected(QString keyName) const;
    bool isDataVisual(QString keyName) const;

    bool isReadOnly() const;

    void setDefaultIcon(QString iconPrefix, QString iconName);
    void setIcon(QString iconPrefix, QString iconName);
    void resetIcon();

    QPair<QString, QString> getIcon() const;


    void addChild(ViewItem* child);
    void removeChild(ViewItem* child);
    QVector<ViewItem *> getDirectChildren() const;
    QList<ViewItem* > getNestedChildren() const;

    ViewItem* getParentItem();
    void setParentViewItem(ViewItem* item);

    QStringList getValidValuesForKey(QString keyName) const;
signals:
    void labelChanged(QString label);
    void iconChanged();

    void dataAdded(QString keyName, QVariant data);
    void dataChanged(QString keyName, QVariant data);
    void dataRemoved(QString keyName);

    void propertyAdded(QString propertyName, QVariant data);
    void propertyChanged(QString propertyName, QVariant data);
    void propertyRemoved(QString propertyName);

    void destructing(int ID);
private:
    QStringList getProtectedKeys() const;
    void changeData(QString keyName, QVariant data);
    void removeData(QString keyName);

    void changeProperty(QString propertyName, QVariant data);
    void removeProperty(QString propertyName);

private:
    void destruct();

private:
    ViewItem* _parent;
    AttributeTableModel* tableModel;
    QMultiMap<ENTITY_KIND, ViewItem*> children;
    ViewController* controller;

    QHash<QString, QVariant> _data;
    QHash<QString, QVariant> _properties;

    int ID;
    QString kind;
    ENTITY_KIND entityKind;

    QPair<QString, QString> defaultIcon;
    QPair<QString, QString> currentIcon;
};
#endif // VIEWITEM_H
