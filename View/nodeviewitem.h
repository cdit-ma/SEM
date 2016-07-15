#ifndef NODEVIEWITEM_H
#define NODEVIEWITEM_H
#include <QObject>

#include "../Controller/nodeadapter.h"
#include "viewitem.h"

class NodeViewItem: public ViewItem
{
    Q_OBJECT
public:
    NodeViewItem(NodeAdapter* entity);
    ~NodeViewItem();


    VIEW_ASPECT getViewAspect();
    int getParentID(int depth = 1);

    NODE_CLASS getNodeClass();
    bool isInModel();
    QList<int> getTreeIndex();

    bool isAncestorOf(NodeViewItem* item);
private:
    NodeAdapter* entity;
};
#endif // VIEWITEM_H
