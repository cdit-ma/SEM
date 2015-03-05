#include "partsdockscrollarea.h"
#include "docktogglebutton.h"
#include "docknodeitem.h"

#include <QDebug>


/**
 * @brief PartsDockScrollArea::PartsDockScrollArea
 * @param label
 * @param parent
 */
PartsDockScrollArea::PartsDockScrollArea(QString label, NodeView *view, DockToggleButton *parent) :
    DockScrollArea(label, view, parent)
{
    // populate allowed node kinds
    allowedKinds.append("InterfaceDefinitions");
    allowedKinds.append("BehaviourDefinitions");
    allowedKinds.append("AssemblyDefinitions");
    allowedKinds.append("File");
    allowedKinds.append("Component");
    allowedKinds.append("ComponentAssembly");
    allowedKinds.append("ComponentImpl");
}


/**
 * @brief PartsDockScrollArea::updateDock
 * This updates the Parts dock.
 */
void PartsDockScrollArea::updateDock()
{
    // this forcefully updates the parts dock node items list
    //addDockNodeItems(getAdoptableNodeListFromView());

    /*
    QString nodeKind = nodeItem->getNodeKind();
    if () {

    }
    */

    // when the selected node can't adopt anything,
    // disbale parentButton and hide this dock
    if (getDockNodeItems().count() == 0) {
        getParentButton()->setEnabled(false);
        getParentButton()->checkEnabled();
    }
}


/**
 * @brief PartsDockScrollArea::addDockNodeItems
 * Add adoptable dock node items to dock.
 * This creates a groupbox for each adoptable node kind
 * which contains the new dock node item and its label.
 * @param nodeKinds
 */
void PartsDockScrollArea::addDockNodeItems(QStringList nodeKinds)
{
    // TODO: maybe create all kinds of node and just hide them
    // instead of deleting them when they can't be adopted
    clear();
    nodeKinds.sort();

    for (int i = 0; i < nodeKinds.count(); i++) {
        DockNodeItem *item = new DockNodeItem(nodeKinds.at(i), 0, this);
        addDockNodeItem(item);
    }

    //checkScrollBar();
    //checkDockNodesList();
    //repaint();
}


/**
 * @brief PartsDockScrollArea::dockNodeItemPressed
 * This gets called when a dock adoptable node item is clicked.
 * It tells the view to create a NodeItem with the specified
 * kind inside the currently selected node.
 */
void PartsDockScrollArea::dockNodeItemClicked()
{
    DockNodeItem* sender = qobject_cast<DockNodeItem*>(QObject::sender());
    getNodeView()->view_constructNode(sender->getKind(), 0);
}
