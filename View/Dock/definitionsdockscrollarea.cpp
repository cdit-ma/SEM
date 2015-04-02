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
    definitions_notAllowedKinds.append("BehaviourDefinitions");
    definitions_notAllowedKinds.append("DeploymentDefinitions");
    definitions_notAllowedKinds.append("AssemblyDefinitions");
    definitions_notAllowedKinds.append("HardwareDefinitions");
    definitions_notAllowedKinds.append("ManagementComponent");
    definitions_notAllowedKinds.append("HardwareCluster");
    definitions_notAllowedKinds.append("File");
    definitions_notAllowedKinds.append("Component");
    setNotAllowedKinds(definitions_notAllowedKinds);
}


/**
 * @brief DefinitionsDockScrollArea::dock_itemClicked
 * When an item in this dock is clicked, it tries to either add a ComponentInstance
 * or connect a ComponentInstance to an existing Component if that action is allowed.
 */
void DefinitionsDockScrollArea::dockNodeItemClicked()
{
    Node* selectedNode = getNodeView()->getSelectedNode();
    DockNodeItem* sender = qobject_cast<DockNodeItem*>(QObject::sender());

    if (selectedNode) {
        Node* dockNode = sender->getNodeItem()->getNode();
        if (selectedNode->getDataValue("kind") == "ComponentAssembly") {
            getNodeView()->constructComponentInstance(selectedNode, dockNode, 0);
        } else if (selectedNode->getDataValue("kind") == "ComponentInstance") {
            getNodeView()->view_ConstructEdge(selectedNode,  dockNode);
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
    // special case - ComponentInstance
    // it's only an allowed kind if it doesn't have a definition
    if (getCurrentNodeItem() && getCurrentNodeItem()->getNodeKind() == "ComponentInstance") {
        Node* inst = getCurrentNodeItem()->getNode();
        if (inst->getDefinition()) {
            getParentButton()->enableDock(false);
        }
        return;
    }

    DockScrollArea::updateDock();
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
            resortFileLayoutItems(fileDockItem);
            connect(fileDockItem, SIGNAL(dockItem_fileRelabelled(DockNodeItem*)), this, SLOT(resortFileLayoutItems(DockNodeItem*)));


            /*
            QString newFileName = fileItem->getNode()->getDataValue("label");
            bool layoutInserted = false;

            // iterate through the items in this dock's layout
            for (int i = 0; i < getLayout()->count(); i++) {

                NodeItem* currentFile = fileLayoutItems.key((QVBoxLayout*)getLayout()->itemAt(i));

                // compare existing file names to the new file name
                // insert the new File into the correct alphabetical spot in the layout
                if (currentFile) {
                    QString currentFileName = currentFile->getNode()->getDataValue("label");
                    int compare = newFileName.compare(currentFileName, Qt::CaseInsensitive);
                    if (compare <= 0) {
                        getLayout()->insertLayout(i, fileLayoutItems[fileItem]);
                        addDockNodeItem(fileDockItem, false);
                        layoutInserted = true;
                        break;
                    }
                }
            }

            // if there was no spot to insert the new File layout, add it to this dock's layout
            if (!layoutInserted) {
                getLayout()->addLayout(fileLayoutItems[fileItem]);
                addDockNodeItem(fileDockItem, false);
            }
            */


        }

        // connect the new dock item to its parent file item
        DockNodeItem* parentItem = getDockNodeItem(fileItem);
        if (parentItem) {
            dockItem->setParentDockNodeItem(parentItem);
        }

        // add new dock item to its parent's File's layout
        fileLayoutItems[fileItem]->addWidget(dockItem);
        addDockNodeItem(dockItem, false);
    }
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
        NodeItem* fileItem = nodeItem->getParentNodeItem();
        if (fileItem->getChildNodeItems().count() == 1) {
            DockNodeItem* fileDockItem = getDockNodeItem(fileItem);
            if (fileDockItem) {
                removeDockNodeItemFromList(fileDockItem);
                delete fileDockItem;
            }
            delete fileLayoutItems[fileItem];
            getLayout()->removeItem(fileLayoutItems[fileItem]);
            fileLayoutItems.remove(fileItem);
        }
    }
}


/**
 * @brief DefinitionsDockScrollArea::resortFileLayoutItems
 * @param fileItem
 */
void DefinitionsDockScrollArea::resortFileLayoutItems(DockNodeItem *fileItem)
{
    if (fileItem) {

        NodeItem* fileNodeItem = fileItem->getNodeItem();

        if (getDockNodeItem(fileNodeItem)) {
            getLayout()->removeItem(fileLayoutItems[fileNodeItem]);
        }

        QString filename = fileNodeItem->getNode()->getDataValue("label");
        bool layoutInserted = false;

        // iterate through the items in this dock's layout
        for (int i = 0; i < getLayout()->count(); i++) {

            NodeItem* currentFile = fileLayoutItems.key((QVBoxLayout*)getLayout()->itemAt(i));

            // compare existing file names to the new file name
            // insert the new File into the correct alphabetical spot in the layout
            if (currentFile) {
                QString currentFileName = currentFile->getNode()->getDataValue("label");
                int compare = filename.compare(currentFileName, Qt::CaseInsensitive);
                if (compare <= 0) {
                    getLayout()->insertLayout(i, fileLayoutItems[fileNodeItem]);
                    layoutInserted = true;
                    break;
                }
            }
        }

        // if there was no spot to insert the new File layout, add it to this dock's layout
        if (!layoutInserted) {
            getLayout()->addLayout(fileLayoutItems[fileNodeItem]);
        }
    }

}
