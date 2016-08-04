#include "edgeviewitem.h"


EdgeViewItem::EdgeViewItem(int ID, ENTITY_KIND entityKind, QString kind, QHash<QString, QVariant> data, QHash<QString, QVariant> properties):ViewItem(ID, entityKind, kind, data, properties)
{
}

EdgeViewItem::~EdgeViewItem()
{

}

int EdgeViewItem::getSourceID()
{
    int ID = -1;
    return ID;
}

int EdgeViewItem::getDestinationID()
{
    int ID = -1;
    return ID;
}
