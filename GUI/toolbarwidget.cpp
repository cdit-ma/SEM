#include "toolbarwidget.h"
#include "toolbarwidgetaction.h"
#include "nodeview.h"

#include <QDebug>
#include <QHBoxLayout>


ToolbarWidget::ToolbarWidget(QWidget *parent) :
    QWidget(parent)
{
    nodeItem = 0;
    prevNodeItem = 0;

    setMinimumSize(100,40);
    setBackgroundRole(QPalette::Dark);
    setWindowFlags(windowFlags() | Qt::Popup);

    setupToolBar();
    setupButtonMenus();
    makeConnections();
}


/**
 * @brief ToolbarWidget::setNodeItem
 * Set nodeItem to the currectly selected item.
 * Update applicable tool buttons only if prev != item.
 * @param item
 */
void ToolbarWidget::setNodeItem(NodeItem *item)
{
    nodeItem = item;
    if (prevNodeItem != nodeItem) {
        updateToolButtons();
    }
    prevNodeItem = nodeItem;
}


/**
 * @brief ToolbarWidget::showDefinitionButton
 * @param show
 */
void ToolbarWidget::showDefinitionButton(bool show)
{
    definitionButton->setVisible(show);
    if (!show) {
        showFrame++;
    }
}


/**
 * @brief ToolbarWidget::showImplementationButton
 * @param show
 */
void ToolbarWidget::showImplementationButton(bool show)
{
    implementationButton->setVisible(show);
    if (!show) {
        showFrame++;
    }
}


/**
 * @brief ToolbarWidget::goToDefinition
 */
void ToolbarWidget::goToDefinition()
{
    emit goToDefinition(nodeItem->getNode());
}


/**
 * @brief ToolbarWidget::goToImplementation
 */
void ToolbarWidget::goToImplementation()
{
    emit goToImplementation(nodeItem->getNode());
}


/**
 * @brief ToolbarWidget::getAdoptableNodeList
 */
void ToolbarWidget::getAdoptableNodeList()
{
    emit updateMenuList("add", nodeItem->getNode());
}


/**
 * @brief ToolbarWidget::getLegalNodesList
 */
void ToolbarWidget::getLegalNodesList()
{
    emit updateMenuList("connect", nodeItem->getNode());
}


/**
 * @brief ToolbarWidget::updateMenuList
 * @param action
 * @param nodeList
 */
void ToolbarWidget::updateMenuList(QString action, QStringList* nodeKinds, QList<Node*>* nodeList)
{
    if (action == "add" && nodeKinds != 0) {

        addMenu->clear();

        if (nodeKinds->count() == 0) {
            addChildButton->hide();
            return;
        } else {
            addChildButton->show();
        }

        for (int i=0; i<nodeKinds->count(); i++) {
            ToolbarWidgetAction* action = new ToolbarWidgetAction(nodeKinds->at(i), this);
            addMenu->addAction(action);
            connect(action, SIGNAL(triggered()), this, SLOT(addChildNode()));
        }

        addChildButton->setMenu(addMenu);

    } else if (action == "connect" && nodeList != 0) {

        connectMenu->clear();

        if (nodeList->count() == 0) {
            connectButton->hide();
            return;
        } else {
            connectButton->show();
        }

        for (int i=0; i<nodeList->count(); i++) {
            ToolbarWidgetAction* action = new ToolbarWidgetAction(nodeList->at(i), this);
            connectMenu->addAction(action);
            connect(action, SIGNAL(triggered()), this, SLOT(connectNodes()));
        }

        connectButton->setMenu(connectMenu);
    }
}


/**
 * @brief ToolbarWidget::constructNode
 */
void ToolbarWidget::addChildNode()
{
    ToolbarWidgetAction* action = qobject_cast<ToolbarWidgetAction*>(QObject::sender());
    if (action) {
        emit constructNode(action->getKind());
    }
}


/**
 * @brief ToolbarWidget::connectNodes
 */
void ToolbarWidget::connectNodes()
{
    ToolbarWidgetAction* action = qobject_cast<ToolbarWidgetAction*>(QObject::sender());
    if (action) {
        emit constructEdge(nodeItem->getNode(), action->getNode());
    }
}


/**
 * @brief ToolbarWidget::hideToolbar
 */
void ToolbarWidget::hideToolbar()
{
    /*
    if (mousePressOutOfBounds) {
        hide();
        resetToolbarStates();
    }
    */
}


/**
 * @brief ToolbarWidget::resetToolbarStates
 */
void ToolbarWidget::resetToolbarStates()
{
    //addChildButton->setChecked(false);
}


/**
 * @brief ToolbarWidget::setupToolBar
 */
void ToolbarWidget::setupToolBar()
{
    QHBoxLayout* layout = new QHBoxLayout();
    QSize buttonSize = QSize(30,30);

    addChildButton = new QToolButton();
    deleteButton = new QToolButton();
    connectButton = new QToolButton();
    definitionButton = new QToolButton();
    implementationButton = new QToolButton();

    addChildButton->setIcon(QIcon(":/Resources/Icons/addChildNode.png"));
    connectButton->setIcon(QIcon(":/Resources/Icons/connectNode.png"));
    deleteButton->setIcon(QIcon(":/Resources/Icons/deleteNode.png"));
    definitionButton->setIcon(QIcon(":/Resources/Icons/definition.png"));
    implementationButton->setIcon(QIcon(":/Resources/Icons/implementation.png"));

    addChildButton->setFixedSize(buttonSize);
    connectButton->setFixedSize(buttonSize);
    deleteButton->setFixedSize(buttonSize);
    definitionButton->setFixedSize(buttonSize);
    implementationButton->setFixedSize(buttonSize);

    addChildButton->setIconSize(buttonSize*0.6);
    connectButton->setIconSize(buttonSize*0.7);
    deleteButton->setIconSize(buttonSize*0.85);
    definitionButton->setIconSize(buttonSize*1.2);
    implementationButton->setIconSize(buttonSize*0.7);

    addChildButton->setToolTip("Add Child Node");
    connectButton->setToolTip("Connect Node");
    deleteButton->setToolTip("Delete Node");
    definitionButton->setToolTip("Go to Definition");
    implementationButton->setToolTip("Go to Implementation");

    frame = new QFrame();
    frame->setFrameShape(QFrame::VLine);
    frame->setPalette(QPalette(Qt::darkGray));

    layout->addWidget(addChildButton);
    layout->addWidget(connectButton);
    layout->addWidget(deleteButton);
    layout->addWidget(frame);
    layout->addWidget(definitionButton);
    layout->addWidget(implementationButton);

    layout->setMargin(5);
    setLayout(layout);
}


/**
 * @brief ToolbarWidget::setupButtonMenus
 */
void ToolbarWidget::setupButtonMenus()
{
    addMenu = new QMenu(this);
    connectMenu = new QMenu(this);

    addChildButton->setPopupMode(QToolButton::InstantPopup);
    addChildButton->setMenu(addMenu);

    connectButton->setPopupMode(QToolButton::InstantPopup);
    connectButton->setMenu(connectMenu);
}


/**
 * @brief ToolbarWidget::makeConnections
 */
void ToolbarWidget::makeConnections()
{
    connect(addMenu, SIGNAL(triggered(QAction*)), this, SLOT(hide()));
    connect(addMenu, SIGNAL(triggered(QAction*)), addMenu, SLOT(hide()));
    connect(connectMenu, SIGNAL(triggered(QAction*)), this, SLOT(hide()));
    connect(connectMenu, SIGNAL(triggered(QAction*)), connectMenu, SLOT(hide()));

    //connect(addMenu, SIGNAL(aboutToHide()), this, SLOT(hide()));
    //connect(addMenu, SIGNAL(aboutToHide()), this, SLOT(hideToolbar()));

    connect(connectButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(deleteButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(definitionButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(implementationButton, SIGNAL(clicked()), this, SLOT(hide()));

    connect(definitionButton, SIGNAL(clicked()), this, SLOT(goToDefinition()));
    connect(implementationButton, SIGNAL(clicked()), this, SLOT(goToImplementation()));

    connectToView();
}


/**
 * @brief ToolbarWidget::connectNodeItem
 * @param item
 */
void ToolbarWidget::connectToView()
{
    NodeView* nodeView = dynamic_cast<NodeView*>(parentWidget());

    connect(deleteButton, SIGNAL(clicked()), nodeView, SLOT(trigger_deletePressed()));

    connect(this, SIGNAL(checkDefinition(Node*, bool)), nodeView, SLOT(goToDefinition(Node*, bool)));
    connect(this, SIGNAL(checkImplementation(Node*, bool)), nodeView, SLOT(goToImplementation(Node*, bool)));

    connect(this, SIGNAL(goToDefinition(Node*)), nodeView, SLOT(goToDefinition(Node*)));
    connect(this, SIGNAL(goToImplementation(Node*)), nodeView, SLOT(goToImplementation(Node*)));

    connect(this, SIGNAL(updateMenuList(QString,Node*)), nodeView, SLOT(updateToolbarMenuList(QString,Node*)));
    connect(nodeView, SIGNAL(updateMenuList(QString,QStringList*,QList<Node*>*)), this, SLOT(updateMenuList(QString,QStringList*,QList<Node*>*)));

    connect(this, SIGNAL(constructNode(QString)), nodeView, SLOT(view_ConstructNodeAction(QString)));
    connect(this, SIGNAL(constructEdge(Node*,Node*)), nodeView, SLOT(view_ConstructEdgeAction(Node*,Node*)));

    connect(nodeView, SIGNAL(hideToolbarWidget()), this, SLOT(hide()));
}


/**
 * @brief ToolbarWidget::updateToolButtons
 * Hide the tool buttons that don't apply to the selected node item.
 */
void ToolbarWidget::updateToolButtons()
{
    frame->setVisible(false);

    if (nodeItem->getNodeKind().endsWith("Definitions") || nodeItem->getNodeKind().startsWith("Management")) {

        deleteButton->hide();
        definitionButton->hide();
        implementationButton->hide();

    } else {

        showFrame = 0;

        deleteButton->show();
        emit checkDefinition(nodeItem->getNode(), false);
        emit checkImplementation(nodeItem->getNode(), false);

        if (showFrame < 2) {
            frame->setVisible(true);
        }
    }

    // isVisible() is giving the wrong value
    //frame->setVisible(definitionButton->isVisible() || implementationButton->isVisible());
    getAdoptableNodeList();
    getLegalNodesList();
}
