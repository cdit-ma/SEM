#ifndef NODEVIEWITEM_H
#define NODEVIEWITEM_H
#include <QObject>

#include "../Controller/nodeadapter.h"
#include "viewitem.h"

class NodeViewItem: public ViewItem
{
    Q_OBJECT
public:
    NodeViewItem(int ID, ENTITY_KIND entityKind, QString kind, QHash<QString, QVariant> data, QHash<QString, QVariant> _properties);
    ~NodeViewItem();


    NodeViewItem* getParentNodeViewItem();
    VIEW_ASPECT getViewAspect();
    int getParentID();

    bool isInModel();
    QString getTreeIndex();


    bool isAncestorOf(NodeViewItem* item);
};
#endif // VIEWITEM_H
