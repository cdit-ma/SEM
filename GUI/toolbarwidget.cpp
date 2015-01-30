#include "toolbarwidget.h"
#include "toolbarwidgetaction.h"
#include "nodeview.h"

#include <QDebug>
#include <QHBoxLayout>


ToolbarWidget::ToolbarWidget(QWidget *parent) :
    QWidget(parent)
{
    nodeItem = 0;

    setMinimumSize(100,40);
    setBackgroundRole(QPalette::Dark);
    setWindowFlags(windowFlags() | Qt::Popup);

    setupToolBar();
    setupButtonMenus();
    makeConnections();
}


/**
 * @brief ToolbarWidget::setNodeItem
 * @param item
 */
void ToolbarWidget::setNodeItem(NodeItem *item)
{
    nodeItem = item;
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
    emit updateMenuList(nodeItem->getNode());
}


/**
 * @brief ToolbarWidget::updateMenuList
 * @param action
 * @param nodeList
 */
void ToolbarWidget::updateMenuList(QString action, QStringList nodeList)
{
    QToolButton* button;
    QMenu* menu;

    if (action == "add") {
        button = addChildButton;
        menu = addMenu;
    } else if (action == "connect") {
        button = connectButton;
        menu = connectMenu;
    }

    menu->clear();

    for (int i=0; i<nodeList.count(); i++) {
        ToolbarWidgetAction* action = new ToolbarWidgetAction(nodeList.at(i), this);
        menu->addAction(action);
        connect(action, SIGNAL(triggered()), this, SLOT(addChildNode()));
    }

    button->setMenu(menu);
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

    addChildButton->setIconSize(buttonSize*0.65);
    connectButton->setIconSize(buttonSize*0.7);
    deleteButton->setIconSize(buttonSize*0.85);
    definitionButton->setIconSize(buttonSize*1.2);
    implementationButton->setIconSize(buttonSize*0.7);

    addChildButton->setToolTip("Add Child Node");
    connectButton->setToolTip("Connect Node");
    deleteButton->setToolTip("Delete Node");
    definitionButton->setToolTip("Go to Definition");
    implementationButton->setToolTip("Go to Implementation");

    QFrame* frame = new QFrame();
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

    //connectButton->setPopupMode(QToolButton::InstantPopup);
    //connectButton->setMenu(connectMenu);
}


/**
 * @brief ToolbarWidget::makeConnections
 */
void ToolbarWidget::makeConnections()
{
    connect(addMenu, SIGNAL(aboutToShow()), this, SLOT(getAdoptableNodeList()));
    connect(addMenu, SIGNAL(triggered(QAction*)), this, SLOT(hide()));
    connect(addMenu, SIGNAL(triggered(QAction*)), addMenu, SLOT(hide()));

    //connect(addMenu, SIGNAL(aboutToHide()), this, SLOT(hide()));
    connect(addMenu, SIGNAL(aboutToHide()), this, SLOT(hideToolbar()));

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

    connect(connectButton, SIGNAL(clicked()), nodeView, SLOT(trigger_shiftPressed()));
    connect(deleteButton, SIGNAL(clicked()), nodeView, SLOT(trigger_deletePressed()));

    connect(this, SIGNAL(goToDefinition(Node*)), nodeView, SLOT(goToDefinition(Node*)));
    connect(this, SIGNAL(goToImplementation(Node*)), nodeView, SLOT(goToImplementation(Node*)));

    connect(this, SIGNAL(updateMenuList(Node*)), nodeView, SLOT(updateMenuList(Node*)));
    connect(nodeView, SIGNAL(updateMenuList(QString,QStringList)), this, SLOT(updateMenuList(QString,QStringList)));

    connect(this, SIGNAL(constructNode(QString)), nodeView, SLOT(view_ConstructNodeAction(QString)));

    connect(nodeView, SIGNAL(hideToolbarWidget()), this, SLOT(hide()));
}
