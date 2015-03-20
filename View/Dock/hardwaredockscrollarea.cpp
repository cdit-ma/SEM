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
    getNodeView()->view_ConstructEdge(getNodeView()->getSelectedNode(), sender->getNodeItem()->getNode());
}


/**
 * @brief DefinitionsDockScrollArea::updateDock
 */
void HardwareDockScrollArea::updateDock()
{
    // special case - ComponentInstance
    // it's only an allowed kind if it has a definition
    if (getCurrentNodeItem() && getCurrentNodeItem()->getNodeKind() == "ComponentInstance") {
        Node* inst = getCurrentNodeItem()->getNode();
        if (!inst->getDefinition()) {
            getParentButton()->enableDock(false);
        }
        return;
    }

    DockScrollArea::updateDock();
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
