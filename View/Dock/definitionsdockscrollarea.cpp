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
    definitions_notAllowedKinds.append("File");
    definitions_notAllowedKinds.append("Component");
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
 * @brief DefinitionsDockScrollArea::dock_itemClicked
 * When an item in this dock is clicked, it tries to either add a ComponentInstance
 * or connect a ComponentInstance to the clicked Component if that action is allowed.
 * If a ComponentImpl is selected, it tries to connect it to the clicked Component.
 */
void DefinitionsDockScrollArea::dockNodeItemClicked()
{
    Node* selectedNode = getNodeView()->getSelectedNode();
    DockNodeItem* sender = qobject_cast<DockNodeItem*>(QObject::sender());

    if (selectedNode) {
        Node* dockNode = sender->getNodeItem()->getNode();
        QString selectedNodeKind = selectedNode->getDataValue("kind");
        if (selectedNodeKind == "ComponentAssembly") {
            getNodeView()->constructConnectedNode(selectedNode, dockNode, "ComponentInstance", 0);
        } else if (selectedNodeKind == "ComponentInstance" || selectedNodeKind == "ComponentImpl") {
            getNodeView()->constructEdge(selectedNode, dockNode);
        } else if (selectedNodeKind == "BehaviourDefinitions") {
            getNodeView()->constructConnectedNode(selectedNode, dockNode, "ComponentImpl", 0);
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

    // special cases - ComponentInstance & ComponentImpl
    // they're only allowed kinds if they don't have a definition
    if (getCurrentNodeItem()) {
        QString nodeKind =  getCurrentNodeItem()->getNodeKind();
        if (nodeKind == "ComponentInstance" || nodeKind == "ComponentImpl") {
            Node* node = getCurrentNodeItem()->getNode();
            if (node->getDefinition()) {
                getParentButton()->enableDock(false);
                return;
            } else if (nodeKind == "ComponentImpl") {
                // if the selected item is a ComponentImpl without a definition,
                // hide Components that already have an implementation
                hideImplementedComponents();
            }
        } else if (nodeKind == "BehaviourDefinitions") {
            // if the selected item is the BehaviourDefinitions,
            // hide Components that already have an implementation
            hideImplementedComponents();
        } else {
            // make sure all Components are shown for all the other allowed kinds
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

        // check if there is already a layout and label for the parent File
        if (!fileLayoutItems[fileItem]) {

            // create a new File label and add it to the File's layout
            DockNodeItem* fileDockItem = new DockNodeItem("FileLabel", fileItem, this);
            fileLayoutItems[fileItem] = new QVBoxLayout();
            fileLayoutItems[fileItem]->addWidget(fileDockItem);

            addDockNodeItem(fileDockItem, false);
            resortDockItems(fileDockItem);
            connect(fileDockItem, SIGNAL(dockItem_relabelled(DockNodeItem*)), this, SLOT(resortDockItems(DockNodeItem*)));
        }

        // connect the new dock item to its parent file item
        DockNodeItem* parentItem = getDockNodeItem(fileItem);
        if (parentItem) {
            dockItem->setParentDockNodeItem(parentItem);
            parentItem->addChildDockItem(dockItem);
        }

        // add new dock item to its parent's File's layout
        fileLayoutItems[fileItem]->addWidget(dockItem);
        addDockNodeItem(dockItem, false);
        resortDockItems(dockItem);
        connect(dockItem, SIGNAL(dockItem_relabelled(DockNodeItem*)), this, SLOT(resortDockItems(DockNodeItem*)));
    }

    refreshDock();
}


/**
 * @brief DefinitionsDockScrollArea::nodeDestructed
 * This gets called when a node has been destructed.
 * If it's of type Component, this checks if the Component's parent File no longer
 * has any children. If it doesn't remove the File's layout and label from this dock.
 * @param nodeItem
 */
void DefinitionsDockScrollArea::nodeDestructed(NodeItem *nodeItem)
{
    if (nodeItem->getNodeKind() == "Component") {

        DockNodeItem* dockItem = getDockNodeItem(nodeItem);

        if (dockItem) {

            NodeItem* fileItem = nodeItem->getParentNodeItem();
            QVBoxLayout* fileLayout = fileLayoutItems[fileItem];
            DockNodeItem* fileDockItem = getDockNodeItem(fileItem);

            // if the node item to be deleted is the last child node item of fileItem,
            // remove the file layout & label from their lists and then delete them
            if (fileItem->getChildNodeItems().count() == 1) {
                if (fileDockItem) {
                    // remove and delete the file label
                    removeDockNodeItemFromList(fileDockItem);
                    delete fileDockItem;
                }
                // remove and delete the file layout
                fileLayoutItems.remove(fileItem);
                itemsLayout->removeItem(fileLayout);
                delete fileLayout;

            } else {
                // otherwise, just remove the dock item from its layout and the File's list
                fileLayout->removeWidget(dockItem);
                fileDockItem->removeChildDockItem(dockItem);
            }

            // remove and delete the dock item
            removeDockNodeItemFromList(dockItem);
            delete dockItem;
        }
    }

    // if the destructed node was the previously selected, clear this donc's selection
    if (!getNodeView()->getSelectedNode()) {
        DockScrollArea::updateCurrentNodeItem(0);
    }
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
    if (dockItem) {

        NodeItem* dockNodeItem = dockItem->getNodeItem();
        bool isFileLabel = dockItem->isFileLabel();

        QVBoxLayout* layout = 0;

        // remove the dock item from its layout before inserting it to the right spot
        if (getDockNodeItem(dockNodeItem)) {
            if (isFileLabel) {
                layout = itemsLayout;
                layout->removeItem(fileLayoutItems[dockNodeItem]);
            } else {
                DockNodeItem* parentDockItem = dockItem->getParentDockNodeItem();
                if (parentDockItem) {
                    layout = fileLayoutItems[parentDockItem->getNodeItem()];
                    layout->removeWidget(dockItem);
                }
            }
        }

        QString dockItemLabel = dockNodeItem->getNode()->getDataValue("label");
        int count = layout->count();
        bool dockItemInserted = false;

        // iterate through the items in this dock's layout
        for (int i = 0; i < count; i++) {

            NodeItem* currentNodeItem = 0;
            if (isFileLabel) {
                currentNodeItem = fileLayoutItems.key((QVBoxLayout*)layout->itemAt(i));
            } else {
                DockNodeItem* currentDockItem = dynamic_cast<DockNodeItem*>(layout->itemAt(i)->widget());
                if (currentDockItem) {
                    if (currentDockItem->isFileLabel()) {
                        continue; // don't compare to the File label
                    }
                    currentNodeItem = currentDockItem->getNodeItem();
                }
            }

            // compare existing file names to the new file name
            // insert the new File into the correct alphabetical spot in the layout
            if (currentNodeItem) {

                QString currentDockItemLabel = currentNodeItem->getNode()->getDataValue("label");
                int compare = dockItemLabel.compare(currentDockItemLabel, Qt::CaseInsensitive);

                if (compare <= 0) {
                    if (isFileLabel) {
                        layout->insertLayout(i, fileLayoutItems[dockNodeItem]);
                    } else {
                        layout->insertWidget(i, dockItem);
                    }
                    dockItemInserted = true;
                    break;
                }
            }
        }

        // if there was no spot to insert the new File layout, add it to this dock's layout
        if (!dockItemInserted) {
            if (isFileLabel) {
                layout->addLayout(fileLayoutItems[dockNodeItem]);
            } else {
                layout->addWidget(dockItem);
            }
        }
    }

}


/**
 * @brief DefinitionsDockScrollArea::hideImplementedComponents
 * This method is called when a ComponentImpl that's not connected to a definition is selected.
 * It hides all the Components that are already connected to a ComponentImpl.
 */
void DefinitionsDockScrollArea::hideImplementedComponents()
{
    foreach (DockNodeItem* dockItem, getDockNodeItems()) {
        NodeItem* componentItem = dockItem->getNodeItem();
        if (componentItem->getNode()->getImplementations().count() > 0) {
            dockItem->setHidden(true);
        }
    }
}


/**
 * @brief DefinitionsDockScrollArea::showAllComponents
 * This is called everytime a node item is selected.
 * It initially shows all the Components this dock before updateDock() filters it.
 */
void DefinitionsDockScrollArea::showAllComponents()
{
    foreach (DockNodeItem* dockItem, getDockNodeItems()) {
        dockItem->setHidden(false);
    }
}
