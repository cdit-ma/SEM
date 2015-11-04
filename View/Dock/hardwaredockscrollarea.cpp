#include "hardwaredockscrollarea.h"
#include "docktogglebutton.h"
#include "docknodeitem.h"

#include <QDebug>


/**
 * @brief HardwareDockScrollArea::HardwareDockScrollArea
 * @param label
 * @param view
 * @param parent
 */
HardwareDockScrollArea::HardwareDockScrollArea(QString label, NodeView* view, DockToggleButton *parent) :
    DockScrollArea(label, view, parent)
{
    // populate list of not allowed kinds
    hardware_notAllowedKinds.append("Model");
    hardware_notAllowedKinds.append("InterfaceDefinitions");
    hardware_notAllowedKinds.append("BehaviourDefinitions");
    hardware_notAllowedKinds.append("DeploymentDefinitions");
    hardware_notAllowedKinds.append("AssemblyDefinitions");
    hardware_notAllowedKinds.append("IDL");
    hardware_notAllowedKinds.append("Component");
    hardware_notAllowedKinds.append("ComponentImpl");
    hardware_notAllowedKinds.append("HardwareNode");
    hardware_notAllowedKinds.append("Aggregate");
    hardware_notAllowedKinds.append("Member");
    hardware_notAllowedKinds.append("AggregateInstance");
    hardware_notAllowedKinds.append("MemberInstance");
    hardware_notAllowedKinds.append("InEventPort");
    hardware_notAllowedKinds.append("OutEventPort");
    hardware_notAllowedKinds.append("Attribute");
    hardware_notAllowedKinds.append("InEventPortImpl");
    hardware_notAllowedKinds.append("OutEventPortImpl");
    hardware_notAllowedKinds.append("AttributeImpl");
    hardware_notAllowedKinds.append("Workload");
    hardware_notAllowedKinds.append("Process");
    hardware_notAllowedKinds.append("BranchState");
    hardware_notAllowedKinds.append("Condition");
    hardware_notAllowedKinds.append("Termination");
    hardware_notAllowedKinds.append("Variable");
    hardware_notAllowedKinds.append("PeriodicEvent");
    hardware_notAllowedKinds.append("InEventPortInstance");
    hardware_notAllowedKinds.append("OutEventPortInstance");
    hardware_notAllowedKinds.append("AttributeInstance");
    hardware_notAllowedKinds.append("InEventPortDelegate");
    hardware_notAllowedKinds.append("OutEventPortDelegate");
    hardware_notAllowedKinds.append("BlackBox");

    setNotAllowedKinds(hardware_notAllowedKinds);
}


/**
 * @brief HardwareDockScrollArea::edgeDeleted
 */
void HardwareDockScrollArea::edgeDeleted()
{
    updateDock();
}


/**
 * @brief HardwareDockScrollArea::dock_itemClicked
 * When an item in this dock is clicked, if it's allowed, it connects the
 * selected node to the selected hardware node from the dock.
 */
void HardwareDockScrollArea::dockNodeItemClicked()
{
    DockNodeItem* dockNodeItem = qobject_cast<DockNodeItem*>(sender());
    if (!dockNodeItem) {
        return;
    }

    /*
     * At this point everything in selectedNodeIDs is deployable.
     * If all nodes in selection are already connected to dockNodeID, disconnect them.
     * If some nodes in selection aren't connected to dockNodeID, disconnect their deployment edge, and connect to docknodeID.
     */

    int dockId = dockNodeItem->getID().toInt();
    getNodeView()->constructDestructEdges(getNodeView()->getSelectedNodeIDs(), dockId);
}


/**
 * @brief DefinitionsDockScrollArea::updateDock
 * This is called whenever a node item is selected.
 * It checks to see if this dock should be enabled for the currently selected item.
 */
void HardwareDockScrollArea::updateDock()
{    
    QList<GraphMLItem*> selectedItems = getNodeView()->getSelectedItems();
    if (selectedItems.isEmpty()) {
        setDockEnabled(false);
        return;
    }

    bool multipleSelection = (selectedItems.count() > 1);
    bool readOnlyState = false;
    bool enableDock = true;

    // check if the dock should be disabled
    foreach (GraphMLItem* selectedItem, selectedItems) {

        if (!selectedItem->isNodeItem()) {
            continue;
        }

        NodeItem* item = (NodeItem*)selectedItem;
        QString itemKind = item->getNodeKind();

        if (getNotAllowedKinds().contains(itemKind)) {
            enableDock = false;
            break;
        }

        if (multipleSelection) {
            if (!getNodeView()->isNodeKindDeployable(itemKind)) {
                enableDock = false;
                break;
            }
        }

        if (!getNodeView()->isNodeKindDeployable(itemKind)) {
            readOnlyState = true;
        }
    }

    setDockEnabled(enableDock);

    if (enableDock) {
        foreach (DockNodeItem* dockItem, getDockNodeItems()) {
            dockItem->setReadOnlyState(readOnlyState);
        }
        highlightHardwareConnection(selectedItems);
    }
}



/**
 * @brief HardwareDockScrollArea::nodeConstructed
 * This gets called whenever a node has been constructed.
 * It checks to see if a dock item needs to be constucted for the new node.
 * @param nodeItem
 */
void HardwareDockScrollArea::nodeConstructed(NodeItem* nodeItem)
{
    if (!nodeItem->isEntityItem()) {
        return;
    }

    EntityItem* entityItem = (EntityItem*)nodeItem;

    if (entityItem->isHardwareCluster() || entityItem->isHardwareNode()) {

        DockNodeItem* dockItem = new DockNodeItem("", entityItem, this);
        insertDockNodeItem(dockItem);

        connect(this, SIGNAL(dock_highlightDockItem(NodeItem*)), dockItem, SLOT(highlightDockItem(NodeItem*)));
        connect(dockItem, SIGNAL(dockItem_relabelled(DockNodeItem*)), this, SLOT(insertDockNodeItem(DockNodeItem*)));

        // if the dock is open, update it
        if (isDockOpen()) {
            updateDock();
        }
    }
}


/**
 * @brief HardwareDockScrollArea::insertDockNodeItem
 * This method gets called every time a new dock item has been added to this dock.
 * It also gets called when one of the dock items is relabelled.
 * It keeps the dock items in this dock in alphabetical order.
 * @param dockItem
 */
void HardwareDockScrollArea::insertDockNodeItem(DockNodeItem* dockItem)
{
    if (getDockNodeItems().count() > 0) {

        // if the dock item has already been added to this dock,
        // remove it from the this dock's list and layout
        if (getDockNodeItems().contains(dockItem)) {
            removeDockNodeItem(dockItem);
        }

        QString dockItemLongName = dockItem->getKind() + dockItem->getLabel();

        // iterate through all the dock items and insert the provided dock item
        for (int i = 0; i < getLayout()->count(); i++) {
            DockNodeItem* currentItem = dynamic_cast<DockNodeItem*>(getLayout()->itemAt(i)->widget());
            if (currentItem){
                QString longName = currentItem->getKind() + currentItem->getLabel();
                int compare = dockItemLongName.compare(longName, Qt::CaseInsensitive);
                if (compare <= 0) {
                    addDockNodeItem(dockItem, i);
                    return;
                }
            }
        }
    }

    // if there's currently no item in this dock or the dock item
    // wasn't inserted, just add it to the end of the layout
    addDockNodeItem(dockItem);
}


/**
 * @brief HardwareDockScrollArea::highlightHardwareConnection
 * This method checks if the selected node is already connected to a hardware node.
 * If it is, highlight the corresponding dock node item.
 * If it's not, make sure none of the dock items is highlighted
 * @param selectedItems
 */
void HardwareDockScrollArea::highlightHardwareConnection(QList<GraphMLItem*> selectedItems)
{
    // if there are no deployable node items selected, clear highlighted items
    if (selectedItems.isEmpty()) {
        emit dock_highlightDockItem();
        return;
    }

    GraphMLItem* item = selectedItems[0];
    NodeItem* hardwareItem = getNodeView()->getDeployedNode(item->getID());

    if (selectedItems.count() == 1) {
        emit dock_highlightDockItem(hardwareItem);
        return;
    }

    for (int i = 1; i < selectedItems.count(); i++) {

        GraphMLItem* prevHardwareItem = hardwareItem;
        item = selectedItems[i];
        hardwareItem = getNodeView()->getDeployedNode(item->getID());

        if (hardwareItem != prevHardwareItem) {
            emit dock_highlightDockItem();
            return;
        }
    }

    emit dock_highlightDockItem(hardwareItem);
}
