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
        currentNodeItemID = currentNodeItem->getGraphML()->getID();
    } else {
        currentNodeItemID = "";
    }

    updateDock();

    /*
    //if(currentNodeItemID != "-1"){

    //}else{

    //}
    //if(currentNodeItemID)
    if(currentNodeItemID != "-1"){

        currentNodeItem = nodeView->getSelectedNodeItem();
        if(currentNodeItem){
            currentNodeItemID = currentNodeItem->getGraphML()->getID();
        }else{
            //currentNodeItemID = "";
        }
        qCritical() << currentNodeItemID;
    }else{
        updateDock();
    }
    */
}


/**
 * @brief DockScrollArea::getCurrentNodeItem
 * Returns the current node item.
 * @return
 */
NodeItem *DockScrollArea::getCurrentNodeItem()
{
    return currentNodeItem;
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
    return nodeView->getAdoptableNodeList(nodeView->getSelectedNode());
}


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
 * @param addToLayout
 */
void DockScrollArea::addDockNodeItem(DockNodeItem *item, bool addToLayout)
{
    dockNodeItems.append(item);

    if (addToLayout) {
        layout->addWidget(item);
    }

    connect(item, SIGNAL(dockItem_clicked()), this, SLOT(dockNodeItemClicked()));
    connect(item, SIGNAL(dockItem_removeFromDock(DockNodeItem*)), this, SLOT(removeDockNodeItemFromList(DockNodeItem*)));
}


/**
 * @brief DockScrollArea::getDockNodeItem
 * This checks if this dock already contains a dock node item attached to item.
 * @param item
 * @return
 */
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
}


/**
 * @brief DockScrollArea::getDockNodeItem
 * @param node
 * @return
 */
DockNodeItem *DockScrollArea::getDockNodeItem(Node *node)
{
    NodeItem* nodeItem = getNodeView()->getNodeItemFromNode(node);
    return getDockNodeItem(nodeItem);
}


/**
 * @brief DockScrollArea::getDockNodeItems
 * Returns the dock node items contained in this dock.
 * @return
 */
QList<DockNodeItem*> DockScrollArea::getDockNodeItems()
{
    return dockNodeItems;
}


/**
 * @brief DockScrollArea::updateDock
 * If the currently selected node kind is contained in notAllowedKinds,
 * it means that this dock can't be used for the selected node.
 * If so, disable this dock and its parentButton.
 * If currentNodeItem is NULL, it means that there is no selected node.
 */
void DockScrollArea::updateDock()
{
    //qDebug() << "updateDock(): currentID = " << currentNodeItemID;

    if (currentNodeItemID != "-1") {
        if (currentNodeItem) {
            if (notAllowedKinds.contains(currentNodeItem->getNodeKind())) {
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
 * @brief DockScrollArea::graphMLDestructed
 * @param ID
 */
void DockScrollArea::graphMLDestructed(QString ID)
{
    if (ID == currentNodeItemID) {
        currentNodeItemID = "-1";
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
    dockNodeItems.removeAll(item);
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
    layout = new QVBoxLayout(this);
    QGroupBox* groupBox = new QGroupBox(0);

    groupBox->setLayout(layout);
    groupBox->setTitle(label);
    groupBox->setFixedSize(140, 140);
    groupBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    groupBox->setStyleSheet("QGroupBox {"
                            "background-color: rgba(255,255,255,0);"
                            "border: 0px;"
                            "padding: 10px;"
                            "}");

    // find a better way to position the elements in the centre of the layout
    layout->setAlignment(Qt::AlignHCenter);
    layout->setSpacing(5);
    layout->setSizeConstraint(QLayout::SetMinimumSize);

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

    // this centers the groupbox and dock node items
    setAlignment(Qt::AlignHCenter);
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
 * @brief DockScrollArea::checkScrollBar
 * This still needs fixing.
 * isVisible() doesn't return the correct value.
 */
void DockScrollArea::checkScrollBar()
{
    if (verticalScrollBar()->isVisible()) {
        //qDebug() << "Vertical scroll bar is visible";
        setStyleSheet("QScrollArea {"
                      "background-color: rgba(255,255,255,180);"
                      "border: 0px;"
                      "border-radius: 10px;"
                      "padding-top: 10px;"
                      "padding-right: 5px;"
                      "}");
    } else {
        //qDebug() << "Vertical scroll bar is NOT visible";
        setStyleSheet("QScrollArea {"
                      "background-color: rgba(255,255,255,180);"
                      "border: 0px;"
                      "border-radius: 10px;"
                      "padding-top: 10px;"
                      "}");
    }
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
    for (int i=0; i<dockNodeItems.count(); i++) {
        layout->removeWidget(dockNodeItems.at(i));
        delete dockNodeItems.at(i);
    }
    dockNodeItems.clear();
}
