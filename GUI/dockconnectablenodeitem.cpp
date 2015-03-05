#include "dockconnectablenodeitem.h"


/**
 * @brief DockConnectableNodeItem::DockConnectableNodeItem
 * @param node_item
 * @param parent
 */
DockConnectableNodeItem::DockConnectableNodeItem(NodeItem *node_item, QWidget* parent) :
    DockNodeItem("", node_item, parent)
{
    // connect to node item
    connect(node_item, SIGNAL(updateDockNodeItem()), this, SLOT(updateData()));
    connect(node_item, SIGNAL(updateOpacity(qreal)), this, SLOT(setOpacity(qreal)));
    connect(node_item, SIGNAL(destroyed()), this, SLOT(deleteLater()));
}


/**
 * @brief DockConnectableNodeItem::updateData
 * This gets called when the dataTable value for the node item has been changed.
 */
void DockConnectableNodeItem::updateData()
{
    setLabel(getNodeItem()->getGraphML()->getDataValue("label"));
    repaint();
}


/**
 * @brief DockConnectableNodeItem::setOpacity
 * This disables this button when SHIFT is held down and the currently
 * selected node can't be connected to this button.
 * @param opacity
 */
void DockConnectableNodeItem::setOpacity(double opacity)
{
    if (opacity < 1) {
        setEnabled(false);
    } else {
        setEnabled(true);
    }
    repaint();
}
