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
    definitions_notAllowedKinds.append("DeploymentDefinitions");
    definitions_notAllowedKinds.append("AssemblyDefinitions");
    definitions_notAllowedKinds.append("HardwareDefinitions");
    definitions_notAllowedKinds.append("ManagementComponent");
    definitions_notAllowedKinds.append("HardwareCluster");
    definitions_notAllowedKinds.append("HardwareNode");
    definitions_notAllowedKinds.append("IDL");
    definitions_notAllowedKinds.append("Component");
    definitions_notAllowedKinds.append("Member");
    definitions_notAllowedKinds.append("AggregateInstance");
    definitions_notAllowedKinds.append("MemberInstance");
    definitions_notAllowedKinds.append("InEventPort");
    definitions_notAllowedKinds.append("OutEventPort");
    definitions_notAllowedKinds.append("Attribute");
    definitions_notAllowedKinds.append("InEventPortImpl");
    definitions_notAllowedKinds.append("OutEventPortImpl");
    definitions_notAllowedKinds.append("AttributeImpl");
    definitions_notAllowedKinds.append("Workload");
    definitions_notAllowedKinds.append("Process");
    definitions_notAllowedKinds.append("BranchState");
    definitions_notAllowedKinds.append("Condition");
    definitions_notAllowedKinds.append("Termination");
    definitions_notAllowedKinds.append("Variable");
    definitions_notAllowedKinds.append("PeriodicEvent");
    definitions_notAllowedKinds.append("InEventPortInstance");
    definitions_notAllowedKinds.append("OutEventPortInstance");
    definitions_notAllowedKinds.append("AttributeInstance");
    definitions_notAllowedKinds.append("InEventPortDelegate");
    definitions_notAllowedKinds.append("OutEventPortDelegate");
    definitions_notAllowedKinds.append("BlackBox");
    definitions_notAllowedKinds.append("BlackBoxInstance");
    definitions_notAllowedKinds.append("ComponentInstance");
    definitions_notAllowedKinds.append("ComponentImpl");
    definitions_notAllowedKinds.append("InputParameter");
    definitions_notAllowedKinds.append("ReturnParameter");
    //definitions_notAllowedKinds.append("Vector");
    definitions_notAllowedKinds.append("VectorInstance");

    setNotAllowedKinds(definitions_notAllowedKinds);

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
        if (!item->isFileLabel()) {
            dockNodeItems.append(item);
        }
    }

    return dockNodeItems;
}


/**
 * @brief DefinitionsDockScrollArea::nodeDeleted
 * @param nodeID
 */
void DefinitionsDockScrollArea::nodeDeleted(QString nodeID)
{
    DockNodeItem* dockItem = getDockNodeItem(nodeID);
    if (!dockItem) {
        return;
    }

    if (dockItem->isFileLabel()) {

        // IDL file deleted - remove corresponding file layout and then delete it
        if (fileLayoutItems.contains(nodeID)) {
            QVBoxLayout* fileLayout = fileLayoutItems[nodeID];
            if (fileLayout) {
                itemsLayout->removeItem(fileLayout);
                fileLayoutItems.remove(nodeID);
                fileLayout->deleteLater();
            }
        }

    } else {

        // Component/BlackBox deleted - remove from parent file's layout and then delete it
        DockNodeItem* fileDockItem = dockItem->getParentDockNodeItem();

        if (!fileDockItem) {
            qWarning() << "DefinitionsDockScrollArea::onNodeDeleted - Component/BlackBox has no parent IDL file.";
            return;
        }

        QString fileID = fileDockItem->getID();
        QVBoxLayout* fileLayout = fileLayoutItems[fileID];

        // if there is only 1 Component/BlackBox stored in the File, delete the File too
        bool deleteFileLayout = fileDockItem->getChildrenDockItems().count() == 1;

        if (fileLayout) {
            // remove this Component/BlackBox from the File's layout and the File item's children items list
            fileLayout->removeWidget(dockItem);
            fileDockItem->removeChildDockItem(dockItem);

            // destruct the Component/BlackBox's DockNodeItem
            DockScrollArea::nodeDeleted(nodeID);
        }

        if (deleteFileLayout) {
            // destruct the File's DockNodeItem
            DockScrollArea::nodeDeleted(fileID);
        }
    }

    //qDebug() << "nodeDeleted";
    updateDock();
}


/**
 * @brief DefinitionsDockScrollArea::edgeDeleted
 */
void DefinitionsDockScrollArea::edgeDeleted()
{
    //qDebug() << "edgeDeleted";
    updateDock();
}


/**
 * @brief DefinitionsDockScrollArea::dock_itemClicked
 * When an item in this dock is clicked, one of the following actions will happen.
 * If a ComponentAssembly or BehaviourDefinitions is selected, this constructs a connected
 * Component/BlackBox Instance or a ComponentImpl respectively as a child of the selected entity.
 * If an unconnected Component/BlackBox Instance is selected, or an undefined ComponentImpl,
 * this connects it to the Component/BlackBox corresponding to the clicked dock node item.
 */
void DefinitionsDockScrollArea::dockNodeItemClicked()
{
    NodeItem* selectedNodeItem = getNodeView()->getSelectedNodeItem();
    DockNodeItem* dockNodeItem = qobject_cast<DockNodeItem*>(QObject::sender());

    if (!selectedNodeItem || !dockNodeItem) {
        return;
    }

    QString selectedNodeKind = selectedNodeItem->getNodeKind();
    int selectedNodeID = selectedNodeItem->getID();
    int dockNodeID = dockNodeItem->getID().toInt();

    if (selectedNodeKind == "ComponentAssembly" || selectedNodeKind == "Aggregate" || selectedNodeKind == "Vector") {
        getNodeView()->constructConnectedNode(selectedNodeID, dockNodeID, dockNodeItem->getKind() + "Instance", 0);
    } else if (selectedNodeKind == "BlackBoxInstance" || selectedNodeKind == "ComponentInstance" || selectedNodeKind == "ComponentImpl") {
        getNodeView()->constructEdge(selectedNodeID, dockNodeID);
    } else if (selectedNodeKind == "BehaviourDefinitions") {
        getNodeView()->constructConnectedNode(selectedNodeID, dockNodeID, "ComponentImpl", 0);
    }
}


/**
 * @brief DefinitionsDockScrollArea::updateDock
 * This is called whenever a node item is selected.
 * It checks to see if this dock should be enabled for the currently selected item.
 */
void DefinitionsDockScrollArea::updateDock()
{
    DockScrollArea::updateDock();
    if (!isDockEnabled()) {
        return;
    }

    //qDebug() << "Updating Definitions dock!";

    // special case - Vector entities can only have one child (AggregateInstance/Member)
    NodeItem* selectedItem = getCurrentNodeItem();
    if (selectedItem && selectedItem->getNodeKind() == "Vector") {
        if (selectedItem->hasChildren()) {
            setDockEnabled(false);
            return;
        }
    }

    filterDock();
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

        DockNodeItem* dockItem = new DockNodeItem("", entityItem, this);
        EntityItem* fileItem = entityItem->getParentEntityItem();

        if (!fileItem) {
            qWarning() << "DefinitionsDockScrollArea::nodeConstructed - Component/BlackBox's parent item is null.";
            return;
        }

        QString fileID = QString::number(fileItem->getID());

        // check if there is already a layout and label for the parent File
        if (!fileLayoutItems.contains(fileID)){
            // create a new File label and add it to the File's layout
            //DockNodeItem* fileDockItem = new DockNodeItem("FileLabel", fileItem, this);
            DockNodeItem* fileDockItem = new DockNodeItem("FileLabel", fileItem, this, true);
            QVBoxLayout* fileLayout = new QVBoxLayout();

            fileLayoutItems[fileID] = fileLayout;
            fileLayout->addWidget(fileDockItem);
            addDockNodeItem(fileDockItem, -1, false);

            insertDockNodeItem(fileDockItem);
            connect(fileDockItem, SIGNAL(dockItem_relabelled(DockNodeItem*)), this, SLOT(insertDockNodeItem(DockNodeItem*)));
        }

        // connect the new dock item to its parent file item
        DockNodeItem* parentItem = getDockNodeItem(fileID);
        if (parentItem) {
            dockItem->setParentDockNodeItem(parentItem);
            parentItem->addChildDockItem(dockItem);
        }

        if (fileLayoutItems.contains(fileID)) {
            QVBoxLayout* fileLayout = fileLayoutItems[fileID];
            fileLayout->addWidget(dockItem);
            addDockNodeItem(dockItem, -1, false);
            insertDockNodeItem(dockItem);
            connect(dockItem, SIGNAL(dockItem_relabelled(DockNodeItem*)), this, SLOT(insertDockNodeItem(DockNodeItem*)));
        }

    } else if (/*nodeKind == "ComponentImpl" ||*/ nodeKind == "Member" /*|| nodeKind == "AggregateInstance"*/) {
        //qDebug() << "nodeConstructed!";
        updateDock();
    }
}


/**
 * @brief DefinitionsDockScrollArea::forceOpenDock
 * @param srcKind
 */
void DefinitionsDockScrollArea::forceOpenDock(QString srcKind)
{
    if (isDockOpen()) {
        return;
    }
    if (!isDockEnabled()) {
        setDockEnabled(true);
    }
    if (getParentButton()) {
        getParentButton()->pressed();
        filterDock(srcKind);
    }
}


/**
 * @brief DefinitionsDockScrollArea::dockClosed
 */
void DefinitionsDockScrollArea::dockClosed()
{
    //qDebug() << "dockClosed";
    //updateDock();
}


/**
 * @brief DefinitionsDockScrollArea::filterDock
 * @param nodeKind
 */
void DefinitionsDockScrollArea::filterDock(QString nodeKind)
{
    if (nodeKind.isEmpty()) {
        if (getCurrentNodeItem()) {
            nodeKind = getCurrentNodeItem()->getNodeKind();
        } else {
            setDockEnabled(false);
            return;
        }
    }

    QStringList kinds;
    bool hideCompsWithImpl = false;

    if (nodeKind.endsWith("Instance")) {
        kinds.append(nodeKind.remove("Instance"));
    }  else if (nodeKind == "ComponentImpl" || nodeKind == "BehaviourDefinitions") {
        kinds.append("Component");
        hideCompsWithImpl = true;
    } else if (nodeKind == "ComponentAssembly") {
        kinds.append("BlackBox");
        kinds.append("Component");
    } else if (nodeKind == "Aggregate") {
        kinds.append("Vector");
        kinds.append("Aggregate");
    } else if (nodeKind == "Vector") {
        kinds.append("Aggregate");
    }

    // all dock items are shown for all the other allowed kinds
    showDockItemsOfKinds(kinds);

    // if required, hide already implemented Components
    if (hideCompsWithImpl) {
        hideImplementedComponents();
    }
}


/**
 * @brief DefinitionsDockScrollArea::insertDockNodeItem
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
    bool isFileLabel = dockItem->isFileLabel();

    if (isFileLabel) {
        // IDL file - remove  fileLayout from itemsLayout
        layoutToSort = itemsLayout;
        QVBoxLayout* fileLayout = fileLayoutItems[ID];
        if (fileLayout) {
            layoutToSort->removeItem(fileLayout);
        }
    } else {
        // Component/BlackBox - remove the Component/BlackBox from its parent file's layout.
        if (parentDockItem) {
            QString parentID = parentDockItem->getID();
            layoutToSort = fileLayoutItems[parentID];
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

        if (isFileLabel) {
            // get the IDL file's label
            QVBoxLayout* fileLayout = dynamic_cast<QVBoxLayout*>(layoutItem);
            if (fileLayout) {
                QString fileID = fileLayoutItems.key(fileLayout, "");
                DockNodeItem* dockItem = getDockNodeItem(fileID);
                if (dockItem) {
                    dockItemLabel = dockItem->getLabel();
                }
            }
        } else {
            // get the Component/BlackBox's label
            DockNodeItem* dockItem = dynamic_cast<DockNodeItem*>(layoutItem->widget());
            if (dockItem && !dockItem->isFileLabel()) {
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
            if (isFileLabel) {
                QVBoxLayout* fileLayout = fileLayoutItems[ID];
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
    if (isFileLabel) {
        QVBoxLayout* fileLayout = fileLayoutItems[ID];
        if (fileLayout) {
            layoutToSort->addLayout(fileLayout);
        }
    } else {
        layoutToSort->addWidget(dockItem);
    }
}


/**
 * @brief DefinitionsDockScrollArea::hideImplementedComponents
 * This method is called when the BehaviourDefinitions or a
 * ComponentImpl that is not connected to a definition is selected.
 * It hides all the Components that already have an implementation.
 */
void DefinitionsDockScrollArea::hideImplementedComponents()
{
    foreach (DockNodeItem* dockItem, getDockNodeItems()) {
        bool convert;
        int ID = dockItem->getID().toInt(&convert);
        if (convert) {
            if (getNodeView() && getNodeView()->getImplementation(ID)) {
                dockItem->setHidden(true);
            }
        }
    }
}


void DefinitionsDockScrollArea::updateForVector()
{
    // special case - Vector entities can only have one child (AggregateInstance/Member)
    NodeItem* selectedItem = getCurrentNodeItem();
    if (selectedItem && selectedItem->getNodeKind() == "Vector") {
         qCritical() << "updateForVector" << selectedItem;
        if (selectedItem->hasChildren()) {
            foreach(GraphMLItem* child, selectedItem->getChildren()){
                if(child->getGraphML()){

                    //qCritical() << child->getGraphML()->toString();
                }else{
                    qCritical() << " NO CHILD!";
                }
            }

            //qDebug() << "Enable definitions dock: " << childDeleted;
            //qDebug() << selectedItem->getChildren().at(0)->getGraphML()->toString();
            setDockEnabled(false);
        } else {
            setDockEnabled(true);
        }
    }
}


/**
 * @brief DefinitionsDockScrollArea::showDockItemsOfKind
 * @param kinds - list of kinds of dock node items to show
 *              - show all kinds if this is empty
 */
void DefinitionsDockScrollArea::showDockItemsOfKinds(QStringList kinds)
{
    if (kinds.isEmpty()) {
        // show all dock node items
        foreach (DockNodeItem* dockItem, getDockNodeItems()) {
            dockItem->setHidden(false);
        }
    } else {
        // only show the dock node items with the specified kind
        foreach (DockNodeItem* dockItem, getDockNodeItems()) {
            QString dockItemKind = dockItem->getKind();
            if (kinds.contains(dockItemKind)) {
                dockItem->setHidden(false);
            } else {
                dockItem->setHidden(true);
            }
        }
    }
}


/**
 * @brief DefinitionsDockScrollArea::clear
 */
void DefinitionsDockScrollArea::clear()
{
    DockScrollArea::clear();
    fileLayoutItems.clear();
}


/**
 * @brief DefinitionsDockScrollArea::onNodeDeleted
 * @param nodeID
 * @param parentID
 */
void DefinitionsDockScrollArea::onNodeDeleted(int nodeID, int parentID)
{
    DockScrollArea::onNodeDeleted(nodeID, parentID);
}
