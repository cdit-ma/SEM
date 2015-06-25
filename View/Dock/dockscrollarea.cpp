#include "dockscrollarea.h"
#include "docktogglebutton.h"
#include "docknodeitem.h"
#include <QDebug>


/**
 * @brief DockScrollArea::DockScrollArea
 * @param label
 * @param view
 * @param parent
 */
DockScrollArea::DockScrollArea(QString label, NodeView* view, DockToggleButton *parent) :
    QScrollArea(parent)
{
    nodeView = view;
    currentNodeItem = 0;

    this->label = label;
    activated = false;

    setParentButton(parent);
    setupLayout();
}


/**
 * @brief DockScrollArea::setNotAllowedKinds
 * @param kinds
 */
void DockScrollArea::setNotAllowedKinds(QStringList kinds)
{
    notAllowedKinds = kinds;
}


/**
 * @brief DockScrollArea::updateCurrentNodeItem
 * This sets nodeItem to be the currently selected node item.
 */
void DockScrollArea::updateCurrentNodeItem()
{
    currentNodeItem = nodeView->getSelectedNodeItem();

    if (currentNodeItem) {
        currentNodeItemID = currentNodeItem->getID();
    } else {
        currentNodeItemID = "";
    }

    updateDock();
}


/**
 * @brief DockScrollArea::getCurrentNodeItem
 * Returns the current node item.
 * @return
 */
NodeItem *DockScrollArea::getCurrentNodeItem()
{
    if(currentNodeItemID != ""){
        return currentNodeItem;
    }
    return 0;
}


/**
 * @brief DockScrollArea::getParentButton
 * Returns this dock's parent button (DockToggleButton).
 * @return
 */
DockToggleButton *DockScrollArea::getParentButton()
{
    return parentButton;
}


/**
 * @brief DockScrollArea::getLabel
 * Returns this dock's label.
 * @return
 */
QString DockScrollArea::getLabel()
{
    return label;
}


/**
 * @brief DockScrollArea::getNodeView
 * Returns the NodeView this dock is attached to.
 * @return
 */
NodeView *DockScrollArea::getNodeView()
{
    return nodeView;
}


/**
 * @brief DockScrollArea::getAdoptableNodeListFromView
 * Returns the list of adoptable nodes for the currently
 * selected item from this dock's node view.
 * @return
 */
QStringList DockScrollArea::getAdoptableNodeListFromView()
{
    if(nodeView){
        if(currentNodeItemID != ""){
            return nodeView->getAdoptableNodeList(currentNodeItemID);
        }
    }
    return QStringList();
}


/**
 * @brief DockScrollArea::onNodeDeleted
 */
void DockScrollArea::onNodeDeleted(QString ID) {
    if(dockNodeItems.contains(ID)){
        DockNodeItem* item = dockNodeItems[ID];
        if(item){
            removeDockNodeItemFromList(item);
            if(layout){
                layout->removeWidget(item);
            }
            item->deleteLater();
        }
        this->update();
    }
}

/**
 * @brief DockScrollArea::onEdgeDeleted
 */
void DockScrollArea::onEdgeDeleted() {}


/**
 * @brief DockScrollArea::getLayout
 * @return
 */
QVBoxLayout *DockScrollArea::getLayout()
{
    return layout;
}


/**
 * @brief DockScrollArea::addDockNodeItem
 * This adds a new dock item to this dock's list and layout.
 * It also connects the dock item's basic signals to this dock.
 * @param item
 * @param insertIndex
 * @param addToLayout
 */
void DockScrollArea::addDockNodeItem(DockNodeItem *item, int insertIndex, bool addToLayout)
{
    if (item) {
        QString ID = item->getID();

        if(!dockNodeItems.contains(ID)){
            dockNodeItems[ID] = item;
        }
        if(!dockNodeIDs.contains(ID)){
             dockNodeIDs.append(ID);

        }

        if (addToLayout) {
            if (insertIndex == -1) {
                layout->addWidget(item);
            } else {
                layout->insertWidget(insertIndex, item);
            }
        }

        connect(item, SIGNAL(dockItem_clicked()), this, SLOT(dockNodeItemClicked()));
        connect(item, SIGNAL(dockItem_removeFromDock(DockNodeItem*)), this, SLOT(removeDockNodeItemFromList(DockNodeItem*)));

    } else {
        qWarning() << "DockScrollArea::addDockNodeItem - Item is null.";
    }
}


/**
 * @brief DockScrollArea::getDockNodeItem
 * This checks if this dock already contains a dock node item attached to item.
 * @param item
 * @return
 */
/*
DockNodeItem *DockScrollArea::getDockNodeItem(NodeItem *item)
{
    if (item) {
        foreach (DockNodeItem* dockItem, dockNodeItems) {
            if (dockItem->getNodeItem() == item) {
                return dockItem;
            }
        }
    }
    return 0;
}*/


/**
 * @brief DockScrollArea::getDockNodeItem
 * @param node
 * @return
 */
/*
DockNodeItem *DockScrollArea::getDockNodeItem(Node* node)
{
    NodeItem* nodeItem = getNodeView()->getNodeItemFromNode(node);
    return getDockNodeItem(nodeItem);
}
*/


/**
 * @brief DockScrollArea::getDockNodeItem
 * @param nodeID
 * @return
 */
DockNodeItem *DockScrollArea::getDockNodeItem(QString nodeID)
{
    if(dockNodeItems.contains(nodeID)){
        return dockNodeItems[nodeID];
    }
    return 0;
}


/**
 * @brief DockScrollArea::getDockNodeItems
 * Returns the dock node items contained in this dock.
 * @return
 */
QList<DockNodeItem*> DockScrollArea::getDockNodeItems()
{
    return dockNodeItems.values();
}

bool DockScrollArea::isDockOpen()
{
    if(getParentButton()){
        return getParentButton()->isEnabled();
    }
    return false;
}

void DockScrollArea::setDockEnabled(bool enabled)
{
    if(getParentButton()){
        getParentButton()->enableDock(enabled);
    }
}


/**
 * @brief DockScrollArea::updateDock
 * If the currently selected node kind is contained in notAllowedKinds,
 * it means that this dock can't be used for the selected node.
 * If so, disable this dock and its parentButton.
 */
void DockScrollArea::updateDock()
{
    if(currentNodeItemID != ""){
        if (currentNodeItem) {
            if (currentNodeItem->getNodeKind() == "Model") {
                parentButton->enableDock(false);
            } else if (notAllowedKinds.contains(currentNodeItem->getNodeKind())) {
                parentButton->enableDock(false);
            } else {
                parentButton->enableDock(true);
            }
        } else {
            // no current node item selected
            parentButton->enableDock(false);
        }
    } else {
        // current node item deleted
        parentButton->enableDock(false);
    }
}


/**
 * @brief DockScrollArea::nodeDeleted
 * This tells the dock if a node has been deleted.
 * It either updates dock for the selected node or it disables this dock.
 * @param nodeID
 * @param parentID
 */
void DockScrollArea::nodeDeleted(QString nodeID, QString parentID)
{



    if (parentID == getCurrentNodeID()) {
        updateDock();
    } else if (nodeID == getCurrentNodeID()) {
        currentNodeItemID = "";
    }

    if(dockNodeIDs.contains(nodeID)){
        onNodeDeleted(nodeID);
    }

}


/**
 * @brief DockScrollArea::edgeDeleted
 * @param srcID
 * @param dstID
 */
void DockScrollArea::edgeDeleted(QString srcID, QString dstID)
{
    if (dockNodeIDs.contains(srcID) || dockNodeIDs.contains(dstID)){
        onEdgeDeleted();
    }
}


/**
 * @brief DockScrollArea::paintEvent
 * Still trying to catch when the scrollbar appears/disappears to adjust dock size.
 * @param e
 */
void DockScrollArea::paintEvent(QPaintEvent *e)
{
    //qDebug() << verticalScrollBar()->isVisible();
    QScrollArea::paintEvent(e);
}


/**
 * @brief DockScrollArea::removeDockNodeItemFromList
 * This is called whenever a DockNodeItem is deleted.
 * It removes the deleted item from this dock's list.
 * @param item
 */
void DockScrollArea::removeDockNodeItemFromList(DockNodeItem *item)
{
    if(item){
        QString ID = item->getID();
        dockNodeItems.remove(ID);
        dockNodeIDs.removeAll(ID);
    }
}


/**
 * @brief DockScrollArea::getCurrentNodeID
 * This method returns the current node item's ID.
 * @return -1 = node item was deleted
 *         "" = no selected node item
 */
QString DockScrollArea::getCurrentNodeID()
{
    return currentNodeItemID;
}


/**
 * @brief DockScrollArea::dock_itemClicked
 * This is called whenever an item in this dock is clicked.
 */
void DockScrollArea::dockNodeItemClicked() {}


/**
 * @brief DockScrollArea::setupLayout
 * This sets up the groupbox widget contained inside this dock's scroll area.
 * It sets up the visual layout, size and alignment of things.
 */
void DockScrollArea::setupLayout()
{
    QGroupBox* groupBox = new QGroupBox(0);
    groupBox->setTitle(label);
    groupBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    groupBox->setStyleSheet("QGroupBox {"
                            "background-color: rgba(255,255,255,0);"
                            "border: 0px;"
                            "padding: 10px;"
                            "padding-left: 15px;"
                            "}");

    layout = new QVBoxLayout(this);
    layout->setSpacing(5);
    layout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    layout->setSizeConstraint(QLayout::SetMinimumSize);
    groupBox->setLayout(layout);

    setWidget(groupBox);
    setVisible(false);
    setWidgetResizable(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setStyleSheet("QScrollArea {"
                  "background-color: rgba(250,250,250,240);"
                  "border: 0px;"
                  "border-radius: 10px;"
                  "padding-top: 10px;"
                  "}");
}


/**
 * @brief DockScrollArea::setParentButton
 * Attach and connect this dock to its parent button.
 */
void DockScrollArea::setParentButton(DockToggleButton *parent)
{
    parentButton = parent;
    parentButton->setContainer(this);
    connect(parentButton, SIGNAL(pressed()), this, SLOT(activate()));
}


/**
 * @brief DockScrollArea::activate
 * This shows or hides the scroll area and its groupbox.
 */
void DockScrollArea::activate()
{
    if (activated) {
        setVisible(false);
        activated = false;
    } else {
        setVisible(true);
        activated = true;
    }
}


/**
 * @brief DockScrollArea::clear
 * This method clears/deletes the nodes from this container.
 */
void DockScrollArea::clear()
{
    while(!dockNodeItems.keys().isEmpty()){
        QString ID = dockNodeItems.keys().first();
        onNodeDeleted(ID);
    }

    dockNodeIDs.clear();
    dockNodeItems.clear();
    currentNodeItem = 0;
    currentNodeItemID ="";

}


/**
 * @brief DockScrollArea::parentHeightChanged
 */
void DockScrollArea::parentHeightChanged(double height)
{
    resize(width(), height);
}
