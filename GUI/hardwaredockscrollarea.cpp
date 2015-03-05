#include "hardwaredockscrollarea.h"
#include "dockconnectablenodeitem.h"


/**
 * @brief HardwareDockScrollArea::HardwareDockScrollArea
 * @param label
 * @param view
 * @param parent
 */
HardwareDockScrollArea::HardwareDockScrollArea(QString label, NodeView* view, DockToggleButton *parent) :
    DockScrollArea(label, view, parent) {}


/**
 * @brief HardwareDockScrollArea::updateDock
 * This updates the Harware nodes dock.
 */
void HardwareDockScrollArea::updateDock()
{
    // do updates for hardware nodes dock here
}


/**
 * @brief HardwareDockScrollArea::dock_itemClicked
 * When an item in this dock is clicked, if it's allowed, it connects the
 * selected node to the selected hardware node from the dock.
 */
void HardwareDockScrollArea::dockNodeItemClicked()
{
    DockConnectableNodeItem* sender = qobject_cast<DockConnectableNodeItem*>(QObject::sender());
    getNodeView()->view_constructEdge(getNodeView()->getSelectedNode(), sender->getNodeItem()->getNode());
}
