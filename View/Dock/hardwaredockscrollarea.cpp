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

    component = 0;
}


/**
 * @brief HardwareDockScrollArea::dock_itemClicked
 * When an item in this dock is clicked, if it's allowed, it connects the
 * selected node to the selected hardware node from the dock.
 */
void HardwareDockScrollArea::dockNodeItemClicked()
{
    qDebug() << "dock clicked";
    DockNodeItem* sender = qobject_cast<DockNodeItem*>(QObject::sender());
    Node* selectedNode;
    if (component) {
        selectedNode = component;
        component = 0;
    } else {
        selectedNode = getNodeView()->getSelectedNode();
    }

    if (selectedNode) {
        QString nodeKind = selectedNode->getDataValue("kind");
        // are there any other kinds I should care about?
        if (nodeKind == "ComponentAssembly" || nodeKind == "ComponentInstance") {
            foreach (Edge *edge, selectedNode->getEdges()) {
                if (edge->getDestination() != sender->getNodeItem()->getNode()) {
                    emit dock_destructEdge(edge);
                }
            }
            getNodeView()->view_ConstructEdge(getNodeView()->getSelectedNode(), sender->getNodeItem()->getNode());
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

    // special case - ComponentInstance
    // it's only an allowed kind if it has a definition
    if (getCurrentNodeItem() && getCurrentNodeItem()->getNodeKind() == "ComponentInstance") {
        Node* inst = getCurrentNodeItem()->getNode();
        if (!inst->getDefinition()) {
            getParentButton()->enableDock(false);
        }
        return;
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
    }
}


/**
 * @brief HardwareDockScrollArea::clickHardwareDockItem
 * @param hardwareNode
 */
void HardwareDockScrollArea::clickHardwareDockItem(Node *component, NodeItem *hardwareNodeItem)
{
    qDebug() << "HELLO";
    DockNodeItem* dockItem = getDockNodeItem(hardwareNodeItem);
    qDebug() << "HELLO2";
    if (dockItem) {
        this->component = component;
        dockItem->clicked();
    }
}
