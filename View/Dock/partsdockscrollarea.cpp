#include "partsdockscrollarea.h"
#include "docktogglebutton.h"
#include "docknodeitem.h"
#include <QDebug>

/**
 * @brief PartsDockScrollArea::PartsDockScrollArea
 * @param type
 * @param view
 * @param parent
 */

PartsDockScrollArea::PartsDockScrollArea(DOCK_TYPE type, NodeView *view, DockToggleButton *parent) :
    DockScrollArea(type, view, parent, "Selected entity cannot adopt any other entity.")
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
    kindsRequiringFunction.append("WorkerProcess");

    connectToView();
    connect(this, SIGNAL(dock_toggled(bool,QString)), SLOT(dockToggled(bool)));
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

    if (kindsToShow.isEmpty()) {
        // if there are no adoptable node kinds, disable the dock
        //setDockEnabled(false);
        //return;

        // if there are no adoptable node kinds, hide all the dock items
        setInfoText("The selected entity does not have any adoptable entities.");
        hideDockNodeItems();
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
 * @brief PartsDockScrollArea::dockToggled
 * @param open
 */
void PartsDockScrollArea::dockToggled(bool open)
{
    if (open) {
        updateDock();
    } else {
        closeAttachedDocks();
    }
}


/**
 * @brief PartsDockScrollArea::attachedDockToggled
 * @param open
 */
void PartsDockScrollArea::attachedDockToggled(bool open)
{
    DockScrollArea* senderDock = qobject_cast<DockScrollArea*>(QObject::sender());
    if (senderDock) {
        if (open) {
            openedDocks.append(senderDock);
        } else {
            openedDocks.removeAll(senderDock);
            // if an attached dock is closed, check if this dock should be reopened
            // or if a closed signal should be sent to the medea
            if (getParentButton() && getParentButton()->isSelected() && !isDockOpen()) {
                setDockOpen(true);
            } else {
                emit dock_toggled(false);
            }
        }
    }
}


/**
 * @brief PartsDockScrollArea::showMainDock
 */
void PartsDockScrollArea::showMainDock()
{
    closeAttachedDocks();
}


/**
 * @brief PartsDockScrollArea::hideDockNodeItems
 */
void PartsDockScrollArea::hideDockNodeItems()
{
    foreach (QString kind, displayedItems) {
        DockNodeItem* dockNodeItem = getDockNodeItem(kind);
        if (dockNodeItem) {
            dockNodeItem->setHidden(true);
        }
    }
    displayedItems.clear();
}


/**
 * @brief PartsDockScrollArea::closeAttachedDocks
 */
void PartsDockScrollArea::closeAttachedDocks()
{
    foreach (DockScrollArea* dock, openedDocks) {
        dock->setDockOpen(false);
    }
    openedDocks.clear();
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
 * @brief PartsDockScrollArea::attachDockSrollArea
 * @param dock
 */
void PartsDockScrollArea::attachDockSrollArea(DockScrollArea* dock)
{
    if (dock) {
        DOCK_TYPE dt = dock->getDockType();
        if (!attachedDocks.contains(dt)) {
            attachedDocks[dt] = dock;
            connect(dock, SIGNAL(dock_opened(bool)), this, SLOT(attachedDockToggled(bool)));
            connect(dock, SIGNAL(dock_closed(bool)), this, SLOT(attachedDockToggled(bool)));
            connect(dock, SIGNAL(dock_dockItemClicked()), this, SLOT(forceOpenDock()));
        }
    }
}


/**
 * @brief PartsDockScrollArea::kindRequiresDockSwitching
 * @param dockItemKind
 * @return
 */
DOCK_TYPE PartsDockScrollArea::kindRequiresDockSwitching(QString dockItemKind)
{
    if (kindsRequiringDefinition.contains(dockItemKind)) {
        return DEFINITIONS_DOCK;
    } else if (kindsRequiringFunction.contains(dockItemKind)) {
        return FUNCTIONS_DOCK;
    } else {
        return UNKNOWN_DOCK;
    }
}


/**
 * @brief PartsDockScrollArea::connectToView
 */
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
 * @brief PartsDockScrollArea::isDockOpen
 * @return
 */
bool PartsDockScrollArea::isDockOpen()
{
    bool dockOpen = DockScrollArea::isDockOpen();
    if (!dockOpen && openedDocks.count() > 0) {
        dockOpen = true;
    }
    return dockOpen;
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
    QString senderItemKind = sender->getKind();
    DOCK_TYPE dt = kindRequiresDockSwitching(senderItemKind);

    if (attachedDocks.contains(dt)) {
        setDockOpen(false);
        attachedDocks[dt]->forceOpenDock(senderItemKind);
        emit dock_toggled(true, senderItemKind);
    } else {
        getNodeView()->constructNode(senderItemKind, 0);
    }
}
