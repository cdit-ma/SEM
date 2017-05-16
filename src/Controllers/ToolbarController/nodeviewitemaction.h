#ifndef NODEVIEWITEMACTION_H
#define NODEVIEWITEMACTION_H

#include "../../Utils/rootaction.h"
#include "../ViewController/nodeviewitem.h"

class NodeViewItemAction : public RootAction
{
    Q_OBJECT
public:
    NodeViewItemAction(NodeViewItem* item);
    void setParentNodeViewItemAction(NodeViewItemAction* item);
    ~NodeViewItemAction();
    NodeViewItemAction* getParentViewItemAction();

    int getID();
    QString getKind();
    NODE_KIND getNodeKind();
    QString getLabel();

    NodeViewItem* getNodeViewItem();


private slots:
    void _labelChanged(QString label);
    void _iconChanged();
private:
    NodeViewItem* nodeViewItem;
    NodeViewItemAction* parentViewItemAction;
};

#endif // NODEVIEWITEMACTION_H
