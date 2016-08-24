#ifndef NODEVIEWITEM_H
#define NODEVIEWITEM_H
#include <QObject>

#include "../Model/node.h"
#include "viewitem.h"

class NodeViewItem: public ViewItem
{
    Q_OBJECT
public:
    NodeViewItem(ViewController* controller, int ID, ENTITY_KIND entityKind, QString kind, QHash<QString, QVariant> data, QHash<QString, QVariant> _properties);
    ~NodeViewItem();


    Node::NODE_KIND getNodeKind() const;
    NodeViewItem* getParentNodeViewItem();
    VIEW_ASPECT getViewAspect();
    int getParentID();

    bool isInModel();
    QString getTreeIndex();


    bool isAncestorOf(NodeViewItem* item);

private:
    Node::NODE_KIND nodeKind;
};
#endif // VIEWITEM_H
