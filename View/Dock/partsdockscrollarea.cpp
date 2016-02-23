#include "partsdockscrollarea.h"
#include "docktogglebutton.h"
#include "docknodeitem.h"
#include <QDebug>

/**
 * @brief PartsDockScrollArea::PartsDockScrollArea
 * @param label
 * @param view
 * @param parent
 */
PartsDockScrollArea::PartsDockScrollArea(QString label, NodeView *view, DockToggleButton *parent) :
    DockScrollArea(label, view, parent, "Selected entity cannot adopt any other entity.")
{
    kindsRequiringDefinition.append("BlackBoxInstance");
    kindsRequiringDefinition.append("ComponentInstance");
    kindsRequiringDefinition.append("ComponentImpl");
    kindsRequiringDefinition.append("AggregateInstance");
    kindsRequiringDefinition.append("VectorInstance");
    kindsRequiringDefinition.append("InEventPort");
    kindsRequiringDefinition.append("OutEventPort");
    kindsRequiringDefinition.append("InEventPortDelegate");
    kindsRequiringDefinition.append("OutEventPortDelegate");
    kindsRequiringDefinition.append("OutEventPortImpl");
    kindsRequiringFunction.append("Process");

    setDockEnabled(false);
    connectToView();
}


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

    // if there are no adoptable node kinds, disable the dock
    if (kindsToShow.isEmpty()) {
        setDockEnabled(false);
        return;
    }

    // for each kind to show, check if it is already displayed
    // if it is, remove it from the list of kinds to show
    // if it's not, kind shouldn't be displayed anymore; hide it
    foreach (QString kind, displayedItems) {
        if (kindsToShow.contains(kind)) {
            kindsToShow.removeAll(kind);
        } else {
            DockNodeItem* dockNodeItem = getDockNodeItem(kind);
            if (dockNodeItem) {
                dockNodeItem->setHidden(true);
                displayedItems.removeAll(kind);
            }
        }
    }

    // show remaining kinds that aren't already displayed
    // and add it to the displayed kinds list
    foreach (QString kind, kindsToShow) {
        DockNodeItem* dockNodeItem = getDockNodeItem(kind);
        if (dockNodeItem) {
            dockNodeItem->setHidden(false);
            displayedItems.append(kind);
        }
    }
}


/**
 * @brief PartsDockScrollArea::clear
 */
void PartsDockScrollArea::clear()
{
    DockScrollArea::clear();
    displayedItems.clear();
}


/**
 * @brief PartsDockScrollArea::forceOpenDock
 */
void PartsDockScrollArea::forceOpenDock()
{
    if (isDockOpen()) {
        return;
    }
    if (!isDockEnabled()) {
        setDockEnabled(true);
    }

    // close the sender dock then open this dock
    DockScrollArea* dock = qobject_cast<DockScrollArea*>(QObject::sender());
    dock->setDockOpen(false);
    setDockOpen();
    updateDock();
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
            DockNodeItem* dockNodeItem = new DockNodeItem(kind, 0, this);
            addDockNodeItem(dockNodeItem);
        }
    }

    // initialise list of displayed items
    displayedItems = nodeKinds;
}


/**
 * @brief PartsDockScrollArea::kindRequiresDockSwitching
 * @param dockItemKind
 * @return
 */
bool PartsDockScrollArea::kindRequiresDockSwitching(QString dockItemKind)
{
    return kindsRequiringDefinition.contains(dockItemKind) || kindsRequiringFunction.contains(dockItemKind);
}

void PartsDockScrollArea::connectToView()
{
    NodeView* view = getNodeView();
    if (view) {
        connect(view, SIGNAL(view_nodeSelected()), this, SLOT(updateCurrentNodeItem()));
        connect(view, SIGNAL(view_nodeConstructed(NodeItem*)), this, SLOT(updateDock()));
        connect(view, SIGNAL(view_nodeDeleted(int, int)), this, SLOT(onNodeDeleted(int, int)));
    }
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
    QString nodeKind = sender->getKind();
    if (kindsRequiringDefinition.contains(nodeKind)) {
        emit dock_forceOpenDock(nodeKind);
    } else if (kindsRequiringFunction.contains(nodeKind)) {
        emit dock_forceOpenDock();
    } else {
        getNodeView()->constructNode(nodeKind, 0);
    }
}
