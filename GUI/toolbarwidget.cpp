#include "toolbarwidget.h"

#include <QDebug>
#include <QHBoxLayout>


ToolbarWidget::ToolbarWidget(NodeView *parent) :
    QWidget(parent)
{
    nodeItem = 0;
    prevNodeItem = 0;
    eventFromToolbar = false;

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
 * This method shows/hides the definitionButton and sets the definitionNode if there is one.
 * @param show
 */
void ToolbarWidget::showDefinitionButton(bool show, Node *definition)
{
    definitionButton->setVisible(show);
    if (show) {
        definitionNode = definition;
    }
}


/**
 * @brief ToolbarWidget::showImplementationButton
 * This method shows/hides the implementationButton and sets the implementationNode if there is one.
 * @param show
 */
void ToolbarWidget::showImplementationButton(bool show, Node* implementation)
{
    implementationButton->setVisible(show);
    if (show) {
        implementationNode = implementation;
    }
}


/**
 * @brief ToolbarWidget::enterEvent
 * @param event
 */
void ToolbarWidget::enterEvent(QEvent *event)
{
    eventFromToolbar = true;
}


/**
 * @brief ToolbarWidget::leaveEvent
 * @param event
 */
void ToolbarWidget::leaveEvent(QEvent *event)
{
    eventFromToolbar = false;
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
void ToolbarWidget::getAdoptableNodesList()
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
 * @brief ToolbarWidget::getComponentDefinitionsList
 */
void ToolbarWidget::getComponentDefinitionsList()
{
    emit updateMenuList("addInstance", nodeItem->getNode());
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
            if (nodeKinds->at(i) != "ComponentInstance") {
                connect(action, SIGNAL(triggered()), this, SLOT(addChildNode()));
            }
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

    } else if (action == "addInstance" && nodeList != 0) {

        /*
        addInstanceMenu->clear();

        if (nodeList->count() == 0) {
            addInstanceButton->hide();
            return;
        } else {
            addInstanceButton->show();
        }

        for (int i=0; i<nodeList->count(); i++) {
            ToolbarWidgetAction* action = new ToolbarWidgetAction(nodeList->at(i), this, true);
            addInstanceMenu->addAction(action);
        }

        addInstanceButton->setMenu(addInstanceMenu);
        */

        addInstanceMenu->clear();

        if (nodeList->count() == 0) {
            return;
        }

        for (int i=0; i<nodeList->count(); i++) {
            ToolbarWidgetAction* action = new ToolbarWidgetAction(nodeList->at(i), this, true);
            addInstanceMenu->addAction(action);
        }

        addInstanceAction = qobject_cast<ToolbarWidgetAction*>(addMenu->actions().at(0));
        addInstanceAction->setMenu(addInstanceMenu);
        connect(addInstanceAction, SIGNAL(triggered()), this, SLOT(show()));
        connect(addInstanceAction, SIGNAL(triggered()), addMenu, SLOT(show()));
        connect(addInstanceAction, SIGNAL(triggered()), this, SLOT(showMenu()));
    }
}


/**
 * @brief ToolbarWidget::constructNode
 * Send a signal to the view to construct a new node with the specified kind.
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
 * Send a signal to the view to construct an edge between the selected node and chosen node.
 */
void ToolbarWidget::connectNodes()
{
    ToolbarWidgetAction* action = qobject_cast<ToolbarWidgetAction*>(QObject::sender());
    if (action) {
        emit constructEdge(nodeItem->getNode(), action->getNode());
    }
}


/**
 * @brief ToolbarWidget::makeNewView
 */
void ToolbarWidget::makeNewView()
{
    QToolButton* button = qobject_cast<QToolButton*>(QObject::sender());
    if (button) {
        emit constructNewView(nodeItem->getNode());
    }

    QAction* action = qobject_cast<QAction*>(QObject::sender());
    if (action) {
        if (action->parentWidget() == definitionMenu) {
            emit constructNewView(definitionNode);
        } else if (action->parentWidget() == implementationMenu) {
            emit constructNewView(implementationNode);
        }
    }
}


/**
 * @brief ToolbarWidget::hideToolbar
 * This method checks if hiding the menu was triggered by the toolbar.
 * If the event came from outside the toolbar, hide the toolbar.
 */
void ToolbarWidget::hideToolbar()
{
    if (!eventFromToolbar) {
        hide();
    }
}


/**
 * @brief ToolbarWidget::showMenu
 * This shows the hidden menu that lists all the existing component definitions.
 */
void ToolbarWidget::showMenu()
{
    QPoint menuPos = addInstanceAction->getButtonPos();
    addInstanceMenu->exec(menuPos);
}


/**
 * @brief ToolbarWidget::setupToolBar
 */
void ToolbarWidget::setupToolBar()
{
    QHBoxLayout* layout = new QHBoxLayout();
    QSize buttonSize = QSize(30,30);

    addChildButton = new QToolButton(this);
    connectButton = new QToolButton(this);
    deleteButton = new QToolButton(this);
    showNewViewButton = new QToolButton(this);
    definitionButton = new QToolButton(this);
    implementationButton = new QToolButton(this);
    //addInstanceButton = new QToolButton();

    addChildButton->setIcon(QIcon(":/Resources/Icons/addChildNode.png"));
    connectButton->setIcon(QIcon(":/Resources/Icons/connectNode.png"));
    deleteButton->setIcon(QIcon(":/Resources/Icons/deleteNode.png"));
    showNewViewButton->setIcon(QIcon(":/Resources/Icons/popup.png"));
    definitionButton->setIcon(QIcon(":/Resources/Icons/definition.png"));
    implementationButton->setIcon(QIcon(":/Resources/Icons/implementation.png"));
    //addInstanceButton->setIcon(QIcon(":/Resources/Icons/ComponentInstance.png"));

    addChildButton->setFixedSize(buttonSize);
    connectButton->setFixedSize(buttonSize);
    deleteButton->setFixedSize(buttonSize);
    showNewViewButton->setFixedSize(buttonSize);
    definitionButton->setFixedSize(buttonSize);
    implementationButton->setFixedSize(buttonSize);
    //addInstanceButton->setFixedSize(buttonSize);

    addChildButton->setIconSize(buttonSize*0.6);
    connectButton->setIconSize(buttonSize*0.7);
    deleteButton->setIconSize(buttonSize*0.85);
    definitionButton->setIconSize(buttonSize*1.2);
    implementationButton->setIconSize(buttonSize*0.7);
    //addInstanceButton->setIconSize(buttonSize*0.8);

    addChildButton->setToolTip("Add Child Node");
    connectButton->setToolTip("Connect Node");
    deleteButton->setToolTip("Delete Node");
    showNewViewButton->setToolTip("Show in New Window");
    definitionButton->setToolTip("Go to Definition");
    implementationButton->setToolTip("Go to Implementation");
    //addInstanceButton->setToolTip("Add Component Instance");

    frame = new QFrame();
    frame->setFrameShape(QFrame::VLine);
    frame->setPalette(QPalette(Qt::darkGray));

    layout->addWidget(addChildButton);
    //layout->addWidget(addInstanceButton);
    layout->addWidget(connectButton);
    layout->addWidget(deleteButton);
    layout->addWidget(frame);
    layout->addWidget(showNewViewButton);
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
    addMenu = new QMenu(addChildButton);
    connectMenu = new QMenu(connectButton);
    definitionMenu = new QMenu(definitionButton);
    implementationMenu = new QMenu(implementationButton);
    //addInstanceMenu = new QMenu(addInstanceButton);
    addInstanceMenu = new QMenu(this);

    addChildButton->setPopupMode(QToolButton::InstantPopup);
    addChildButton->setMenu(addMenu);

    connectButton->setPopupMode(QToolButton::InstantPopup);
    connectButton->setMenu(connectMenu);

    //addInstanceButton->setPopupMode(QToolButton::InstantPopup);
    //addInstanceButton->setMenu(addInstanceMenu);

    QAction* defn_goTo = definitionMenu->addAction(QIcon(":/Resources/Icons/goto.png"), "Go to Definition");
    QAction* defn_popup = definitionMenu->addAction(QIcon(":/Resources/Icons/popup.png"), "Popup Definition");
    connect(defn_goTo, SIGNAL(triggered()), this, SLOT(goToDefinition()));
    connect(defn_popup, SIGNAL(triggered()), this, SLOT(makeNewView()));

    definitionButton->setPopupMode(QToolButton::InstantPopup);
    definitionButton->setMenu(definitionMenu);

    QAction* impl_goTo = implementationMenu->addAction(QIcon(":/Resources/Icons/goto.png"), "Go to Implementation");
    QAction* impl_popup = implementationMenu->addAction(QIcon(":/Resources/Icons/popup.png"), "Popup Implementation");
    connect(impl_goTo, SIGNAL(triggered()), this, SLOT(goToImplementation()));
    connect(impl_popup, SIGNAL(triggered()), this, SLOT(makeNewView()));

    implementationButton->setPopupMode(QToolButton::InstantPopup);
    implementationButton->setMenu(implementationMenu);
}


/**
 * @brief ToolbarWidget::makeConnections
 */
void ToolbarWidget::makeConnections()
{
    connect(addMenu, SIGNAL(triggered(QAction*)), this, SLOT(hide()));
    connect(addMenu, SIGNAL(triggered(QAction*)), addMenu, SLOT(hide()));
    connect(addMenu, SIGNAL(aboutToHide()), this, SLOT(hideToolbar()));

    connect(connectMenu, SIGNAL(triggered(QAction*)), this, SLOT(hide()));
    connect(connectMenu, SIGNAL(triggered(QAction*)), connectMenu, SLOT(hide()));
    connect(connectMenu, SIGNAL(aboutToHide()), this, SLOT(hideToolbar()));

    //connect(addInstanceMenu, SIGNAL(triggered(QAction*)), this, SLOT(hide()));
    //connect(addInstanceMenu, SIGNAL(triggered(QAction*)), addInstanceMenu, SLOT(hide()));
    //connect(addInstanceMenu, SIGNAL(aboutToHide()), this, SLOT(hideToolbar()));

    connect(definitionMenu, SIGNAL(triggered(QAction*)), this, SLOT(hide()));
    connect(definitionMenu, SIGNAL(triggered(QAction*)), definitionMenu, SLOT(hide()));
    connect(definitionMenu, SIGNAL(aboutToHide()), this, SLOT(hideToolbar()));

    connect(implementationMenu, SIGNAL(triggered(QAction*)), this, SLOT(hide()));
    connect(implementationMenu, SIGNAL(triggered(QAction*)), definitionMenu, SLOT(hide()));
    connect(implementationMenu, SIGNAL(aboutToHide()), this, SLOT(hideToolbar()));

    connect(connectButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(deleteButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(definitionButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(implementationButton, SIGNAL(clicked()), this, SLOT(hide()));

    connect(showNewViewButton, SIGNAL(clicked()), this, SLOT(makeNewView()));
    connect(implementationButton, SIGNAL(clicked()), this, SLOT(goToImplementation()));

    connectToView();
}


/**
 * @brief ToolbarWidget::connectNodeItem
 */
void ToolbarWidget::connectToView()
{
    NodeView* nodeView = dynamic_cast<NodeView*>(parentWidget());

    connect(deleteButton, SIGNAL(clicked()), nodeView, SLOT(toolbar_deleteSelectedNode()));

    connect(this, SIGNAL(checkDefinition(Node*, bool)), nodeView, SLOT(goToDefinition(Node*, bool)));
    connect(this, SIGNAL(checkImplementation(Node*, bool)), nodeView, SLOT(goToImplementation(Node*, bool)));

    connect(this, SIGNAL(goToDefinition(Node*)), nodeView, SLOT(goToDefinition(Node*)));
    connect(this, SIGNAL(goToImplementation(Node*)), nodeView, SLOT(goToImplementation(Node*)));

    connect(this, SIGNAL(updateMenuList(QString,Node*)), nodeView, SLOT(updateToolbarMenuList(QString,Node*)));
    connect(nodeView, SIGNAL(updateMenuList(QString,QStringList*,QList<Node*>*)), this, SLOT(updateMenuList(QString,QStringList*,QList<Node*>*)));

    connect(this, SIGNAL(constructNode(QString)), nodeView, SLOT(view_ConstructNodeAction(QString)));
    connect(this, SIGNAL(constructEdge(Node*,Node*)), nodeView, SLOT(view_ConstructEdgeAction(Node*,Node*)));

    connect(this, SIGNAL(constructNewView(Node*)), nodeView, SLOT(constructNewView(Node*)));
}


/**
 * @brief ToolbarWidget::updateToolButtons
 * Hide the tool buttons that don't apply to the selected node item.
 */
void ToolbarWidget::updateToolButtons()
{
    QString nodeKind = nodeItem->getNodeKind();
    getAdoptableNodesList();

    if (nodeKind.endsWith("Definitions") || nodeKind.startsWith("Management")) {

        deleteButton->hide();
        connectButton->hide();
        //addInstanceButton->hide();
        definitionButton->hide();
        implementationButton->hide();

    } else {

        deleteButton->show();
        getLegalNodesList();

        if (nodeKind == "ComponentAssembly") {
            getComponentDefinitionsList();
        }

        emit checkDefinition(nodeItem->getNode(), false);
        emit checkImplementation(nodeItem->getNode(), false);
    }
}
