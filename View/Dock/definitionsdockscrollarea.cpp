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
    definitions_notAllowedKinds.append("Aggregate");
    definitions_notAllowedKinds.append("Member");
    definitions_notAllowedKinds.append("AggregateInstance");
    definitions_notAllowedKinds.append("MemberInstance");
    definitions_notAllowedKinds.append("InEventPort");
    definitions_notAllowedKinds.append("OutEventPort");
    definitions_notAllowedKinds.append("Attribute");
    //definitions_notAllowedKinds.append("ComponentImpl");
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
    //definitions_notAllowedKinds.append("ComponentInstance");
    definitions_notAllowedKinds.append("InEventPortInstance");
    definitions_notAllowedKinds.append("OutEventPortInstance");
    definitions_notAllowedKinds.append("AttributeInstance");
    definitions_notAllowedKinds.append("InEventPortDelegate");
    definitions_notAllowedKinds.append("OutEventPortDelegate");
    definitions_notAllowedKinds.append("BlackBox");
    definitions_notAllowedKinds.append("BlackBoxInstance");

    setNotAllowedKinds(definitions_notAllowedKinds);

    // setup definitions-dock specific layout
    mainLayout = new QVBoxLayout();
    itemsLayout = new QVBoxLayout();
    mainLayout->addLayout(itemsLayout);
    mainLayout->addStretch();
    getLayout()->addLayout(mainLayout);
}


/**
 * @brief DefinitionsDockScrollArea::getDockNodeItems
 * This returns the list of existing dock node items; not including file labels.
 * @return
 */
QList<DockNodeItem*> DefinitionsDockScrollArea::getDockNodeItems()
{
    QList<DockNodeItem*> dockNodeItems;
    foreach (DockNodeItem* item, DockScrollArea::getDockNodeItems()) {
        if (!item->isFileLabel()) {
            dockNodeItems.append(item);
        }
    }
    return dockNodeItems;
}


/**
 * @brief DefinitionsDockScrollArea::onNodeDeleted
 * @param ID
 */
void DefinitionsDockScrollArea::onNodeDeleted(QString ID)
{
    DockNodeItem* dockItem = getDockNodeItem(ID);
    if (!dockItem) {
        return;
    }

    if (dockItem->isFileLabel()) {

        //Its a Node

        //Get File Layout
        if (fileLayoutItems.contains(ID)) {
            QVBoxLayout* fileLayout = fileLayoutItems[ID];
            if (fileLayout) {
                itemsLayout->removeItem(fileLayout);
                fileLayout->deleteLater();
                fileLayoutItems.remove(ID);
            }
        }

     } else {

        //Its a Component
        DockNodeItem* fileDockItem = dockItem->getParentDockNodeItem();

        if (!fileDockItem) {
            qCritical() << "Deleting Component, but has no parent file.";
            return;
        }

        QString fileID = fileDockItem->getID();
        QVBoxLayout* fileLayout = fileLayoutItems[fileID];

        //If there is only 1 Component Stored in the File, we should delete the File too!
        bool deleteFileLayout = fileDockItem->getChildrenDockItems().count() == 1;

        if (fileLayout) {
            //Remove this Component from the File Layout
            fileLayout->removeWidget(dockItem);
            fileDockItem->removeChildDockItem(dockItem);

            //Destruct the Components' DockNodeItem
            DockScrollArea::onNodeDeleted(ID);
        }

        if (deleteFileLayout) {
            //Destruct the Files' DockNodeItem
            DockScrollArea::onNodeDeleted(fileID);
        }
    }
}


/**
 * @brief DefinitionsDockScrollArea::onEdgeDeleted
 */
void DefinitionsDockScrollArea::onEdgeDeleted()
{
    refreshDock();
}


/**
 * @brief DefinitionsDockScrollArea::dock_itemClicked
 * When an item in this dock is clicked, it tries to either add a ComponentInstance
 * or connect a ComponentInstance to the clicked Component if that action is allowed.
 * If a ComponentImpl is selected, it tries to connect it to the clicked Component.
 */
void DefinitionsDockScrollArea::dockNodeItemClicked()
{
    NodeItem* selectedNodeItem = getNodeView()->getSelectedNodeItem();
    DockNodeItem* dockNodeItem = dynamic_cast<DockNodeItem*>(sender());

    if (selectedNodeItem && dockNodeItem) {
        QString selectedNodeID = selectedNodeItem->getID();
        QString selectedNodeKind = selectedNodeItem->getNodeKind();
        QString dockNodeID = dockNodeItem->getID();

        if (selectedNodeKind == "ComponentAssembly") {
            getNodeView()->constructConnectedNode(selectedNodeID, dockNodeID, "ComponentInstance", 0);
        } else if (selectedNodeKind == "ComponentInstance" || selectedNodeKind == "ComponentImpl") {
            getNodeView()->constructEdge(selectedNodeID, dockNodeID); //, "ComponentInstance");
        } else if (selectedNodeKind == "BehaviourDefinitions") {
            getNodeView()->constructConnectedNode(selectedNodeID, dockNodeID, "ComponentImpl", 0);
        }
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

    // if the dock is disabled, there is no need to update
    if (!isDockEnabled()) {
        return;
    }

    // the first check should catch this case
    if (!getCurrentNodeItem() || getCurrentNodeID() == "") {
        return;
    }

    if (getCurrentNodeItem()) {

        QString nodeKind =  getCurrentNodeItem()->getNodeKind();

        // check for special cases - ComponentInstance & ComponentImpl
        // they're only allowed kinds if they don't have a definition
        // if the selected item is the BehaviourDefinitions or a ComponentImpl without
        // a definition, hide Components that already have an implementation

        if (nodeKind == "ComponentInstance" || nodeKind == "ComponentImpl") {
            Node* node = getCurrentNodeItem()->getNode();
            if (node->getDefinition()) {
                setDockEnabled(false);
            } else if (nodeKind == "ComponentInstance") {
                showAllComponents();
            } else if (nodeKind == "ComponentImpl") {
                hideImplementedComponents();
            }
        } else if (nodeKind == "BehaviourDefinitions") {
            hideImplementedComponents();
        } else {
            // make sure all the Components are shown for all the other allowed kinds
            showAllComponents();
        }
    }
}


/**
 * @brief DefinitionsDockScrollArea::nodeConstructed
 * This gets called whenever a node has been constructed.
 * It checks to see if a dock item needs to be constucted for the new node.
 * @param node
 */
void DefinitionsDockScrollArea::nodeConstructed(NodeItem *nodeItem)
{
    if (nodeItem->getNodeKind() == "Component") {

        DockNodeItem* dockItem = new DockNodeItem("", nodeItem, this);
        NodeItem* fileItem = nodeItem->getParentNodeItem();
        if (!fileItem) {
            qCritical() << "COMPONENT HAS NO PARENT";
            return;
        }

        QString fileID = fileItem->getID();

        // check if there is already a layout and label for the parent File
        if (!fileLayoutItems.contains(fileID)){
            // create a new File label and add it to the File's layout
            DockNodeItem* fileDockItem = new DockNodeItem("FileLabel", fileItem, this);
            QVBoxLayout* fileLayout = new QVBoxLayout();

            fileLayout->addWidget(fileDockItem);


            fileLayoutItems[fileID] = fileLayout;
            addDockNodeItem(fileDockItem, -1, false);

            resortDockItems(fileDockItem);
            connect(fileDockItem, SIGNAL(dockItem_relabelled(DockNodeItem*)), this, SLOT(resortDockItems(DockNodeItem*)));
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
            resortDockItems(dockItem);
            connect(dockItem, SIGNAL(dockItem_relabelled(DockNodeItem*)), this, SLOT(resortDockItems(DockNodeItem*)));
        }
    }

    refreshDock();
}



/**
 * @brief DefinitionsDockScrollArea::refreshDock
 */
void DefinitionsDockScrollArea::refreshDock()
{
    showAllComponents();
    updateDock();
}


/**
 * @brief DefinitionsDockScrollArea::resortDockItems
 * @param dockItem
 */
void DefinitionsDockScrollArea::resortDockItems(DockNodeItem *dockItem)
{
    if (!dockItem) {
        return;
    }

    DockNodeItem* parentDockItem = dockItem->getParentDockNodeItem();

    QString ID = dockItem->getID();
    QString newDockItemLabel = dockItem->getLabel();
    bool isFileLabel = dockItem->isFileLabel();

    QVBoxLayout* layoutToSort = 0;

    //It's either a file or a Component.
    if (isFileLabel) {
        //If it's a file, Remove the FileLayout from the Items Layout
        layoutToSort = itemsLayout;
        QVBoxLayout* fileLayout = fileLayoutItems[ID];
        if (fileLayout) {
            layoutToSort->removeItem(fileLayout);
        }
    } else {
        //If it's a component, Remove the component from the File Layout.
        if (parentDockItem) {
            QString parentID = parentDockItem->getID();
            layoutToSort = fileLayoutItems[parentID];
            if (layoutToSort) {
                layoutToSort->removeWidget(dockItem);
            }
        }
    }

    if(!layoutToSort){
        qWarning() << "DefinitionsDockScrollArea::resortDockItems - Layout to sort is null.";
        return;
    }

    // iterate through the items in this dock's layout
    for (int i = 0; i < layoutToSort->count(); i++) {

        QLayoutItem* layoutItem = layoutToSort->itemAt(i);
        QString dockItemLabel;

        if (isFileLabel) {
            //Get the File ID of the layout Item
            QVBoxLayout* fileLayout = dynamic_cast<QVBoxLayout*>(layoutItem);
            if (fileLayout) {
                QString fileID = fileLayoutItems.key(fileLayout, "");
                if (fileID != "") {
                    DockNodeItem* dockItem = getDockNodeItem(fileID);
                    if (dockItem) {
                        dockItemLabel = dockItem->getLabel();
                    }
                }
            }
        } else {
            //Get the Component ID of the layout item Widget.
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
        if (newDockItemLabel.compare(dockItemLabel, Qt::CaseInsensitive) <= 0) {
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
        if (dockItem) {
            QString ID = dockItem->getID();
            if (getNodeView() && getNodeView()->getImplementation(ID)) {
                dockItem->setHidden(true);
            }
        }
    }
}


/**
 * @brief DefinitionsDockScrollArea::showAllComponents
 * This is called everytime a node item is selected.
 * It shows all the Components in this dock.
 */
void DefinitionsDockScrollArea::showAllComponents()
{

    foreach (DockNodeItem* dockItem, getDockNodeItems()) {
        dockItem->setHidden(false);
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
