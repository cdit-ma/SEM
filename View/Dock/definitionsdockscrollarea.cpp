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

    if (getCurrentNodeItem()) {

        QString nodeKind =  getCurrentNodeItem()->getNodeKind();

        // check for special cases - ComponentInstance & ComponentImpl
        // they're only allowed kinds if they don't have a definition
        // if the selected item is the BehaviourDefinitions or a ComponentImpl without
        // a definition, hide Components that already have an implementation

        if (nodeKind == "ComponentInstance" || nodeKind == "ComponentImpl") {
            Node* node = getCurrentNodeItem()->getNode();
            if (node->getDefinition()) {
                getParentButton()->enableDock(false);
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

        // check if there is already a layout and label for the parent File
        if (!fileLayoutItems[fileItem]) {
            // create a new File label and add it to the File's layout
            DockNodeItem* fileDockItem = new DockNodeItem("FileLabel", fileItem, this);
            fileLayoutItems[fileItem] = new QVBoxLayout();
            fileLayoutItems[fileItem]->addWidget(fileDockItem);

            addDockNodeItem(fileDockItem, -1, false);
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
        addDockNodeItem(dockItem, -1, false);
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

    // if the destructed node was selected before it was destroyed, clear this dock's selection
    if (!getNodeView()->getSelectedNode()) {
        DockScrollArea::updateCurrentNodeItem();
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
    NodeItem* dockNodeItem = dockItem->getNodeItem();
    bool isFileLabel = dockItem->isFileLabel();
    QVBoxLayout* layout = 0;

    // remove the dock item from its layout before re-inserting it to the right spot
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

    QString dockItemLabel = dockItem->getLabel();

    // iterate through the items in this dock's layout
    for (int i = 0; i < layout->count(); i++) {

           QString currentDockItemLabel;
           if (isFileLabel) {
               NodeItem* currentNodeItem = fileLayoutItems.key((QVBoxLayout*)layout->itemAt(i));
               if (currentNodeItem) {
                   currentDockItemLabel = currentNodeItem->getNode()->getDataValue("label");
               }
           } else {
               DockNodeItem* currentDockItem = dynamic_cast<DockNodeItem*>(layout->itemAt(i)->widget());
               if (currentDockItem && !currentDockItem->isFileLabel()) {
                   currentDockItemLabel = currentDockItem->getLabel();
               }
           }

           // if for some reason the label is empty, skip to the next item
           if (currentDockItemLabel.isEmpty()) {
               continue;
           }

           // compare existing file names to the new file name
           // insert the new File into the correct alphabetical spot in the layout
           int compare = dockItemLabel.compare(currentDockItemLabel, Qt::CaseInsensitive);
           if (compare <= 0) {
               if (isFileLabel) {
                   layout->insertLayout(i, fileLayoutItems[dockNodeItem]);
               } else {
                   layout->insertWidget(i, dockItem);
               }
               return;
           }
    }

    // if there was no spot to insert the new File layout, add it to the end of the layout
    if (isFileLabel) {
        layout->addLayout(fileLayoutItems[dockNodeItem]);
    } else {
        layout->addWidget(dockItem);
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
        NodeItem* componentItem = dockItem->getNodeItem();
        if (componentItem->getNode()->getImplementations().count() > 0) {
            dockItem->setHidden(true);
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
