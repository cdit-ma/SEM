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
    hardware_notAllowedKinds.append("File");
    hardware_notAllowedKinds.append("Component");
    hardware_notAllowedKinds.append("ComponentImpl");
    hardware_notAllowedKinds.append("HardwareNode");
    setNotAllowedKinds(hardware_notAllowedKinds);
}


/**
 * @brief HardwareDockScrollArea::dock_itemClicked
 * When an item in this dock is clicked, if it's allowed, it connects the
 * selected node to the selected hardware node from the dock.
 */
void HardwareDockScrollArea::dockNodeItemClicked()
{
    DockNodeItem* sender = qobject_cast<DockNodeItem*>(QObject::sender());
    Node* senderNode = sender->getNodeItem()->getNode();
    Node* selectedNode = getNodeView()->getSelectedNode();

    if (selectedNode) {

        QString nodeKind = selectedNode->getDataValue("kind");

        // NOTE: Are there any other kinds I should care about?
        if (nodeKind == "ComponentAssembly" || nodeKind == "ComponentInstance") {

            Edge* hardwareEdge = getHardwareConnection(selectedNode);

            // check if the selected node is already connected to a hardware node
            // if it is, check if the user is trying to connect to a different node
            if (hardwareEdge) {
                if (hardwareEdge->getDestination() != senderNode) {
                    getNodeView()->view_TriggerAction("Deploying Component on Node");
                    emit dock_destructEdge(hardwareEdge);
                } else {
                    // trying to connect to the same node; do nothing
                    return;
                }
            }

            getNodeView()->constructEdge(selectedNode, senderNode, false);
        }
    }
}


/**
 * @brief DefinitionsDockScrollArea::updateDock
 * This is called whenever a node item is selected.
 * It checks to see if this dock should be enabled for the currently selected item.
 */
void HardwareDockScrollArea::updateDock()
{
    DockScrollArea::updateDock();

    if (getCurrentNodeItem()) {

        QString nodeKind = getCurrentNodeItem()->getNodeKind();
        Node* node = getCurrentNodeItem()->getNode();

        // check for special case - ComponentInstance
        // it's only an allowed kind if it has a definition
        if (nodeKind == "ComponentInstance") {
            if (!node->getDefinition()) {
                getParentButton()->enableDock(false);
                return;
            }
        }

        // check if any of the hardware dock items should be highlighted
        highlightHardwareConnection();
    }
}

void HardwareDockScrollArea::edgeDeleted(QString srcID, QString dstID)
{
    if(getSelectedNodeID() == srcID || getSelectedNodeID() == dstID){
        updateDock();
    }
}


/**
 * @brief HardwareDockScrollArea::nodeConstructed
 * This gets called whenever a node has been constructed.
 * It checks to see if a dock item needs to be constucted for the new node.
 * @param nodeItem
 */
void HardwareDockScrollArea::nodeConstructed(NodeItem *nodeItem)
{
    if (nodeItem->getNodeKind() == "HardwareNode") {
        DockNodeItem* dockItem = new DockNodeItem("", nodeItem, this);
        insertDockNodeItem(dockItem);
        nodeItem->setHidden(true);
        connect(this, SIGNAL(dock_higlightDockItem(Node*)), dockItem, SLOT(highlightDockItem(Node*)));
        connect(dockItem, SIGNAL(dockItem_relabelled(DockNodeItem*)), this, SLOT(insertDockNodeItem(DockNodeItem*)));
    }
}


/**
 * @brief HardwareDockScrollArea::refreshDock
 */
void HardwareDockScrollArea::refreshDock()
{
    // if a node was destructed and it was previously selected, clear this dock's selection
    if (!getNodeView()->getSelectedNode()) {
        DockScrollArea::updateCurrentNodeItem();
    }
    emit dock_higlightDockItem();
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
 * @brief HardwareDockScrollArea::getHardwareConnection
 * @param selectedNode
 * @return
 */
Edge* HardwareDockScrollArea::getHardwareConnection(Node *selectedNode)
{
    foreach (Edge *edge, selectedNode->getEdges(0)) {
        if (edge->isDeploymentLink()) {
            return edge;
        }
    }
    return 0;
}


/**
 * @brief HardwareDockScrollArea::highlightHardwareConnection
 * This method checks if the selected node is already connected to a hardware node.
 * If it is, highlight the corresponding dock node item.
 * If it's not, make sure none of the dock items is highlighted
 */
void HardwareDockScrollArea::highlightHardwareConnection()
{
    NodeItem* selectedItem = getCurrentNodeItem();

    // we only care if there is a selected item and the Hardware dock is enabled
    if (selectedItem && getParentButton()->isEnabled()) {

        //qDebug() << "highlightHardwareConnection";

        QString nodeKind = selectedItem->getNodeKind();

        // NOTE: Is there any other kinds that can be connected to a Harware Node/Cluster?
        if (nodeKind == "ComponentAssembly" || nodeKind == "ComponentInstance") {
            Edge* hardwareEdge = getHardwareConnection(selectedItem->getNode());
            if (hardwareEdge) {
                //qDebug() << "Highlight item";
                emit dock_higlightDockItem(hardwareEdge->getDestination());
            } else {
                //qDebug() << "Remove highlight";
                emit dock_higlightDockItem();
            }
        } else {
            // remove higlight for any other kinds
            emit dock_higlightDockItem();
        }
    }
}
