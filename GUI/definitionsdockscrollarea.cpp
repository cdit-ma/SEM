#include "definitionsdockscrollarea.h"
#include "docktogglebutton.h"
#include "docknodeitem.h"

#include <QDebug>


/**
 * @brief DefinitionsDockScrollArea::DefinitionsDockScrollArea
 * @param label
 * @param view
 * @param parent
 */
DefinitionsDockScrollArea::DefinitionsDockScrollArea(QString label, NodeView* view, DockToggleButton *parent) :
    DockScrollArea(label, view, parent)
{
    // populate list of not allowed kinds
    definitions_notAllowedKinds.append("Model");
    definitions_notAllowedKinds.append("InterfaceDefinitions");
    definitions_notAllowedKinds.append("BehaviourDefinitions");
    definitions_notAllowedKinds.append("DeploymentDefinitions");
    definitions_notAllowedKinds.append("AssemblyDefinitions");
    definitions_notAllowedKinds.append("HardwareDefinitions");
    definitions_notAllowedKinds.append("ManagementComponent");
    definitions_notAllowedKinds.append("HardwareCluster");
    definitions_notAllowedKinds.append("File");
    definitions_notAllowedKinds.append("Component");
    setNotAllowedKinds(definitions_notAllowedKinds);
}


/**
 * @brief DefinitionsDockScrollArea::dock_itemClicked
 * When an item in this dock is clicked, it tries to either add a ComponentInstance
 * or connect a ComponentInstance to an existing Component if that action is allowed.
 */
void DefinitionsDockScrollArea::dockNodeItemClicked()
{
    Node* selectedNode = getNodeView()->getSelectedNode();
    DockNodeItem* sender = qobject_cast<DockNodeItem*>(QObject::sender());

    if (selectedNode) {
        Node* dockNode = sender->getNodeItem()->getNode();
        if (selectedNode->getDataValue("kind") == "ComponentAssembly") {
            getNodeView()->view_constructComponentInstance(selectedNode, dockNode, 0);
        } else if (selectedNode->getDataValue("kind") == "ComponentInstance") {
            getNodeView()->view_constructEdge(selectedNode,  dockNode);
        }
    }
}


/**
 * @brief DefinitionsDockScrollArea::updateDock
 */
void DefinitionsDockScrollArea::updateDock()
{
    // special case - ComponentInstance
    // it's only an allowed kind if it doesn't have a definition
    if (getCurrentNodeItem() && getCurrentNodeItem()->getNodeKind() == "ComponentInstance") {
        Node* inst = getCurrentNodeItem()->getNode();
        if (inst->getDefinition()) {
            getParentButton()->enableDock(false);
        }
        return;
    }
    DockScrollArea::updateDock();
}


/**
 * @brief DefinitionsDockScrollArea::nodeConstructed
 * This gets called whenever a node has been constructed.
 * It checks to see if a dock item needs to be constucted for the new node.
 * @param node
 */
void DefinitionsDockScrollArea::nodeConstructed(NodeItem *nodeItem)
{
   if (nodeItem->getNodeKind() == "Component") {
       DockNodeItem* dockItem = new DockNodeItem("", nodeItem, this);
       addDockNodeItem(dockItem);
   }
}
