#ifndef NODEVIEWITEMACTION_H
#define NODEVIEWITEMACTION_H

#include "../View/nodeviewitem.h"
#include "rootaction.h"
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

    NodeViewItem* getNodeViewItem();


private slots:
    void iconChanged();
    void labelChanged(QString label);
private:
    NodeViewItem* nodeViewItem;
    NodeViewItemAction* parentViewItemAction;
};

#endif // NODEVIEWITEMACTION_H
