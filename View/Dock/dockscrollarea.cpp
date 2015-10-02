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
 * @brief DockScrollArea::getNotAllowedKinds
 * @return
 */
QStringList DockScrollArea::getNotAllowedKinds()
{
    return notAllowedKinds;
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
    removeDockNodeItem(getDockNodeItem(ID));
    updateDock();
}


/**
 * @brief DockScrollArea::onEdgeDeleted
 */
void DockScrollArea::onEdgeDeleted() {}


/**
 * @brief DockScrollArea::dockClosed
 */
void DockScrollArea::dockClosed(){}


/**
 * @brief DockScrollArea::getLayout
 * @return
 */
QVBoxLayout* DockScrollArea::getLayout()
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
void DockScrollArea::addDockNodeItem(DockNodeItem* item, int insertIndex, bool addToLayout)
{
    if (item) {

        QString itemID = item->getID();

        // if the dock already contains the item, do nothing
        if (getDockNodeItem(itemID)) {
            qCritical() << "AM I HAPPENING";
            return;
        }

        if (addToLayout) {
            if (insertIndex == -1) {
                layout->addWidget(item);
            } else {
                layout->insertWidget(insertIndex, item);
            }
        }

        dockNodeIDs.append(itemID);
        dockNodeItems[itemID] = item;
        connect(item, SIGNAL(dockItem_clicked()), this, SLOT(dockNodeItemClicked()));

    } else {
        qWarning() << "DockScrollArea::addDockNodeItem - Item is null.";
    }
}


/**
 * @brief DockScrollArea::getDockNodeItem
 * This checks if this dock already contains a dock node item attached to nodeID.
 * DockNodeItems are indexed by their kind instead of their ID for the parts list.
 * @param nodeID
 * @return
 */
DockNodeItem* DockScrollArea::getDockNodeItem(QString nodeID)
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
 * @brief DockScrollArea::removeDockNodeItemFromList
 * @param dockNodeItem
 */
void DockScrollArea::removeDockNodeItemFromList(DockNodeItem* dockNodeItem)
{
    dockNodeItems.remove(dockNodeItem->getID());
    dockNodeIDs.removeAll(dockNodeItem->getID());
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
        getParentButton()->setEnabled(enabled);
    }
}

bool DockScrollArea::isDockOpen()
{
    return dockOpen;
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
            if (notAllowedKinds.contains(currentNodeItem->getNodeKind())) {
                setDockEnabled(false);
            } else {
                setDockEnabled(true);
            }
        } else {
            // no current node item selected
            setDockEnabled(false);
        }
    } else {
        // current node item deleted or there is no current node item selected
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
 * @brief DockScrollArea::removeDockNodeItem
 * This is called whenever a DockNodeItem is deleted.
 * It removes the deleted item from this dock.
 * @param item
 */
void DockScrollArea::removeDockNodeItem(DockNodeItem* item)
{
    if (item) {
        dockNodeItems.remove(item->getID());
        dockNodeIDs.removeAll(item->getID());
        if (layout) {
            layout->removeWidget(item);
        }
        item->deleteLater();
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
        dockOpen = false;
        setVisible(false);
        dockClosed();
    } else {
        dockOpen = true;
        setVisible(true);
    }
}


/**
 * @brief DockScrollArea::clear
 * This method clears/deletes the nodes from this container.
 */
void DockScrollArea::clear()
{
    clearSelected();

    while (!dockNodeItems.keys().isEmpty()) {
        QString ID = dockNodeItems.keys().first();
        DockNodeItem* item =  dockNodeItems.take(ID);
        if (item) {
            item->deleteLater();;
        }
    }

    dockNodeIDs.clear();
    dockNodeItems.clear();
}

/**
 * @brief DockScrollArea::clearSelected
 */
void DockScrollArea::clearSelected()
{
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
