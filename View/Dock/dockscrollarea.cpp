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
DockScrollArea::DockScrollArea(QString label, NodeView* view, DockToggleButton* parent) :
    QScrollArea(parent)
{
    nodeView = view;
    currentNodeItem = 0;

    this->label = label;
    dockOpen = false;

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
NodeItem* DockScrollArea::getCurrentNodeItem()
{
    if (currentNodeItemID != "") {
        return currentNodeItem;
    }
    return 0;
}


/**
 * @brief DockScrollArea::getParentButton
 * Returns this dock's parent button (DockToggleButton).
 * @return
 */
DockToggleButton* DockScrollArea::getParentButton()
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
NodeView* DockScrollArea::getNodeView()
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
    if (nodeView && currentNodeItemID != "") {
        return nodeView->getAdoptableNodeList(currentNodeItemID);
    }
    return QStringList();
}


/**
 * @brief DockScrollArea::onNodeDeleted
 * @param ID
 */
void DockScrollArea::onNodeDeleted(QString ID)
{
    if (dockNodeItems.contains(ID)) {
        DockNodeItem* item = dockNodeItems[ID];
        if (item) {
            removeDockNodeItemFromList(item);
            item->deleteLater();
        }
        updateDock();
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
 * This checks if this dock already contains a dock node item attached to nodeID.
 * @param nodeID
 * @return
 */
DockNodeItem *DockScrollArea::getDockNodeItem(QString nodeID)
{
    if (dockNodeItems.contains(nodeID)) {
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


/**
 * @brief DockScrollArea::isDockEnabled
 * @return
 */
bool DockScrollArea::isDockEnabled()
{
    if (getParentButton()) {
        return getParentButton()->isEnabled();
    }
    return false;
}


/**
 * @brief DockScrollArea::setDockEnabled
 * @param enabled
 */
void DockScrollArea::setDockEnabled(bool enabled)
{
    if (getParentButton()) {
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
    if (currentNodeItemID != "") {
        if (currentNodeItem) {
            if (currentNodeItem->getNodeKind() == "Model") {
                setDockEnabled(false);
            } else if (notAllowedKinds.contains(currentNodeItem->getNodeKind())) {
                setDockEnabled(false);
            } else {
                setDockEnabled(true);
            }
        } else {
            // no current node item selected
            setDockEnabled(false);
        }
    } else {
        // current node item deleted
        setDockEnabled(false);
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
    if (dockNodeIDs.contains(nodeID)) {
        onNodeDeleted(nodeID);
    }

    /*
    if (parentID == getCurrentNodeID()) {
        updateDock();
    } else if (nodeID == getCurrentNodeID()) {
        currentNodeItemID = "";
    }
    */

    // need to update on both cases to enable/disable the dock correctly
    if (nodeID == getCurrentNodeID() || parentID == getCurrentNodeID()) {
        updateCurrentNodeItem();
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
 * @brief DockScrollArea::removeDockNodeItemFromList
 * This is called whenever a DockNodeItem is deleted.
 * It removes the deleted item from this dock's list.
 * @param item
 */
void DockScrollArea::removeDockNodeItemFromList(DockNodeItem *item)
{
    if (item) {
        dockNodeItems.remove(item->getID());
        dockNodeIDs.removeAll(item->getID());
        if (layout) {
            layout->removeWidget(item);
        }
    }
}


/**
 * @brief DockScrollArea::getCurrentNodeID
 * This method returns the current node item's ID.
 * @return "" - current node item was deleted/no selected node item
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
    connect(parentButton, SIGNAL(pressed()), this, SLOT(on_parentButtonPressed()));
}


/**
 * @brief DockScrollArea::on_parentButtonPressed
 * This shows or hides the scroll area and its groupbox.
 */
void DockScrollArea::on_parentButtonPressed()
{
    if (dockOpen) {
        setVisible(false);
        dockOpen = false;
    } else {
        setVisible(true);
        dockOpen = true;
    }
}


/**
 * @brief DockScrollArea::clear
 * This method clears/deletes the nodes from this container.
 */
void DockScrollArea::clear()
{
    while (!dockNodeItems.keys().isEmpty()) {
        QString ID = dockNodeItems.keys().first();
        onNodeDeleted(ID);
    }

    dockNodeIDs.clear();
    dockNodeItems.clear();
    currentNodeItem = 0;
    currentNodeItemID = "";
}


/**
 * @brief DockScrollArea::parentHeightChanged
 * @param height
 */
void DockScrollArea::parentHeightChanged(double height)
{
    resize(width(), height);
}
