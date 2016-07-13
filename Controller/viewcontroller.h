#ifndef VIEWCONTROLLER_H
#define VIEWCONTROLLER_H

#include "entityadapter.h"
#include "../View/viewitem.h"
#include "selectionhandler.h"


class ViewController : public QObject
{
    Q_OBJECT
public:
    ViewController();

    SelectionHandler* getSelectionHandler(QObject* object);

    void setDefaultIcon(ViewItem* viewItem);
signals:
    void viewItemConstructed(ViewItem* viewItem);
    void viewItemDestructing(int ID, ViewItem *viewItem);

private slots:
    void entityConstructed(EntityAdapter* entity);
    void entityDestructed(EntityAdapter* entity);

private:
    QHash<int, ViewItem*> viewItems;
    SelectionHandler* selectionHandler;
};

#endif // VIEWCONTROLLER_H
