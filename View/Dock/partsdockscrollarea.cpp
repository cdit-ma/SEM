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
    DockScrollArea(label, view, parent) {}


/**
 * @brief PartsDockScrollArea::updateDock
 * This is called whenever a node item is selected, constructed or destructed.
 * This checks the new adoptable nodes list to see which dock node
 * items need to either be hidden or shown from this dock.
 */
void PartsDockScrollArea::updateDock()
{
    // this will enable/disable the dock depending on whether there's a selected item
    DockScrollArea::updateDock();

    // if the dock is disabled, there is no need to update
    if (!isDockEnabled()) {
        return;
    }

    QStringList kindsToShow = getAdoptableNodeListFromView();

    foreach(QString kind, displayedItems){
        if(kindsToShow.contains(kind)){
            kindsToShow.removeAll(kind);
        }else{
            DockNodeItem* dockNodeItem = getDockNodeItem(kind);
            if(dockNodeItem){
                dockNodeItem->hide();
                displayedItems.removeAll(kind);
            }else{
                qCritical() << "Dont have item" << kind;
            }
        }
    }

    //Got nothing to show. So Hide the Dock!
    if (displayedItems.isEmpty() && kindsToShow.isEmpty()) {
        setDockEnabled(false);
    } else {
        foreach (QString kind, kindsToShow) {
            DockNodeItem* dockNodeItem = getDockNodeItem(kind);
            if (dockNodeItem) {
                dockNodeItem->show();
                displayedItems.append(kind);
            } else {
                qCritical() << "Dont have item" << kind;
            }
        }
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
    nodeKinds.removeDuplicates();
    nodeKinds.sort();

    foreach (QString kind, nodeKinds) {
        if (!getDockNodeItem(kind)) {

        }
        DockNodeItem* dockNodeItem = new DockNodeItem(kind, 0, this);
        addDockNodeItem(dockNodeItem);
    }

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
    //DockNodeItems are indexed by their kind for the parts list.
    return DockScrollArea::getDockNodeItem(kind);
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
    getNodeView()->constructNode(sender->getKind(), 0);
}
