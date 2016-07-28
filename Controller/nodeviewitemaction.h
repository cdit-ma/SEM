#ifndef NODEVIEWITEMACTION_H
#define NODEVIEWITEMACTION_H

#include <QAction>
#include "../View/nodeviewitem.h"
class NodeViewItemAction : public QAction
{
    Q_OBJECT
public:
    NodeViewItemAction(NodeViewItem* item);
    ~NodeViewItemAction();

private slots:
    void iconChanged();
    void labelChanged(QString label);
private:
    NodeViewItem* nodeViewItem;
};

#endif // NODEVIEWITEMACTION_H
