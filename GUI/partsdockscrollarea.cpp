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
    // populate list of not allowed kinds
    parts_notAllowedKinds.append("Model");
    parts_notAllowedKinds.append("DeploymentDefinitions");
    parts_notAllowedKinds.append("HardwareDefinitions");
    parts_notAllowedKinds.append("ManagementComponent");
    parts_notAllowedKinds.append("HardwareCluster");
    parts_notAllowedKinds.append("ComponentInstance");
    setNotAllowedKinds(parts_notAllowedKinds);
}


/**
 * @brief PartsDockScrollArea::updateDock
 * This checks the new adoptable nodes list to see which dock node
 * items need to either be hidden or shown from this dock.
 */
void PartsDockScrollArea::updateDock()
{
    /*
    // when the selected node can't adopt anything,
    // disbale parentButton and hide this dock
    if (getDockNodeItems().count() == 0) {
        getParentButton()->enableDock(false);
        return;
    }
    */

    // check selected node kind against notAllowedKinds list first
    DockScrollArea::updateDock();
    if (!getParentButton()->isEnabled()) {
        return;
    }

    /*
     * This doesn't prune the list; it may show an item that is already displayed.
     * Not sure if hide/show/setVisible are heavy calls.
     *
    QStringList itemsToDisplay = getAdoptableNodeListFromView();

    // compare the list of itemsToDisplay against the list of displayedItems
    // if item in displayedItems is not contained in itemsToDisplay, hide it
    for (int i = 0; i < displayedItems.count(); i++) {
        QString item = displayedItems.at(i);
        if (!itemsToDisplay.contains(item)) {
            getDockNodeItem(item)->hide();
        }
    }

    // show all the hidden dock node items with kind in itemsToDisplay
    for (int i = 0; i < itemsToDisplay.count(); i++) {
        getDockNodeItem(itemsToDisplay.at(i))->show();
    }

    // update list of currently displayed dock node items
    displayedItems = itemsToDisplay;
    */

    // NOTE: AdoptableNodeList from view is incorrect when deleteing node using undo.

    QStringList itemsToDisplay = getAdoptableNodeListFromView();
    QStringList newDisplayedItems;

    // compare the list of itemsToDisplay against the list of displayedItems
    for (int i = 0; i < displayedItems.count(); i++) {
        QString item = displayedItems.at(i);
        // if item in displayedItems is contained in itemsToDisplay, remove it from itemsToDisplay
        // otherwise, item shouldn't be displayed anymore; hide it
        if (itemsToDisplay.contains(item)) {
            itemsToDisplay.removeAll(item);
            newDisplayedItems.append(item);
        } else {
            getDockNodeItem(item)->hide();
        }
    }

    // show all the hidden dock node items with kind in itemsToDisplay
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
    clear();
    nodeKinds.removeDuplicates();
    nodeKinds.sort();

    for (int i = 0; i < nodeKinds.count(); i++) {
        DockNodeItem *item = new DockNodeItem(nodeKinds.at(i), 0, this);
        addDockNodeItem(item);
    }

    //checkScrollBar();
    repaint();

    // initialise list of displayed items
    displayedItems = nodeKinds;
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
