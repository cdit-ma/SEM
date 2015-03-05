#include "definitionsdockscrollarea.h"
#include "docknodeitem.h"

#include <QDebug>


/**
 * @brief DefinitionsDockScrollArea::DefinitionsDockScrollArea
 * @param label
 * @param view
 * @param parent
 */
DefinitionsDockScrollArea::DefinitionsDockScrollArea(QString label, NodeView* view, DockToggleButton *parent) :
    DockScrollArea(label, view, parent) {}


/**
 * @brief DefinitionsDockScrollArea::updateDock
 * This updates the Component definitions dock.
 */
void DefinitionsDockScrollArea::updateDock()
{
    // do updates to deifinitions dock here
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
