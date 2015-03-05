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

    // intially add all kinds of nodes to this dock
    // hide them when they're not adoptable; don't delete them
    //addDockNodeItems(getConstructableNodeKindsFromView());
}


/**
 * @brief PartsDockScrollArea::updateDock
 * This updates the Parts dock.
 * This checks the new adoptable nodes list to see which dock node
 * items need to either be hidden or shown from this dock.
 */
void PartsDockScrollArea::updateDock()
{
    // this forcefully updates the parts dock node items list
    //addDockNodeItems(getAdoptableNodeListFromView());

    /*
    // check to see if current node item can adopt nodes
    QString nodeKind = nodeItem->getNodeKind();
    if (!allowedKinds.contains(nodeKind)) {
        getParentButton()->setEnabled(false);
        getParentButton()->checkEnabled();
    }
    */

    /*
    // when the selected node can't adopt anything,
    // disbale parentButton and hide this dock
    if (getDockNodeItems().count() == 0) {
        getParentButton()->setEnabled(false);
        getParentButton()->checkEnabled();
    }
    */

    QStringList itemsToDisplay = getAdoptableNodeListFromView();
    QStringList newDisplayedItems;

    // check if items from new list are already displayed
    for (int i = 0; i < displayedItems.count(); i++) {
        QString item = displayedItems.at(i);
        // if it is, remove it from the list of itemsToDisplay
        // otherwise, hide the dock node item
        if (itemsToDisplay.contains(item)) {
            itemsToDisplay.removeAll(item);
            newDisplayedItems.append(item);
        } else {
            getDockNodeItem(item)->hide();
        }
    }

    // show all dock node items with kind in itemsToDisplay
    for (int i = 0; i < itemsToDisplay.count(); i++) {
        getDockNodeItem(itemsToDisplay.at(i))->show();
        newDisplayedItems.append(itemsToDisplay.at(i));
    }

    // update list of currently displayed dock node items
    displayedItems = newDisplayedItems;
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
    nodeKinds.removeDuplicates();
    nodeKinds.sort();

    for (int i = 0; i < nodeKinds.count(); i++) {
        DockNodeItem *item = new DockNodeItem(nodeKinds.at(i), 0, this);
        addDockNodeItem(item);
    }

    // initialise list of displayed items
    displayedItems = nodeKinds;

    //checkScrollBar();
    //checkDockNodesList();
    repaint();
}


/**
 * @brief PartsDockScrollArea::getDockNodeItem
 * Go through this dock's item list and find the item with the specified kind.
 * @param kind
 * @return
 */
DockNodeItem* PartsDockScrollArea::getDockNodeItem(QString kind)
{
    foreach (DockNodeItem* item, getDockNodeItems()) {
        if (item->getKind() == kind) {
            return item;
        }
    }
    return 0;
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
