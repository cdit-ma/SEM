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
    // populate list of not allowed kinds
    if (view) {
        definitions_notAllowedKinds = view->getAllNodeKinds();
    }

    // this is the list of entity kinds that this dock constructs items for
    definitionKinds.append("IDL");
    definitionKinds.append("Component");
    definitionKinds.append("BlackBox");
    definitionKinds.append("Aggregate");
    definitionKinds.append("Vector");

    // setup definitions-dock specific layout
    mainLayout = new QVBoxLayout();
    itemsLayout = new QVBoxLayout();
    mainLayout->addLayout(itemsLayout);
    mainLayout->addStretch();
    getLayout()->addLayout(mainLayout);

    setNotAllowedKinds(definitions_notAllowedKinds);
    setDockEnabled(false);

    connectToView();
    connect(this, SIGNAL(dock_closed()), this, SLOT(dockClosed()));
}


/**
 * @brief DefinitionsDockScrollArea::getDockNodeItems
 * This returns the list of existing dock node items; not including file labels.
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
        setDockEnabled(false);
        return;
    }

    int selectedNodeID = selectedNodeItem->getID();
    int dockNodeID = dockNodeItem->getID().toInt();
    getNodeView()->constructConnectedNode(selectedNodeID, dockNodeID, sourceDockItemKind, 0);

    // disable this dock after an item has been clicked
    dockClosed();

    // then re-open the parts dock
    emit dock_forceOpenDock(PARTS_DOCK);
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

    // if there is no selected item, disable the dock
    if (!getCurrentNodeItem() || getCurrentNodeID() == -1) {
        setDockEnabled(false);
        return;
    }

    // only update the dock if the previously selected item hasn't changed
    if (getCurrentNodeID() == sourceSelectedItemID) {
        filterDock();
    } else {
        setDockEnabled(false);
    }
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

            // create a new IDL label and add it to the IDL's layout
            DockNodeItem* idlDockItem = new DockNodeItem("DockItemLabel", entityItem, this, true);
            QVBoxLayout* idlLayout = new QVBoxLayout();

            idlLayoutItems[QString::number(entityItem->getID())] = idlLayout;
            idlLayout->addWidget(idlDockItem);
            addDockNodeItem(idlDockItem, -1, false);

            insertDockNodeItem(idlDockItem);
            connect(idlDockItem, SIGNAL(dockItem_relabelled(DockNodeItem*)), this, SLOT(insertDockNodeItem(DockNodeItem*)));

            // initially hide dock items for IDLs that don't have any children
            if (!entityItem->hasChildren()) {
                idlDockItem->setHidden(true);
            }

        } else {

            DockNodeItem* dockItem = new DockNodeItem("", entityItem, this);
            EntityItem* parentEntityItem = entityItem->getParentEntityItem();

            if (!parentEntityItem) {
                qWarning() << "DefinitionsDockScrollArea::nodeConstructed - IDL entity item is null.";
                return;
            }

            QString idlID = QString::number(parentEntityItem->getID());

            // check if there is already a layout and label for the parent IDL
            if (!idlLayoutItems.contains(idlID)){
                qWarning() << "DefinitionsDockScrollArea::nodeConstructed - Parent IDL doesn't have a layout.";
                return;
            }

            // connect the new dock item to its parent file item
            DockNodeItem* parentDockItem = getDockNodeItem(idlID);
            if (parentDockItem) {
                dockItem->setParentDockNodeItem(parentDockItem);
                parentDockItem->addChildDockItem(dockItem);
            }

            QVBoxLayout* idlLayout = idlLayoutItems[idlID];
            idlLayout->addWidget(dockItem);
            addDockNodeItem(dockItem, -1, false);
            insertDockNodeItem(dockItem);
            connect(dockItem, SIGNAL(dockItem_relabelled(DockNodeItem*)), this, SLOT(insertDockNodeItem(DockNodeItem*)));

            // initially hide dock items for Vectors that don't have any children
            if (nodeKind == "Vector") {
                if (!entityItem->hasChildren()) {
                    dockItem->setForceHidden(true);
                }
                connect(entityItem, SIGNAL(entityItem_firstChildAdded(int)), dockItem, SLOT(changeVectorHiddenState()));
                connect(entityItem, SIGNAL(entityItem_lastChildRemoved(int)), dockItem, SLOT(changeVectorHiddenState()));
            }
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

    if (!isDockEnabled()) {
        setDockEnabled(true);
    }

    sourceDockItemKind = srcKind;
    sourceSelectedItemID = getCurrentNodeID();

    getParentButton()->pressed();
    filterDock(srcKind);

    //sourceDockItemKind = srcKind;
    //sourceSelectedItemID = getCurrentNodeID();
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

    if (nodeKind.endsWith("Instance")) {
        kind = nodeKind.remove("Instance");
        if (kind == "Vector") {
            infoLabelText = "There are no IDL files containing initialised Vector entities.";
        } else {
            infoLabelText = "There are no IDL files containing " + kind + " entities.";
        }
    } else if (nodeKind.endsWith("Delegate")) {
        kind = "Aggregate";
        infoLabelText = "There are no IDL files containing Aggregate entities.";

    } else if (nodeKind.endsWith("Impl")) {
        kind = nodeKind.remove("Impl");
        if (kind == "Component") {
            hideCompsWithImpl = true;
            infoLabelText = "There are no IDL files containing unimplemented Component entities.";
        } else {
            infoLabelText = "The selected entity's definition does not contain any OutEventPort entities.";
        }
    } else {
        qWarning() << "DefinitionsDockScrollArea::filterDock - Node kind is not handled.";
        setDockEnabled(false);
        return;
    }

    // only show dock items with the corresponding kind
    showDockItemsOfKind(kind);

    // if required, hide already implemented Components
    if (hideCompsWithImpl) {
        hideImplementedComponents();
    }

    setInfoText(infoLabelText);
}


/**
 * @brief DefinitionsDockScrollArea::insertDockNodeItem
 * This is called whenever this dock constructs a new dock item.
 * It inserts the new dock item to the correct spot to maintain alphabetical ordering.
 * @param dockItem
 */
void DefinitionsDockScrollArea::insertDockNodeItem(DockNodeItem* dockItem)
{
    if (!dockItem) {
        return;
    }

    DockNodeItem* parentDockItem = dockItem->getParentDockNodeItem();
    QVBoxLayout* layoutToSort = 0;

    QString ID = dockItem->getID();
    QString labelToSort = dockItem->getLabel();
    bool isDockItemLabel = dockItem->isDockItemLabel();

    if (isDockItemLabel) {
        // IDL file - remove  fileLayout from itemsLayout
        layoutToSort = itemsLayout;
        QVBoxLayout* fileLayout = idlLayoutItems[ID];
        if (fileLayout) {
            layoutToSort->removeItem(fileLayout);
        }
    } else {
        // Component/BlackBox - remove the Component/BlackBox from its parent file's layout.
        if (parentDockItem) {
            QString parentID = parentDockItem->getID();
            layoutToSort = idlLayoutItems[parentID];
            if (layoutToSort) {
                layoutToSort->removeWidget(dockItem);
            }
        }
    }

    if (!layoutToSort) {
        qWarning() << "DefinitionsDockScrollArea::insertDockNodeItem - Layout to sort is null.";
        return;
    }

    // iterate through the items in this dock's layout
    for (int i = 0; i < layoutToSort->count(); i++) {

        QLayoutItem* layoutItem = layoutToSort->itemAt(i);
        QString dockItemLabel;

        if (isDockItemLabel) {
            // get the IDL file's label
            QVBoxLayout* fileLayout = dynamic_cast<QVBoxLayout*>(layoutItem);
            if (fileLayout) {
                QString fileID = idlLayoutItems.key(fileLayout, "");
                DockNodeItem* dockItem = getDockNodeItem(fileID);
                if (dockItem) {
                    dockItemLabel = dockItem->getLabel();
                }
            }
        } else {
            // get the Component/BlackBox's label
            DockNodeItem* dockItem = dynamic_cast<DockNodeItem*>(layoutItem->widget());
            if (dockItem && !dockItem->isDockItemLabel()) {
                dockItemLabel = dockItem->getLabel();
            }
        }

        // if for some reason the label is empty, skip to the next item
        if (dockItemLabel.isEmpty()) {
            continue;
        }

        // compare existing file names to the new file name
        // insert the new File into the correct alphabetical spot in the layout
        if (labelToSort.compare(dockItemLabel, Qt::CaseInsensitive) <= 0) {
            if (isDockItemLabel) {
                QVBoxLayout* fileLayout = idlLayoutItems[ID];
                if (fileLayout) {
                    layoutToSort->insertLayout(i, fileLayout);
                }
            } else {
                layoutToSort->insertWidget(i, dockItem);
            }
            return;
        }
    }

    // if there was no spot to insert the new File layout, add it to the end of the layout
    if (isDockItemLabel) {
        QVBoxLayout* fileLayout = idlLayoutItems[ID];
        if (fileLayout) {
            layoutToSort->addLayout(fileLayout);
        }
    } else {
        layoutToSort->addWidget(dockItem);
    }
}


/**
 * @brief DefinitionsDockScrollArea::dockClosed
 * This is called everytime the dock is closed.
 * It is either when the parent toggle button or an item in this dock has been clicked.
 */
void DefinitionsDockScrollArea::dockClosed()
{
    // the moment this dock is closed, it is also disabled
    if (isDockEnabled()) {
        setDockEnabled(false, true);
    }

    sourceDockItemKind = "";
    sourceSelectedItemID = -1;
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
        setDockEnabled(false);
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
 * @brief DefinitionsDockScrollArea::showChildrenOutEventPorts
 */
void DefinitionsDockScrollArea::showChildrenOutEventPorts()
{
    /*
    if (!getCurrentNodeItem() || getCurrentNodeID() == -1) {
        return;
    }

    hideDockItems();

    QList<NodeItem*> outEventPorts;
    NodeItem* component = getNodeView()->getNodeItemFromID(nodeItem->getNode()->getDefinition()->getID());
    if (!component) {
        return;
    }

    foreach (GraphMLItem* item, component->getChildren()) {
        if (item->getNodeKind() == "OutEventPort") {
            outEventPorts.append((NodeItem*)item);
        }
    }

    DockNodeItem* compDockItem = new DockNodeItem("DockItemLabel", ((EntityItem*)getCurrentNodeItem()), this, true);
    QVBoxLayout* compLayout = new QVBoxLayout();
    tempDockItems.append(compDockItem);
    addDockNodeItem(compDockItem, -1, false);

    foreach (NodeItem* item, outEventPorts) {
        DockNodeItem* dockItem = new DockNodeItem("", ((EntityItem*)item), this);
        compLayout->addWidget(dockItem);
        tempDockItems.append(dockItem);
    }

    // initially hide dock items for IDLs that don't have any children
    if (!entityItem->hasChildren()) {
        idlDockItem->setHidden(true);
    }


    // connect the new dock item to its parent file item
    DockNodeItem* parentDockItem = getDockNodeItem(idlID);
    if (parentDockItem) {
        dockItem->setParentDockNodeItem(parentDockItem);
        parentDockItem->addChildDockItem(dockItem);
    }

    QVBoxLayout* idlLayout = idlLayoutItems[idlID];
    idlLayout->addWidget(dockItem);
    addDockNodeItem(dockItem, -1, false);
    insertDockNodeItem(dockItem);
    connect(dockItem, SIGNAL(dockItem_relabelled(DockNodeItem*)), this, SLOT(insertDockNodeItem(DockNodeItem*)));

    // initially hide dock items for Vectors that don't have any children
    if (nodeKind == "Vector") {
        if (!entityItem->hasChildren()) {
            dockItem->setForceHidden(true);
        }
        connect(entityItem, SIGNAL(entityItem_firstChildAdded(int)), dockItem, SLOT(changeVectorHiddenState()));
        connect(entityItem, SIGNAL(entityItem_lastChildRemoved(int)), dockItem, SLOT(changeVectorHiddenState()));
    }
    */
}


/**
 * @brief DefinitionsDockScrollArea::clear
 */
void DefinitionsDockScrollArea::clear()
{
    DockScrollArea::clear();
    idlLayoutItems.clear();
}
