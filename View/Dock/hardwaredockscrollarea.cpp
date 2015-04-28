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
        // are there any other kinds I should care about?
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

        // special case - ComponentInstance
        // it's only an allowed kind if it has a definition
        if (nodeKind == "ComponentInstance") {
            if (!node->getDefinition()) {
                getParentButton()->enableDock(false);
            }
        }

        if (getParentButton()->isEnabled()) {
            if (nodeKind == "ComponentAssembly" || nodeKind == "ComponentInstance") {
                // check if selected node is already connected to a hardware node
                // if it is, highlight corresponding hardware dock node item
                Edge* hardwareEdge = getHardwareConnection(node);
                if (hardwareEdge) {
                    emit dock_higlightDockItem(hardwareEdge->getDestination());
                }
            }
        }
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
    }
}


/**
 * @brief HardwareDockScrollArea::getHardwareConnection
 * @param selectedNode
 * @return
 */
Edge* HardwareDockScrollArea::getHardwareConnection(Node *selectedNode)
{
    foreach (Edge *edge, selectedNode->getEdges()) {
        if (edge->getDestination()->getDataValue("kind") == "HardwareNode") {
            return edge;
        }
    }
    return 0;
}
