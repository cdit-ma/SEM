#ifndef EDGEVIEWITEM_H
#define EDGEVIEWITEM_H
#include <QObject>

#include "../Controller/edgeadapter.h"
#include "viewitem.h"

class EdgeViewItem: public ViewItem
{
    Q_OBJECT
public:
    EdgeViewItem(EdgeAdapter* entity);
    ~EdgeViewItem();

    int getSourceID();
    int getDestinationID();
private:
    EdgeAdapter* entity;
};
#endif // VIEWITEM_H
