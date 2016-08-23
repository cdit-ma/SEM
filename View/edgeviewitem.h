#ifndef EDGEVIEWITEM_H
#define EDGEVIEWITEM_H

#include "viewitem.h"

class NodeViewItem;
class EdgeViewItem: public ViewItem
{
    Q_OBJECT
public:
    EdgeViewItem(ViewController* controller, int ID, NodeViewItem* parent, NodeViewItem* src, NodeViewItem* dst, QString kind, QHash<QString, QVariant> data, QHash<QString, QVariant> properties);
    ~EdgeViewItem();

    NodeViewItem* getSource();
    NodeViewItem* getDestination();
    NodeViewItem* getParentItem();
    int getSourceID();
    int getDestinationID();
private:
    NodeViewItem* source;
    NodeViewItem* destination;
    NodeViewItem* parent;

    int sID;
    int dID;
};
#endif // VIEWITEM_H
