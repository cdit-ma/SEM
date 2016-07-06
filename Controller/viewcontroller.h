#ifndef VIEWCONTROLLER_H
#define VIEWCONTROLLER_H

#include "entityadapter.h"
#include "../View/viewitem.h"


class ViewController : public QObject
{
    Q_OBJECT
public:
    ViewController();

    //SelectionHandler* constructSelectionHandler(QObject* parent);
signals:
    void viewItemConstructed(ViewItem* viewItem);
    void viewItemDestructing(int ID);

private slots:
    void entityConstructed(EntityAdapter* entity);
    void entityDestructed(EntityAdapter* entity);

private:
    QHash<int, ViewItem*> viewItems;
};

#endif // VIEWCONTROLLER_H
