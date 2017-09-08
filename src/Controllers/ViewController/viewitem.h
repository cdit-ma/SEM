#ifndef VIEWITEM_H
#define VIEWITEM_H
#include <QObject>

#include "../../enumerations.h"
#include "../../Views/Table/datatablemodel.h"
#include "../../Utils/qobjectregistrar.h"
#include "../../ModelController/nodekinds.h"
#include "../../ModelController/kinds.h"


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
    virtual bool isInModel();

    VIEW_ASPECT getViewAspect();

    QVariant getData(QString keyName) const;
    QStringList getKeys() const;
    bool hasData(QString keyName) const;

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

    QList<QVariant> getValidValuesForKey(QString keyName) const;
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
protected:
    QStringList getProtectedKeys() const;
    void changeData(QString keyName, QVariant data);
    void removeData(QString keyName);

    ViewController* getController();
private:
    void destruct();

private:
    ViewItem* _parent;
    DataTableModel* tableModel;
    QMultiMap<GRAPHML_KIND, ViewItem*> children;
    ViewController* controller;

    QHash<QString, QVariant> _data;

    int ID;
    QString kind;
    GRAPHML_KIND entityKind;

    QPair<QString, QString> defaultIcon;
    QPair<QString, QString> currentIcon;
    VIEW_ASPECT aspect = VIEW_ASPECT::NONE;
};
#endif // VIEWITEM_H
