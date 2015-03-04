#include "dockscrollarea.h"
#include "docktogglebutton.h"
#include "docknodeitem.h"
#include "dockadoptablenodeitem.h"
#include "nodeview.h"
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
    parentButton = parent;

    this->label = label;
    activated = false;

    layout = new QVBoxLayout(this);
    groupBox = new QGroupBox(0);

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

    QString parentKind = parentButton->getKind();
    if (parentKind == "P") {

        //Empty means all kinds!

    } else if(parentKind == "H") {
        utilisedKinds << "HardwareDefinitions";
        utilisedKinds << "HardwareCluster";
        utilisedKinds << "ManagementComponent";
        utilisedKinds << "ComponentInstance";
        utilisedKinds << "ComponentAssembly";
    } else if(parentKind == "D") {
        utilisedKinds << "ComponentInstance";
        utilisedKinds << "ComponentAssembly";
    }
}


/**
 * @brief DockScrollArea::~DockScrollArea
 */
DockScrollArea::~DockScrollArea()
{

}

void DockScrollArea::setNodeView(NodeView *v)
{
    this->nodeView = v;
}

void DockScrollArea::setCurrentNodeItem(NodeItem *currentNodeItem)
{
    /*
    Node* currentNode = currentNodeItem->getNode();

    //Then do some updates.
    QStringList nodeList = nodeView->getAdoptableNodeList(currentNode);

    if(nodeList.length() > 0){
        parentButton->setEnabled(true);
    }else{
        parentButton->setEnabled(false);
    }

    addAdoptableDockNodes(currentNode, nodeList);
    */
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
 * @brief DockScrollArea::addDockNode
 * @param item
 */
void DockScrollArea::addDockNode(NodeItem* item)
{
    DockNodeItem *itm = new DockNodeItem(item, this);
    itm->setContainer(this);

    dockNodes.append(itm);
    layout->addWidget(itm);

    connect(itm, SIGNAL(getSelectedNode()), this, SLOT(dock_getSelectedNode()));
    connect(this, SIGNAL(selectedNode(Node*)), itm, SLOT(setSelectedNode(Node*)));

    connect(itm, SIGNAL(removeFromDockNodeList(QWidget*)), this,
            SLOT(removeFromDockNodeList(QWidget*)));

    connect(itm, SIGNAL(dockNode_addComponentInstance(Node*,Node*)),
            this, SLOT(dock_addComponentInstance(Node*,Node*)));
    connect(itm, SIGNAL(dockNode_connectComponentInstance(Node*,Node*)),
            this, SLOT(dock_connectComponentInstance(Node*,Node*)));

    // keep a list of all the component definitions
    if (label == "Definitions") {
        componentDefinitions.append(item->getNode());
    }
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

    adoptableNodesList = nodes;

    for (int i=0; i < nodes.count(); i++) {
        DockAdoptableNodeItem *itm = new DockAdoptableNodeItem(nodes.at(i), this);
        layout->addWidget(itm);
        dockNodes.append(itm);
        connect(itm, SIGNAL(itemPressed(QString)), this, SLOT(buttonPressed(QString)));
    }

    //checkScrollBar();
    //checkDockNodesList();
    repaint();
}


/**
 * @brief DockScrollArea::checkDockNodesList
 */
void DockScrollArea::checkDockNodesList()
{
    if (parentButton && dockNodes.count() == 0) {
        parentButton->hideContainer();
        parentButton->setEnabled(false);
    }
}


/**
 * @brief DockScrollArea::getAdoptableNodesList
 * @return
 */
QStringList DockScrollArea::getAdoptableNodesList()
{
    return adoptableNodesList;
}


/**
 * @brief DockScrollArea::getComponentDefinitions
 * @return
 */
QList<Node *> DockScrollArea::getComponentDefinitions()
{
    return componentDefinitions;
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
    emit trigger_addChildNode(kind, 0);
}


/**
 * @brief DockScrollArea::dock_addComponentInstance
 * Tell the view to try and create a ComponentInstance of
 * NodeItem itm and add it into the currently selected node.
 */
void DockScrollArea::dock_addComponentInstance(Node* assm, Node* defn)
{
    emit trigger_addComponentInstance(assm, defn, 0);
}


/**
 * @brief DockScrollArea::dock_connectComponentInstance
 * @param itm
 */
void DockScrollArea::dock_connectComponentInstance(Node *inst, Node *defn)
{
    emit trigger_connectComponentInstance(inst, defn);
}


/**
 * @brief DockScrollArea::dock_connectHardwareNode
 * @param src
 * @param hardwareNode
 */
void DockScrollArea::dock_connectHardwareNode(Node *src, Node *hardwareNode)
{
   emit trigger_connectHardwareNode(src, hardwareNode);
}


/**
 * @brief DockScrollArea::removeFromDockNodeList
 * This is called whenever a DockNodeItem is deleted.
 * @param widget
 */
void DockScrollArea::removeFromDockNodeList(QWidget *widget)
{
    if (dockNodes.contains(widget)) {
        dockNodes.removeAt(dockNodes.indexOf(widget));

        // if node is of kind Component, update componentDefinitions list
        DockNodeItem* item = qobject_cast<DockNodeItem*>(widget);
        componentDefinitions.removeAll(item->getNodeItem()->getNode());
    }
}

void DockScrollArea::updatePartsDock()
{
    /*
    if(!nodeView){
        return;
    }

    Node* selectedNode = nodeView->getSelectedNode();
    if(!selectedNode){
       //qCritical() << "DockScrollArea::updateCurrentNode() selected node is NULL";
       return;
    }
    //Then do some updates.
    QStringList nodeList = nodeView->getAdoptableNodeList(selectedNode);

    if(nodeList.length() > 0){
        parentButton->setEnabled(true);
    }else{
        parentButton->setEnabled(false);
    }

    addAdoptableDockNodes(selectedNode, nodeList);
    */
}


/**
 * @brief DockScrollArea::dock_getSelectedNode
 */
void DockScrollArea::dock_getSelectedNode()
{
    emit getSelectedNode();
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
