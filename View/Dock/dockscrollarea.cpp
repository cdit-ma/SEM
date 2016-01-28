#include "dockscrollarea.h"
#include "docktogglebutton.h"
#include "docknodeitem.h"

#include "definitionsdockscrollarea.h"
#include "hardwaredockscrollarea.h"

#include <QDebug>
#include <qmath.h>

#define MAX_LABEL_LENGTH 12
#define BUTTON_WIDTH 131


/**
 * @brief DockScrollArea::DockScrollArea
 * @param label
 * @param view
 * @param parent
 * @param dockEmptyText
 */
DockScrollArea::DockScrollArea(QString label, NodeView* view, DockToggleButton* parent, QString dockEmptyText) :
    QScrollArea(parent)
{
    if (!view || !parent) {
        qWarning() << "DockScrollArea::DockScrollArea - NodeView or parent DockToggleButton is null.";
        return;
    }

    nodeView = view;
    parentButton = 0;

    this->label = label;

    currentNodeItem = 0;
    currentNodeItemID = -1;

    dockOpen = false;
    infoLabelVisible = true;

    defaultInfoText = dockEmptyText;
    infoText = defaultInfoText;

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
        currentNodeItemID = -1;
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
    if (currentNodeItemID != -1) {
        return currentNodeItem;
    }
    return 0;
}


/**
 * @brief DockScrollArea::getCurrentNodeKind
 * @return
 */
QString DockScrollArea::getCurrentNodeKind()
{
   if (getCurrentNodeItem()) {
        return getCurrentNodeItem()->getNodeKind();
   }
   return "";
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
 * @brief DockScrollArea::getDockType
 * @return
 */
DOCK_TYPE DockScrollArea::getDockType()
{
    if (parentButton) {
        return parentButton->getDockType();
    }
    return UNKNOWN_DOCK;
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
 * @brief DockScrollArea::getLayout
 * @return
 */
QVBoxLayout* DockScrollArea::getLayout()
{
    return layout;
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
    if (nodeView && currentNodeItemID != -1) {
        return nodeView->getAdoptableNodeList(currentNodeItemID);
    }
    return QStringList();
}


/**
 * @brief DockScrollArea::setInfoText
 * @param text
 */
void DockScrollArea::setInfoText(QString text)
{
    if (!text.isEmpty()) {

        QStringList textList = text.split(" ");
        int lineWidth=0;
        text = "";

        foreach (QString s, textList) {
            int sWidth = infoLabel->fontMetrics().width(s + " ");
            if ((lineWidth + sWidth) < BUTTON_WIDTH) {
                text += s + " ";
                lineWidth += sWidth;
            } else {
                text += "<br/>" + s + " ";
                lineWidth = sWidth;
            }
        }

        text.truncate(text.length() - 1);
    }

    infoText = text;
    infoLabel->setText(infoText);
}


/**
 * @brief DockScrollArea::hasVisibleItems
 * @return
 */
bool DockScrollArea::hasVisibleItems()
{
    foreach (DockNodeItem* item, getDockNodeItems()) {
        if (!item->isHidden() && !item->isForceHidden()) {
            return true;
        }
    }
    return false;
}


/**
 * @brief DockScrollArea::nodeDeleted
 * @param nodeID
 */
void DockScrollArea::nodeDeleted(QString nodeID)
{
    removeDockNodeItem(getDockNodeItem(nodeID), true);
}


/**
 * @brief DockScrollArea::addDockNodeItem
 * This adds a new dock item to this dock's list and layout.
 * It also connects the dock item's basic signals to this dock.
 * @param dockItem
 * @param insertIndex
 * @param addToLayout
 */
void DockScrollArea::addDockNodeItem(DockNodeItem* dockItem, int insertIndex, bool addToLayout)
{
    if (dockItem) {

        QString dockItemID = dockItem->getID();

        // if the dock already contains the item, do nothing
        if (getDockNodeItem(dockItemID)) {
            return;
        }

        if (addToLayout) {
            if (insertIndex == -1) {
                layout->addWidget(dockItem);
            } else {
                layout->insertWidget(insertIndex, dockItem);
            }
        }

        dockNodeIDs.append(dockItemID.toInt());
        dockNodeItems[dockItemID] = dockItem;
        connect(dockItem, SIGNAL(dockItem_clicked()), this, SLOT(dockNodeItemClicked()));

    } else {
        qWarning() << "DockScrollArea::addDockNodeItem - Item is null.";
    }

    updateInfoLabel();
}


/**
 * @brief DockScrollArea::removeDockNodeItemFromList
 * This method removes the provided dock item from this dock's list and layout.
 * @param dockItem
 */
void DockScrollArea::removeDockNodeItem(DockNodeItem* dockItem, bool deleteItem)
{
    if (!dockItem || !getDockNodeItem(dockItem->getID())) {
        qWarning() << "DockScrollArea::removeDockNodeItem - Trying to remove a null item or item doesn't exist in this dock.";
        return;
    }

    QString dockItemID = dockItem->getID();
    dockNodeItems.remove(dockItemID);
    dockNodeIDs.removeAll(dockItemID.toInt());

    if (layout) {
        layout->removeWidget(dockItem);
    }

    if (deleteItem) {
        dockItem->deleteLater();
    }

    updateInfoLabel();
}


/**
 * @brief DockScrollArea::getDockNodeItem
 * This checks if this dock already contains a dock node item attached to nodeID.
 * DockNodeItems are indexed by their kind instead of their ID for the parts list.
 * @param dockItemID
 * @return
 */
DockNodeItem* DockScrollArea::getDockNodeItem(QString dockItemID)
{
    foreach (QString itemID, dockNodeItems.keys()) {
        if (itemID == dockItemID) {
            return dockNodeItems[itemID];
        }
    }

    if (dockNodeItems.contains(dockItemID)) {
        return dockNodeItems[dockItemID];
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
        getParentButton()->setEnabled(enabled);
    }
}


/**
 * @brief DockScrollArea::isDockOpen
 * @return
 */
bool DockScrollArea::isDockOpen()
{
    return dockOpen;
}


/**
 * @brief DockScrollArea::setDockOpen
 * @param open
 */
void DockScrollArea::setDockOpen(bool open)
{
    dockOpen = open;
    setVisible(open);

    if (open) {
        emit dock_opened();
    } else {
        emit dock_closed();
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
    // TODO - Get and use list of allowed kinds instead!
    if (currentNodeItemID != -1) {
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
 * @brief DockScrollArea::onNodeDeleted
 * This tells the dock if a node has been deleted.
 * It either updates dock for the selected node or it disables this dock.
 * @param nodeID
 * @param parentID
 */
void DockScrollArea::onNodeDeleted(int nodeID, int parentID)
{
    if (dockNodeIDs.contains(nodeID)) {
        nodeDeleted(QString::number(nodeID));
    }

    // need to update on all these cases to enable/disable the dock correctly
    if (!nodeView->getSelectedNodeItem() || nodeID == getCurrentNodeID() || parentID == getCurrentNodeID()) {
        updateCurrentNodeItem();
    }
}


/**
 * @brief DockScrollArea::onEdgeDeleted
 * This is called when an edge has been deleted. This is needed to update the displayed items
 * in case the edge that has been deleted is linked to what is currently being displayed.
 * @param srcID
 * @param dstID
 */
void DockScrollArea::onEdgeDeleted(int srcID, int dstID)
{
    bool forceRefreshDock = (srcID == -1) && (dstID == -1);
    if (forceRefreshDock || dockNodeIDs.contains(srcID) || dockNodeIDs.contains(dstID)) {
        updateDock();
    }
}


/**
 * @brief DockScrollArea::getCurrentNodeID
 * This method returns the current node item's ID.
 * @return "-1" - current node item was deleted/no selected node item
 */
int DockScrollArea::getCurrentNodeID()
{
    return currentNodeItemID;
}


/**
 * @brief DockScrollArea::setupLayout
 * This sets up the groupbox widget contained inside this dock's scroll area.
 * It sets up the visual layout, size and alignment of things.
 */
void DockScrollArea::setupLayout()
{
    infoLabel = new QLabel(this);
    infoLabel->setTextFormat(Qt::RichText);
    infoLabel->setAlignment(Qt::AlignCenter);
    infoLabel->setFixedWidth(BUTTON_WIDTH + 10);
    infoLabel->setStyleSheet("padding: 10px; font-style: italic;");
    setInfoText(defaultInfoText);

    QGroupBox* groupBox = new QGroupBox(0);
    groupBox->setTitle(label);
    groupBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    groupBox->setStyleSheet("QGroupBox {"
                            "background-color: rgba(255,255,255,0);"
                            "border: 0px;"
                            "padding: 10px;"
                            "}");

    layout = new QVBoxLayout(this);
    layout->setSpacing(2);
    layout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    layout->setSizeConstraint(QLayout::SetMinimumSize);
    layout->addWidget(infoLabel);
    groupBox->setLayout(layout);

    setWidget(groupBox);
    setVisible(false);
    setWidgetResizable(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setStyleSheet("QScrollArea {"
                  "padding: 10px 0px;"
                  "background-color: rgba(250,250,250,240);"
                  "border: 0px;"
                  //"border-radius: 10px;"
                  "}");
}


/**
 * @brief DockScrollArea::setParentButton
 * Attach and connect this dock to its parent button.
 */
void DockScrollArea::setParentButton(DockToggleButton *parent)
{
    if (!parent) {
        qWarning() << "DockScrollArea::setParentButton - Parent is null.";
        return;
    }
    parentButton = parent;
    parentButton->setDock(this);
}


/**
 * @brief DockScrollArea::clear
 * This method clears/deletes the nodes from this container.
 */
void DockScrollArea::clear()
{
    clearSelected();

    while (!dockNodeItems.keys().isEmpty()) {
        QString dockItemID = dockNodeItems.keys().first();
        DockNodeItem* item =  dockNodeItems.take(dockItemID);
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
    currentNodeItemID = -1;
}


/**
 * @brief DockScrollArea::parentHeightChanged
 * @param height
 */
void DockScrollArea::parentHeightChanged(double height)
{
    resize(width(), height);
}


/**
 * @brief DockScrollArea::updateInfoLabel
 */
void DockScrollArea::updateInfoLabel()
{
    bool showLabel = !hasVisibleItems();
    if (showLabel != infoLabelVisible) {
        infoLabel->setVisible(showLabel);
        infoLabelVisible = showLabel;
    }
}

