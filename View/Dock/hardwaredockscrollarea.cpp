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
            if (hardwareEdge && hardwareEdge->getDestination() != senderNode) {
                emit dock_destructEdge(hardwareEdge);
            }
            getNodeView()->view_ConstructEdge(selectedNode, senderNode);
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
        addDockNodeItem(dockItem);
        nodeItem->setHidden(true);
        connect(this, SIGNAL(dock_higlightDockItem(Node*)), dockItem, SLOT(highlightDockItem(Node*)));

        // when undoing - in case an edge was reconstructed before the hardware node
        //highlightHardwareConnection();
    }
}


/**
 * @brief HardwareDockScrollArea::nodeDestructed
 * @param nodeItem
 */
void HardwareDockScrollArea::nodeDestructed(NodeItem *nodeItem)
{

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

        QString nodeKind = selectedItem->getNodeKind();

        // NOTE: Is there any other kinds that can be connected to a Harware Node/Cluster?
        if (nodeKind == "ComponentAssembly" || nodeKind == "ComponentInstance") {
            Edge* hardwareEdge = getHardwareConnection(selectedItem->getNode());
            if (hardwareEdge) {
                emit dock_higlightDockItem(hardwareEdge->getDestination());
            } else {
                emit dock_higlightDockItem();
            }
        }
    }
}
