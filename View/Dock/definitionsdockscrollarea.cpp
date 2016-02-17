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
DefinitionsDockScrollArea::DefinitionsDockScrollArea(QString label, NodeView* view, DockToggleButton* parent) :
    DockScrollArea(label, view, parent, "No entity of the following kinds has been constructed: <br/>Aggregate, BlackBox, Component and Vector")
{
    // this is the list of entity kinds that this dock constructs items for
    definitionKinds.append("IDL");
    definitionKinds.append("Component");
    definitionKinds.append("BlackBox");
    definitionKinds.append("Aggregate");
    definitionKinds.append("Vector");
    definitionKinds.append("OutEventPort");

    // setup definitions-dock specific layout
    mainLayout = new QVBoxLayout();
    itemsLayout = new QVBoxLayout();
    mainLayout->addLayout(itemsLayout);
    mainLayout->addStretch();
    getLayout()->addLayout(mainLayout);

    setDockOpen(false);
    connectToView();

    connect(this, SIGNAL(dock_opened(bool)), this, SLOT(dockToggled(bool)));
    connect(this, SIGNAL(dock_closed(bool)), this, SLOT(dockToggled(bool)));
    connect(this, SIGNAL(dock_closed()), this, SLOT(dockClosed()));
}


/**
 * @brief DefinitionsDockScrollArea::getDockNodeItems
 * This returns the list of existing dock node items; not including dock item labels.
 * @return
 */
QList<DockNodeItem*> DefinitionsDockScrollArea::getDockNodeItems()
{
    QList<DockNodeItem*> dockItems = DockScrollArea::getDockNodeItems();
    QList<DockNodeItem*> dockNodeItems;

    foreach (DockNodeItem* item, dockItems) {
        if (!item->isDockItemLabel()) {
            dockNodeItems.append(item);
        }
    }

    return dockNodeItems;
}


/**
 * @brief DefinitionsDockScrollArea::getDockItemsOfKind
 * @param nodeKind
 * @return
 */
QList<DockNodeItem*> DefinitionsDockScrollArea::getDockItemsOfKind(QString nodeKind)
{
    QList<DockNodeItem*> itemsOfKind;

    if (nodeKind.isEmpty()) {
        return itemsOfKind;
    }

    // only show the dock node items with the specified kind
    foreach (DockNodeItem* dockItem, getDockNodeItems()) {
        if (dockItem->getKind() == nodeKind) {
            itemsOfKind.append(dockItem);
        }
    }

    return itemsOfKind;
}


/**
 * @brief DefinitionsDockScrollArea::connectToView
 */
void DefinitionsDockScrollArea::connectToView()
{
    NodeView* view = getNodeView();
    if (view) {
        connect(view, SIGNAL(view_nodeSelected()), this, SLOT(updateCurrentNodeItem()));
        connect(view, SIGNAL(view_edgeConstructed()), this, SLOT(updateDock()));
        connect(view, SIGNAL(view_nodeConstructed(NodeItem*)), this, SLOT(nodeConstructed(NodeItem*)));
        connect(view, SIGNAL(view_edgeDeleted(int,int)), this, SLOT(onEdgeDeleted(int, int)));
        connect(view, SIGNAL(view_nodeDeleted(int,int)), this, SLOT(onNodeDeleted(int, int)));
    }
}


/**
 * @brief DefinitionsDockScrollArea::nodeDeleted
 * This is called from DockScrollArea when a node has been deleted
 * and the dock contains an item that is attached to it.
 * @param nodeID - ID of the node that has been deleted.
 */
void DefinitionsDockScrollArea::nodeDeleted(QString nodeID)
{
    DockNodeItem* dockItem = getDockNodeItem(nodeID);
    if (!dockItem) {
        return;
    }

    if (dockItem->isDockItemLabel()) {

        // IDL deleted - remove corresponding file layout and then delete it
        if (idlLayoutItems.contains(nodeID)) {
            QVBoxLayout* idlLayout = idlLayoutItems[nodeID];
            if (idlLayout) {
                itemsLayout->removeItem(idlLayout);
                idlLayoutItems.remove(nodeID);
                idlLayout->deleteLater();
            }
        }

    } else {

        // Component/BlackBox/Aggregate/Vector deleted - remove from parent file's layout and then delete it
        DockNodeItem* idlDockItem = dockItem->getParentDockNodeItem();

        if (!idlDockItem) {
            qWarning() << "DefinitionsDockScrollArea::onNodeDeleted - Deleted node has no parent IDL file.";
            return;
        }

        QString idlID = idlDockItem->getID();
        QVBoxLayout* idlLayout = idlLayoutItems[idlID];

        // if the item to delete is the last child item stored in the IDL, hide the IDL item
        bool hideIdlLayout = idlDockItem->getChildrenDockItems().count() == 1;

        if (idlLayout) {
            // remove the child item from the File's layout and the File item's children items list
            idlLayout->removeWidget(dockItem);
            idlDockItem->removeChildDockItem(dockItem);

            // destruct the child dockNodeItem
            DockScrollArea::nodeDeleted(nodeID);
        }

        if (hideIdlLayout) {
            idlDockItem->setHidden(true);
        }
    }
}


/**
 * @brief DefinitionsDockScrollArea::dock_itemClicked
 * When an item in this dock is clicked, depending on what kind of node item is selected,
 * it either tries to create an instance/impl of the clicked dock item or it tries to
 * connect the selected node item to the node item corresponding to the clicked dock item.
 */
void DefinitionsDockScrollArea::dockNodeItemClicked()
{
    NodeItem* selectedNodeItem = getNodeView()->getSelectedNodeItem();
    DockNodeItem* dockNodeItem = qobject_cast<DockNodeItem*>(QObject::sender());

    if (!selectedNodeItem || !dockNodeItem || dockNodeItem->isDockItemLabel()) {
        setDockOpen(false);
        return;
    }

    int selectedNodeID = selectedNodeItem->getID();
    int dockNodeID = dockNodeItem->getID().toInt();
    getNodeView()->constructConnectedNode(selectedNodeID, dockNodeID, sourceDockItemKind, 0);

    // this closes this dock and then re-opens the parts dock
    emit dock_forceOpenDock();
}


/**
 * @brief DefinitionsDockScrollArea::updateDock
 * This is called whenever a node item is selected.
 * It checks to see if this dock should be enabled for the currently selected item.
 */
void DefinitionsDockScrollArea::updateDock()
{
    // if the dock is not open, do nothing
    if (!isDockOpen()) {
        return;
    }

    // if there is no selected item, close the dock
    if (!getCurrentNodeItem() || getCurrentNodeID() == -1) {
        setDockOpen(false);
        return;
    }

    // only update the dock if the previously selected item hasn't changed
    if (getCurrentNodeID() == sourceSelectedItemID) {
        filterDock();
    } else {
        if (isDockOpen()) {
            // this closes this dock and then opens the parts dock
            emit dock_forceOpenDock();
        }
    }
}


/**
 * @brief DefinitionsDockScrollArea::clear
 */
void DefinitionsDockScrollArea::clear()
{
    DockScrollArea::clear();
    idlLayoutItems.clear();
}


/**
 * @brief DefinitionsDockScrollArea::nodeConstructed
 * This gets called whenever a node has been constructed.
 * It checks to see if a dock item needs to be constucted for the new node.
 * @param nodeItem
 */
void DefinitionsDockScrollArea::nodeConstructed(NodeItem* nodeItem)
{
    if (!nodeItem || !nodeItem->isEntityItem()) {
        return;
    }

    EntityItem* entityItem = (EntityItem*) nodeItem;
    QString nodeKind = entityItem->getNodeKind();

    if (definitionKinds.contains(nodeKind)) {
        if (nodeKind == "IDL") {
            constructDockLabelItem(entityItem);
        } else {
            constructDockItem(entityItem);
        }
        updateDock();
    }
}


/**
 * @brief DefinitionsDockScrollArea::forceOpenDock
 * This function is called when certain dock items are clicked from the Parts dock.
 * This ensures that this dock is enabled and then opens it. This dock is then
 * filtered using the dock item kind that was clicked on from the Parts dock.
 * @param srcKind - item kind clicked on from the Parts dock.
 */
void DefinitionsDockScrollArea::forceOpenDock(QString srcKind)
{
    if (isDockOpen() || !getParentButton() || !getCurrentNodeItem() || (getCurrentNodeID() == -1)) {
        return;
    }

    sourceDockItemKind = srcKind;
    sourceSelectedItemID = getCurrentNodeID();

    // close the sender dock then open this dock
    DockScrollArea* dock = qobject_cast<DockScrollArea*>(QObject::sender());
    if (dock) {
        dock->setDockOpen(false);
    }

    setDockOpen();
    filterDock(srcKind);
}


/**
 * @brief DefinitionsDockScrollArea::filterDock
 * This function filters what is displayed in the dock based on nodeKind.
 * @param nodeKind - if this is empty, use the current selected item's kind.
 */
void DefinitionsDockScrollArea::filterDock(QString nodeKind)
{
    if (nodeKind.isEmpty()) {
        nodeKind = sourceDockItemKind;
    }

    QString kind;
    QString infoLabelText;
    bool hideCompsWithImpl = false;
    bool hideSelectedAggr = false;

    if (nodeKind.endsWith("Instance")) {
        kind = nodeKind.remove("Instance");
        if (kind == "Vector") {
            infoLabelText = "There are no IDL files containing initialised Vector entities.";
        } else {
            if (kind == "Aggregate") {
                hideSelectedAggr = true;
            }
            infoLabelText = "There are no IDL files containing " + kind + " entities.";
        }
    } else if (nodeKind.endsWith("Port") || nodeKind.endsWith("Delegate")) {
        kind = "Aggregate";
        infoLabelText = "There are no IDL files containing Aggregate entities.";
    } else if (nodeKind.endsWith("Impl")) {
        kind = nodeKind.remove("Impl");
        if (kind == "Component") {
            hideCompsWithImpl = true;
            infoLabelText = "There are no IDL files containing unimplemented Component entities.";
        } else {
            setInfoText("The selected entity's definition does not contain any OutEventPort entities.");
            showChildrenOutEventPorts();
            return;
        }
    } else {
        qWarning() << "DefinitionsDockScrollArea::filterDock - Node kind is not handled.";
        setDockOpen(false);
        return;
    }

    // only show dock items with the corresponding kind
    showDockItemsOfKind(kind);

    // if required, hide already implemented Components
    if (hideCompsWithImpl) {
        hideImplementedComponents();
    }

    // if the selected item is an Aggregate and the kind to show is Aggregate, hide the selected item's dock item
    if (hideSelectedAggr) {
        hideSelectedAggregate();
    }

    // update the information text for when this dock is empty depending on the filtered kind
    setInfoText(infoLabelText);
}


/**
 * @brief DefinitionsDockScrollArea::dockClosed
 * This is called everytime the dock is closed.
 * It is either when the selection has changed or an item in this dock has been clicked.
 */
void DefinitionsDockScrollArea::dockClosed()
{
    // reset previous source kind and ID that were used to filter this dock
    sourceDockItemKind = "";
    sourceSelectedItemID = -1;
}


/**
 * @brief DefinitionsDockScrollArea::dockToggled
 * @param opened
 */
void DefinitionsDockScrollArea::dockToggled(bool opened)
{
    QString action = "";
    if (opened) {
        action = "Select to construct a " + sourceDockItemKind;
    }
    emit dock_toggled(opened, action);
}


/**
 * @brief DefinitionsDockScrollArea::showDockItemsOfKind
 * This function displays all the dock items with the provided kind and hides the rest.
 * @param nodeKind - kind of dock node item to show
 *                 - disable dock if kind is empty
 */
void DefinitionsDockScrollArea::showDockItemsOfKind(QString nodeKind)
{
    // disable the dock
    if (nodeKind.isEmpty()) {
        setDockOpen(false);
        return;
    }

    // only show the dock node items with the specified kind
    foreach (DockNodeItem* dockItem, getDockNodeItems()) {
        QString dockItemKind = dockItem->getKind();
        bool showItem = dockItemKind == nodeKind;
        dockItem->setHidden(!showItem);
    }
}


/**
 * @brief DefinitionsDockScrollArea::hideDockItems
 */
void DefinitionsDockScrollArea::hideDockItems()
{
    foreach (DockNodeItem* dockItem, getDockNodeItems()) {
        dockItem->setHidden(true);
    }
}


/**
 * @brief DefinitionsDockScrollArea::hideImplementedComponents
 * This method is called when the BehaviourDefinitions or a ComponentImpl that is not connected
 * to a definition is selected. It hides all the Components that already have an implementation.
 */
void DefinitionsDockScrollArea::hideImplementedComponents()
{
    foreach (DockNodeItem* dockItem, getDockNodeItems()) {
        QString dockItemKind = dockItem->getKind();
        if (dockItemKind != "Component") {
            continue;
        }
        bool convert;
        int ID = dockItem->getID().toInt(&convert);
        if (convert) {
            if (getNodeView() && getNodeView()->getImplementation(ID)) {
                dockItem->setHidden(true);
            }
        }
    }
}


/**
 * @brief DefinitionsDockScrollArea::hideSelectedAggregate
 */
void DefinitionsDockScrollArea::hideSelectedAggregate()
{
    if (getCurrentNodeKind() == "Aggregate") {
        DockNodeItem* dockItem = getDockNodeItem(QString::number(getCurrentNodeID()));
        if (dockItem) {
            dockItem->setHidden(true);
        }
    }
}


/**
 * @brief DefinitionsDockScrollArea::showChildrenOutEventPorts
 */
void DefinitionsDockScrollArea::showChildrenOutEventPorts()
{
    NodeItem* selectedItem = getCurrentNodeItem();
    if (getCurrentNodeID() == -1 || !selectedItem || !selectedItem->getNodeAdapter()) {
        qWarning() << "DefinitionsDockScrollArea::showChildrenOutEventPorts - The selected entity is invalid.";
        setDockOpen(false);
        return;
    }

    int definitionID = selectedItem->getNodeAdapter()->getDefinitionID();
    if (definitionID == -1) {
        qWarning() << "DefinitionsDockScrollArea::showChildrenOutEventPorts - The selected entity doesn't have a definition.";
        setDockOpen(false);
        return;
    }

    DockNodeItem* componentLabelItem = getDockNodeItem("Component_" + QString::number(definitionID));
    if (componentLabelItem){
        showDockItemsOfKind("OutEventPort");
        foreach (DockNodeItem* item, getDockItemsOfKind("OutEventPort")) {
            if (item->getParentDockNodeItem() != componentLabelItem) {
                item->setHidden(true);
            }
        }
    } else {
        hideDockItems();
        setInfoText("The selected entity's definition does not contain any OutEventPorts.");
    }
}


/**
 * @brief DefinitionsDockScrollArea::sortDockItems
 * @param dockItem
 */
void DefinitionsDockScrollArea::sortDockItems(DockNodeItem* dockItem)
{
    if (!dockItem || dockItem->isDockItemLabel()) {
        return;
    }

    DockNodeItem* parentDockItem = dockItem->getParentDockNodeItem();
    QVBoxLayout* layoutToSort = 0;

    // remove the dock item from its parent file's layout.
    if (parentDockItem) {
        QString parentID = parentDockItem->getID();
        if (dockItem->getKind() == "OutEventPort") {
            layoutToSort = componentLayoutItems[parentID.remove("Component_")];
        } else {
            layoutToSort = idlLayoutItems[parentID];
        }
    } else {
        qWarning() << "DefinitionsDockScrollArea::sortDockItems - Dock item doesn't have a parent dock item.";
        return;
    }

    if (layoutToSort) {

        QString labelToSort = dockItem->getLabel();

        // remove the dock item from its parent's layout
        layoutToSort->removeWidget(dockItem);

        // iterate through the items in the parent's layout
        for (int i = 0; i < layoutToSort->count(); i++) {

            QLayoutItem* layoutItem = layoutToSort->itemAt(i);
            QString dockItemLabel;

            // get the current dock item's label
            DockNodeItem* currentDockItem = dynamic_cast<DockNodeItem*>(layoutItem->widget());
            if (currentDockItem && !currentDockItem->isDockItemLabel()) {
                dockItemLabel = currentDockItem->getLabel();
            }

            // if for some reason the label is empty or the dock item is a label, skip to the next item
            if (dockItemLabel.isEmpty()) {
                continue;
            }

            // compare current label to labelToSort
            if (labelToSort.compare(dockItemLabel, Qt::CaseInsensitive) <= 0) {
                // insert the dock item into the correct alphabetical spot in the layout
                layoutToSort->insertWidget(i, dockItem);
                return;
            }
        }

        // if there was no spot to insert the new dock item, add it to the end of the layout
        layoutToSort->addWidget(dockItem);

    } else {
        qWarning() << "DefinitionsDockScrollArea::sortDockItems - Layout to sort is null.";
    }
}


/**
 * @brief DefinitionsDockScrollArea::sortDockLabelItems
 * @param dockItem
 */
void DefinitionsDockScrollArea::sortDockLabelItems(DockNodeItem* dockItem)
{ if (!dockItem || !dockItem->isDockItemLabel() || !itemsLayout) {
        return;
    }

    QVBoxLayout* layout = 0;
    QString ID = dockItem->getID();
    QString labelToSort = dockItem->getLabel();
    bool isComponentLabel = dockItem->getKind() == "Component";

    if (isComponentLabel) {
        layout = componentLayoutItems[ID.remove("Component_")];
    } else {
        layout = idlLayoutItems[ID];
    }

    // remove the dock item label's layout from itemsLayout
    if (layout) {
        itemsLayout->removeItem(layout);
    } else {
        qWarning() << "DefinitionsDockScrollArea::sortDockLabelItems - Dock label item doesn't have a layout.";
        return;
    }

    // iterate through the items in this dock's layout
    for (int i = 0; i < itemsLayout->count(); i++) {

        QLayoutItem* layoutItem = itemsLayout->itemAt(i);
        QString dockItemLabel;

        // get the dock item's label
        QVBoxLayout* vLayout = dynamic_cast<QVBoxLayout*>(layoutItem);
        if (vLayout) {
            DockNodeItem* currentDockItem = 0;
            QString layoutID;
            if (isComponentLabel) {
                layoutID = componentLayoutItems.key(vLayout);
                currentDockItem = getDockNodeItem("Component_" + layoutID);
            } else {
                layoutID = idlLayoutItems.key(vLayout);
                currentDockItem = getDockNodeItem(layoutID);
            }
            if (currentDockItem) {
                dockItemLabel = currentDockItem->getLabel();
            }
        }

        // if for some reason the label is empty, skip to the next item
        if (dockItemLabel.isEmpty()) {
            continue;
        }

        // compare current label to labelToSort
        if (labelToSort.compare(dockItemLabel, Qt::CaseInsensitive) <= 0) {
            // insert the dock label item's layout into the correct alphabetical spot in the layout
            itemsLayout->insertLayout(i, layout);
            return;
        }
    }

    // if there was no spot to insert the dock label item's layout, add it to the end of the layout
    itemsLayout->addLayout(layout);
}


/**
 * @brief DefinitionsDockScrollArea::constructDockLabelItem
 * @param item
 */
void DefinitionsDockScrollArea::constructDockLabelItem(EntityItem *item)
{
    if (!item) {
        return;
    }

    // create a new dock item label and add it to its corresponding layout
    QString itemID = QString::number(item->getID());
    bool isComponentLabel = item->getNodeKind() == "Component";

    QString dockItemID = itemID;
    if (isComponentLabel) {
        dockItemID = "Component_" + itemID;
    }

    // check if the dock already has a label for item
    if (getDockNodeItem(dockItemID)) {
        return;
    }

    QVBoxLayout* layout = new QVBoxLayout();
    DockNodeItem* labelDockItem = new DockNodeItem("", item, this, true);

    if (isComponentLabel) {
        labelDockItem->setID(dockItemID);
        componentLayoutItems[itemID] = layout;
    } else {
        idlLayoutItems[itemID] = layout;
    }

    layout->addWidget(labelDockItem);
    addDockNodeItem(labelDockItem, -1, false);
    sortDockLabelItems(labelDockItem);
    connect(labelDockItem, SIGNAL(dockItem_relabelled(DockNodeItem*)), this, SLOT(sortDockLabelItems(DockNodeItem*)));

    // initially hide label dock items that don't have any children
    if (!item->hasChildren()) {
        labelDockItem->setHidden(true);
    }
}


/**
 * @brief DefinitionsDockScrollArea::constructDockItem
 * @param item
 */
void DefinitionsDockScrollArea::constructDockItem(EntityItem *item)
{
    if (!item || getDockNodeItem(QString::number(item->getID()))) {
        return;
    }

    EntityItem* parentEntityItem = item->getParentEntityItem();
    if (!parentEntityItem) {
        qWarning() << "DefinitionsDockScrollArea::nodeConstructed - Parent entity item is null.";
        return;
    }

    QString parentID = QString::number(parentEntityItem->getID());
    bool parentComponent = parentEntityItem->getNodeKind() == "Component";
    DockNodeItem* parentDockItem = 0;
    QVBoxLayout* layout = 0;

    // construct a dock item label for parent Components
    if (parentComponent) {
        parentDockItem = getDockNodeItem("Component_" + parentID);
        if (!parentDockItem) {
            constructDockLabelItem(parentEntityItem);
            parentDockItem = getDockNodeItem("Component_" + parentID);
        }
        layout = componentLayoutItems[parentID];
    } else {
        parentDockItem = getDockNodeItem(parentID);
        layout = idlLayoutItems[parentID];
    }

    if (parentDockItem && layout) {

        // connect the new dock item to its parent dock item
        DockNodeItem* dockItem = new DockNodeItem("", item, this);
        dockItem->setParentDockNodeItem(parentDockItem);
        parentDockItem->addChildDockItem(dockItem);

        // add it to its parent's layout and to this dock's list of dock items
        layout->addWidget(dockItem);
        addDockNodeItem(dockItem, -1, false);
        sortDockItems(dockItem);
        connect(dockItem, SIGNAL(dockItem_relabelled(DockNodeItem*)), this, SLOT(sortDockItems(DockNodeItem*)));

        // initially hide dock items for Vectors that don't have any children
        if (item->getNodeKind() == "Vector") {
            if (!item->hasChildren()) {
                dockItem->setForceHidden(true);
            }
            connect(item, SIGNAL(entityItem_firstChildAdded(int)), dockItem, SLOT(changeVectorHiddenState()));
            connect(item, SIGNAL(entityItem_lastChildRemoved(int)), dockItem, SLOT(changeVectorHiddenState()));
        }

    } else {
        qWarning() << "DefinitionsDockScrollArea::constructDockItem - Parent dock item or layout is null.";
    }
}

