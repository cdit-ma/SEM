#include "dockscrollarea.h"
#include "docktogglebutton.h"
#include "docknodeitem.h"

#include "definitionsdockscrollarea.h"
#include "hardwaredockscrollarea.h"

#include <QDebug>
#include <qmath.h>

//#define DOCK_PADDING 5
#define DOCK_PADDING 10
#define MAX_LABEL_LENGTH 12
#define BUTTON_WIDTH 81
//#define BUTTON_WIDTH 131


/**
 * @brief DockScrollArea::DockScrollArea
 * @param type
 * @param view
 * @param parent
 * @param dockEmptyText
 */
DockScrollArea::DockScrollArea(DOCK_TYPE type, NodeView* view, DockToggleButton* parent, QString dockEmptyText) :
    QScrollArea(parent)
{
    if (!view) {
        qWarning() << "DockScrollArea::DockScrollArea - NodeView is null for dock: " << GET_DOCK_LABEL(type);
        return;
    }

    nodeView = view;

    currentNodeItem = 0;
    currentNodeItemID = -1;

    label = GET_DOCK_LABEL(type);
    dockType = type;

    OPEN = true;
    ENABLED = true;

    defaultInfoText = dockEmptyText;
    infoText = defaultInfoText;
    infoLabelVisible = true;

    setupLayout();
    setParentButton(parent);
    setDockOpen(false);

    // the definitions and functions dock have their own signal related to this
    switch (getDockType()) {
    case PARTS_DOCK:
    case HARDWARE_DOCK:
        connect(this, SIGNAL(dock_opened(bool)), this, SIGNAL(dock_toggled(bool)));
        connect(this, SIGNAL(dock_closed(bool)), this, SIGNAL(dock_toggled(bool)));
        break;
    default:
        break;
    }
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
    return dockType;
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
 * This sets the text that is displayed when this dock is empty.
 * @param text
 */
void DockScrollArea::setInfoText(QString text)
{
    if (!text.isEmpty()) {

        // break up the text so that it fits inside the dock
        QStringList textList = text.split(" ");
        int lineWidth = 0;
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
        infoText = text;
        infoLabel->setText(infoText);
    }
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
 * @brief DockScrollArea::removeDockNodeItem
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
 * If this dock is attached to a DockToggleButton, it returns whether or not that button is enabled.
 * @return
 */
bool DockScrollArea::isDockEnabled()
{
    return ENABLED;
}


/**
 * @brief DockScrollArea::setDockEnabled
 * If this dock is attached to a DockToggleButton, it enables/disables that button.
 * @param enabled
 */
void DockScrollArea::setDockEnabled(bool enabled)
{
    ENABLED = enabled;
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
    return OPEN;
}


/**
 * @brief DockScrollArea::setDockOpen
 * This opens/closes this dock.
 * @param open
 */
void DockScrollArea::setDockOpen(bool open)
{
    if ((!ENABLED && open) || (open == OPEN)) {
        return;
    }

    OPEN = open;
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
 * If so, disable this dock's parentButton.
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
    infoLabel->setFixedWidth(BUTTON_WIDTH + DOCK_PADDING*2);
    infoLabel->setStyleSheet("font-style: italic;"
                             "padding:" + QString::number(DOCK_PADDING) + "px "
                             + QString::number(DOCK_PADDING/2) + "px; ");

    QGroupBox* groupBox = new QGroupBox(0);
    groupBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    groupBox->setStyleSheet("QGroupBox {"
                            "background-color: rgba(0,0,0,0);"
                            "margin: 0px 18px;"
                            "border: 0px;"
                            "padding: 0px;"
                            "}");

    layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    layout->setSizeConstraint(QLayout::SetMinimumSize);
    layout->addWidget(infoLabel);
    groupBox->setLayout(layout);

    setWidget(groupBox);
    setWidgetResizable(true);
    setInfoText(defaultInfoText);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setStyleSheet("QScrollArea {"
                  "padding: 0px 0px 10px 0px;"
                  "background: rgba(250,250,250,240);"
                  //"border: none;"
                  "border-left: 1px solid rgb(125,125,125);"
                  "border-right: 1px solid rgb(125,125,125);"
                  "border-bottom: 1px solid rgb(125,125,125);;"
                  //"border-bottom: none;"
                  "border-top: none;"
                  "}");
}


/**
 * @brief DockScrollArea::setParentButton
 * Attach and connect this dock to its parent button.
 */
void DockScrollArea::setParentButton(DockToggleButton* parent)
{
    parentButton = parent;
    if (parent) {
        parentButton->setDock(this);
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
        if (infoLabelVisible && infoLabel->text().isEmpty()) {
            infoLabel->setText("Info label text is empty!");
        }
    }
}

