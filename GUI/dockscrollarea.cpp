#include "dockscrollarea.h"
#include "docktogglebutton.h"
#include "docknodeitem.h"
#include "dockadoptablenodeitem.h"

#include <QScrollBar>
#include <QDebug>


/**
 * @brief DockScrollArea::DockScrollArea
 * @param title
 * @param parent
 */
DockScrollArea::DockScrollArea(QString label, DockToggleButton *parent) :
    QScrollArea(parent)
{
    layout = new QVBoxLayout(this);
    groupBox = new QGroupBox(0);
    parentButton = parent;
    this->label = label;
    activated = false;

    // attach scroll area and groupbox to their parent button
    parentButton->setContainer(this);

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
                  "background-color: rgba(255,255,255,180);"
                  "border: 0px;"
                  "border-radius: 10px;"
                  "padding-top: 10px;"
                  "}");

    // this centers the groupbox and dock node items
    setAlignment(Qt::AlignHCenter);

    // connect this container to its dock toglle button
    connect(parentButton, SIGNAL(pressed()), this, SLOT(activate()));
}


/**
 * @brief DockScrollArea::~DockScrollArea
 */
DockScrollArea::~DockScrollArea()
{

}


/**
 * @brief DockScrollArea::getParentButton
 * Returns this scroll area's parent button (DockToggleButton).
 * @return
 */
DockToggleButton *DockScrollArea::getParentButton()
{
    return parentButton;
}


/**
 * @brief DockScrollArea::addNode
 * @param buttonName
 * @param nodeName
 */
void DockScrollArea::addDockNode(NodeItem* item)
{
    DockNodeItem *itm = new DockNodeItem(item, this);

    dockNodes.append(itm);
    layout->addWidget(itm);
    itm->setContainer(this);

    connect(itm, SIGNAL(removeFromDockNodeList(QWidget*)), this,
            SLOT(removeFromDockNodeList(QWidget*)));
    connect(itm, SIGNAL(dockNode_addComponentInstance(NodeItem*)),
            this, SLOT(dock_addComponentInstance(NodeItem*)));

}


/**
 * @brief DockScrollArea::addNodes
 * Add adoptable node items to dock.
 * This creates a groupbox for each adoptable node kind
 * which contains the new dock node item and its label.
 * @param nodes
 */
void DockScrollArea::addAdoptableDockNodes(Node* node, QStringList nodes)
{
    clear();
    nodes.sort();

    parentNode = node;

    for (int i=0; i < nodes.count(); i++) {
        DockAdoptableNodeItem *itm = new DockAdoptableNodeItem(nodes.at(i), this);
        layout->addWidget(itm);
        dockNodes.append(itm);
        connect(itm, SIGNAL(itemPressed(QString)), this, SLOT(buttonPressed(QString)));
    }

    //checkScrollBar();
    checkDockNodesList();
    repaint();
}


/**
 * @brief DockScrollArea::checkDockNodesList
 */
void DockScrollArea::checkDockNodesList()
{
    qDebug() << "dockNodes.count() = " << dockNodes.count();
    if (parentButton && dockNodes.count() == 0) {
        parentButton->hideContainer();
        parentButton->setEnabled(false);
    }
    qDebug() << "------------------------------";
 }


/**
 * @brief DockScrollArea::paintEvent
 * @param e
 */
void DockScrollArea::paintEvent(QPaintEvent *e)
{
    //qDebug() << verticalScrollBar()->isVisible();
    QScrollArea::paintEvent(e);
}


/**
 * @brief DockScrollArea::buttonPressed
 * This gets called when a dock adoptable node item is pressed.
 * It tells the view to create a NodeItem with the specified
 * kind inside the currently selected node.
 * @param kind
 */
void DockScrollArea::buttonPressed(QString kind)
{
    emit constructDockNode(parentNode, kind);
}


/**
 * @brief DockScrollArea::dock_addComponentInstance
 * Tell the view to try and create a ComponentInstance of
 * NodeItem itm and add it into the currently selected node.
 */
void DockScrollArea::dock_addComponentInstance(NodeItem *itm)
{
    emit trigger_addComponentInstance(itm);
}


/**
 * @brief DockScrollArea::removeFromDockNodeList
 * @param widget
 */
void DockScrollArea::removeFromDockNodeList(QWidget *widget)
{
    if (dockNodes.contains(widget)) {
        dockNodes.removeAt(dockNodes.indexOf(widget));
    }
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
        //checkDockNodesList();
    }
}


/**
 * @brief DockScrollArea::clear
 * This method clears/deletes the nodes from this container.
 */
void DockScrollArea::clear()
{
    for (int i=0; i<dockNodes.count(); i++) {
        layout->removeWidget(dockNodes.at(i));
        delete dockNodes.at(i);
    }

    dockNodes.clear();
}
