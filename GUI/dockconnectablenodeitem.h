#ifndef DOCKCONNECTABLENODEITEM_H
#define DOCKCONNECTABLENODEITEM_H

#include "docknodeitem.h"
#include "dockscrollarea.h"
#include "../Model/graphml.h"
#include "./nodeitem.h"


class DockConnectableNodeItem : public DockNodeItem
{
    Q_OBJECT
public:
    explicit DockConnectableNodeItem(NodeItem *node_item, QWidget* parent = 0);

public slots:
    void updateData();
    void setOpacity(double opacity);

};

#endif // DOCKCONNECTABLENODEITEM_H
