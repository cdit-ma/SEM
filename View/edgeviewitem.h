#ifndef EDGEVIEWITEM_H
#define EDGEVIEWITEM_H

#include "viewitem.h"

class EdgeViewItem: public ViewItem
{
    Q_OBJECT
public:
    EdgeViewItem(int ID, ENTITY_KIND entityKind, QString kind, QHash<QString, QVariant> data, QHash<QString, QVariant> _properties);
    ~EdgeViewItem();

    int getSourceID();
    int getDestinationID();
};
#endif // VIEWITEM_H
