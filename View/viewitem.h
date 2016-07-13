#ifndef VIEWITEM_H
#define VIEWITEM_H
#include <QObject>

#include "../Controller/entityadapter.h"

//Forward declaration.
class ViewController;

class ViewItem: public QObject
{
    friend class ViewController;

    Q_OBJECT
public:
    ViewItem(EntityAdapter* entity);
    ~ViewItem();

    int getID();

    bool isNode();
    bool isEdge();


    QVariant getData(QString keyName);
    bool hasData(QString keyName);

    void setDefaultIcon(QString icon_prefix, QString icon_name);
    void setIcon(QString icon_prefix, QString icon_name);
    void resetIcon();
    QPair<QString, QString> getIcon();

    void addListener(QObject* object);
    void removeListener(QObject* object);
    bool hasListeners();
signals:
    void dataChanged(QString key_name, QVariant data);
    void destructing();
    void iconChanged();
private:
    void destruct();
    EntityAdapter* entity;
    int ID;
    QPair<QString, QString> defaultIcon;
    QPair<QString, QString> currentIcon;

    QVector<QObject*> _listeners;

};
#endif // VIEWITEM_H
