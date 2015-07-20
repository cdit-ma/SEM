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
    //hardware_notAllowedKinds.append("BlackBoxInstance");

    setNotAllowedKinds(hardware_notAllowedKinds);
}


/**
 * @brief HardwareDockScrollArea::onNodeDeleted
 * @param ID
 */
void HardwareDockScrollArea::onNodeDeleted(QString ID)
{
    DockScrollArea::onNodeDeleted(ID);
}


/**
 * @brief HardwareDockScrollArea::onEdgeDeleted
 */
void HardwareDockScrollArea::onEdgeDeleted()
{
    refreshDock();
}


/**
 * @brief HardwareDockScrollArea::dock_itemClicked
 * When an item in this dock is clicked, if it's allowed, it connects the
 * selected node to the selected hardware node from the dock.
 */
void HardwareDockScrollArea::dockNodeItemClicked()
{
    DockNodeItem* dockNodeItem = dynamic_cast<DockNodeItem*>(sender());
    if (!dockNodeItem) {
        return;
    }

    QList<NodeItem*> selectedNodeItems = getNodeView()->getSelectedNodeItems();
    QStringList selectedNodeIDS = getNodeView()->getSelectedNodeIDs();

    QString dockNodeID = dockNodeItem->getID();

    foreach (NodeItem* selectedNodeItem, selectedNodeItems) {
        if (selectedNodeItem) {
            QString selectedNodeKind = selectedNodeItem->getNodeKind();
            if (!getNodeView()->isNodeKindDeployable(selectedNodeKind)) {
                return;
            }
        } else {
            //This shouldn't happen
            qCritical() << "Null NodeItem in HardwareDock";
            return;
        }
    }

    //At this point everything in selectedNodeIDs is deployable.
    //If all nodes in selection are already connected to dockNodeID, disconnect them.
    //If some nodes in selection aren't connected to dockNodeID, disconnect their deployment edge, and connect to docknodeID.
    getNodeView()->constructDestructEdges(selectedNodeIDS, dockNodeID);
}


/**
 * @brief DefinitionsDockScrollArea::updateDock
 * This is called whenever a node item is selected.
 * It checks to see if this dock should be enabled for the currently selected item.
 */
void HardwareDockScrollArea::updateDock()
{    
    QList<NodeItem*> selectedItems = getNodeView()->getSelectedNodeItems();
    if (selectedItems.isEmpty()) {
        setDockEnabled(false);
        return;
    }

    // if the dock is disabled, there is no need to update
    foreach (NodeItem* item, selectedItems) {
        QString itemKind = item->getNodeKind();
        if (getNotAllowedKinds().contains(itemKind)) {
            setDockEnabled(false);
            return;
        }
        if (itemKind == "ComponentInstance") {
            if (!item->getNode()->getDefinition()) {
                setDockEnabled(false);
                return;
            }
        }
    }

    setDockEnabled(true);
    highlightHardwareConnection(selectedItems);
}



/**
 * @brief HardwareDockScrollArea::nodeConstructed
 * This gets called whenever a node has been constructed.
 * It checks to see if a dock item needs to be constucted for the new node.
 * @param nodeItem
 */
void HardwareDockScrollArea::nodeConstructed(NodeItem *nodeItem)
{
    if (nodeItem->getNodeKind() == "HardwareNode" || nodeItem->getNodeKind() == "HardwareCluster") {
        DockNodeItem* dockItem = new DockNodeItem("", nodeItem, this);
        insertDockNodeItem(dockItem);
        connect(this, SIGNAL(dock_higlightDockItem(NodeItem*)), dockItem, SLOT(highlightDockItem(NodeItem*)));
        connect(dockItem, SIGNAL(dockItem_relabelled(DockNodeItem*)), this, SLOT(insertDockNodeItem(DockNodeItem*)));

        // if the dock is open, refresh it
        if (isDockEnabled()) {
            refreshDock();
        }
    }
}


/**
 * @brief HardwareDockScrollArea::refreshDock
 */
void HardwareDockScrollArea::refreshDock()
{
    // if a node was destructed and it was previously selected, clear this dock's selection
    if (!getNodeView()->getSelectedNodeItem()) {
        DockScrollArea::updateCurrentNodeItem();
    }
    //emit dock_higlightDockItem();
    updateDock();
}


/**
 * @brief HardwareDockScrollArea::insertDockNodeItem
 * This method gets called every time a new dock item has been added to this dock.
 * It also gets called when one of the dock items is relabelled.
 * It keeps the dock items in this dock in alphabetical order.
 * @param dockItem
 */
void HardwareDockScrollArea::insertDockNodeItem(DockNodeItem *dockItem)
{
    if (getDockNodeItems().count() > 0) {

        // if the dock item has already been added to this dock,
        // remove it from the this dock's list and layout
        if (getDockNodeItems().contains(dockItem)) {
            //removeDockNodeItemFromList(dockItem);
            getLayout()->removeWidget(dockItem);
        }

        QString dockItemLabel = dockItem->getLabel();

        // iterate through all the dock items and insert the provided dock item
        //for (int i = 0; i < getDockNodeItems().count(); i++) {
        for (int i = 0; i < getLayout()->count(); i++) {
            DockNodeItem* currentItem = dynamic_cast<DockNodeItem*>(getLayout()->itemAt(i)->widget());
            if (currentItem) {
                QString currentLabel = currentItem->getLabel();
                int compare = dockItemLabel.compare(currentLabel, Qt::CaseInsensitive);
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
void HardwareDockScrollArea::highlightHardwareConnection(QList<NodeItem*> selectedItems)
{
    // if there are no deployable node items selected, clear highlighted items
    if (selectedItems.isEmpty()) {
        emit dock_higlightDockItem();
        return;
    }

    NodeItem* item = selectedItems[0];
    NodeItem* hardwareItem = getNodeView()->getDeployedNode(item->getID());

    if (selectedItems.count() == 1) {
        emit dock_higlightDockItem(hardwareItem);
        return;
    }

    for (int i = 1; i < selectedItems.count(); i++) {

        NodeItem* prevHardwareItem = hardwareItem;
        item = selectedItems[i];
        hardwareItem = getNodeView()->getDeployedNode(item->getID());

        if (hardwareItem != prevHardwareItem) {
            emit dock_higlightDockItem();
            return;
        }
    }

    emit dock_higlightDockItem(hardwareItem);
}
