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

        if (nodeKinds->count() == 0) { // || nodeItem->getNodeKind() == "ComponentInstance") {
            addChildButton->hide();
            return;
        } else {
            addChildButton->show();
        }

        for (int i=0; i<nodeKinds->count(); i++) {

            QString nodeKind = nodeKinds->at(i);
            ToolbarWidgetAction* action;

            if (nodeKind == "ComponentInstance") {
                action = addInstanceAction;
            } else {
                action  = new ToolbarWidgetAction(nodeKind, this);
                connect(action, SIGNAL(triggered()), this, SLOT(addChildNode()));
            }

            addMenu->addAction(action);
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

        addInstanceActionMenu->clear();

        if (nodeList->count() == 0) {
            return;
        }

        for (int i=0; i<nodeList->count(); i++) {
            ToolbarWidgetAction* action = new ToolbarWidgetAction(nodeList->at(i), this, true);
            addInstanceActionMenu->addAction(action);
            connect(action, SIGNAL(triggered()), this, SLOT(addComponentInstance()));
        }

        addInstanceAction = qobject_cast<ToolbarWidgetAction*>(addMenu->actions().at(0));
        addInstanceAction->setMenu(addInstanceActionMenu);
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
 * @brief ToolbarWidget::addComponentInstance
 */
void ToolbarWidget::addComponentInstance()
{
    ToolbarWidgetAction* action = qobject_cast<ToolbarWidgetAction*>(QObject::sender());
    if (action) {
        emit constructComponentInstance(nodeItem->getNode(), action->getNode(), 1);
    }
}


/**
 * @brief ToolbarWidget::hideToolbar
 * When a QAction is triggered and it's not addInstanceAction,
 * hide both the action's menu (if it has one) and the toolbar.
 * @param action
 */
void ToolbarWidget::hideToolbar(QAction *action)
{
    if (action == addInstanceAction) {
        return;
    } else {
        QMenu* menu = qobject_cast<QMenu*>(QObject::sender());
        menu->hide();
        hide();

        if (menu == addInstanceActionMenu) {
            addMenu->hide();
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
    QMenu* menu = qobject_cast<QMenu*>(QObject::sender());
    if (!eventFromToolbar) {
        qDebug() << "Hide Toolbar";
        menu->hide();
        hide();
    }
}


/**
 * @brief ToolbarWidget::hideMenu
 */
void ToolbarWidget::hideMenu()
{
    qDebug() << "Hide Menu";
    QMenu* menu = qobject_cast<QMenu*>(QObject::sender());
    menu->hide();
}


/**
 * @brief ToolbarWidget::showMenu
 * This shows the hidden menu that lists all the existing component definitions.
 */
void ToolbarWidget::showMenu()
{
    QPoint menuPos = addInstanceAction->getButtonPos();
    addInstanceActionMenu->exec(menuPos);
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

    addChildButton->setIcon(QIcon(":/Resources/Icons/addChildNode.png"));
    connectButton->setIcon(QIcon(":/Resources/Icons/connectNode.png"));
    deleteButton->setIcon(QIcon(":/Resources/Icons/deleteNode.png"));
    showNewViewButton->setIcon(QIcon(":/Resources/Icons/popup.png"));
    definitionButton->setIcon(QIcon(":/Resources/Icons/definition.png"));
    implementationButton->setIcon(QIcon(":/Resources/Icons/implementation.png"));

    addChildButton->setFixedSize(buttonSize);
    connectButton->setFixedSize(buttonSize);
    deleteButton->setFixedSize(buttonSize);
    showNewViewButton->setFixedSize(buttonSize);
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
    showNewViewButton->setToolTip("Show in New Window");
    definitionButton->setToolTip("Go to Definition");
    implementationButton->setToolTip("Go to Implementation");

    frame = new QFrame();
    frame->setFrameShape(QFrame::VLine);
    frame->setPalette(QPalette(Qt::darkGray));

    layout->addWidget(addChildButton);
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

    addChildButton->setPopupMode(QToolButton::InstantPopup);
    addChildButton->setMenu(addMenu);

    connectButton->setPopupMode(QToolButton::InstantPopup);
    connectButton->setMenu(connectMenu);

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

    // this is used when ComponentInstance can be adopted by the current node
    addInstanceAction = new ToolbarWidgetAction("ComponentInstance", this);
    addInstanceActionMenu = new QMenu(addChildButton);
}


/**
 * @brief ToolbarWidget::makeConnections
 */
void ToolbarWidget::makeConnections()
{
    connect(addInstanceAction, SIGNAL(triggered()), this, SLOT(showMenu()));

    connect(addInstanceActionMenu, SIGNAL(triggered(QAction*)), this, SLOT(hideToolbar(QAction*)));
    //connect(addInstanceActionMenu, SIGNAL(aboutToHide()), this, SLOT(hideToolbar()));

    connect(addMenu, SIGNAL(triggered(QAction*)), this, SLOT(hideToolbar(QAction*)));
    connect(addMenu, SIGNAL(aboutToHide()), this, SLOT(hideToolbar()));
    connect(addMenu, SIGNAL(aboutToHide()), this, SLOT(hideMenu()));

    connect(connectMenu, SIGNAL(triggered(QAction*)), this, SLOT(hideToolbar(QAction*)));
    connect(connectMenu, SIGNAL(aboutToHide()), this, SLOT(hideToolbar()));
    connect(connectMenu, SIGNAL(aboutToHide()), this, SLOT(hideMenu()));

    connect(definitionMenu, SIGNAL(triggered(QAction*)), this, SLOT(hideToolbar(QAction*)));
    connect(definitionMenu, SIGNAL(aboutToHide()), this, SLOT(hideToolbar()));

    connect(implementationMenu, SIGNAL(triggered(QAction*)), this, SLOT(hideToolbar(QAction*)));
    connect(implementationMenu, SIGNAL(aboutToHide()), this, SLOT(hideToolbar()));

    connect(deleteButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(definitionButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(implementationButton, SIGNAL(clicked()), this, SLOT(hide()));

    connect(showNewViewButton, SIGNAL(clicked()), this, SLOT(makeNewView()));

    connectToView();
}


/**
 * @brief ToolbarWidget::connectNodeItem
 */
void ToolbarWidget::connectToView()
{
    NodeView* nodeView = dynamic_cast<NodeView*>(parentWidget());

    connect(deleteButton, SIGNAL(clicked()), nodeView, SLOT(view_deleteSelectedNode()));

    connect(this, SIGNAL(checkDefinition(Node*, bool)), nodeView, SLOT(goToDefinition(Node*, bool)));
    connect(this, SIGNAL(checkImplementation(Node*, bool)), nodeView, SLOT(goToImplementation(Node*, bool)));

    connect(this, SIGNAL(goToDefinition(Node*)), nodeView, SLOT(goToDefinition(Node*)));
    connect(this, SIGNAL(goToImplementation(Node*)), nodeView, SLOT(goToImplementation(Node*)));

    connect(this, SIGNAL(updateMenuList(QString,Node*)), nodeView, SLOT(updateToolbarMenuList(QString,Node*)));
    connect(nodeView, SIGNAL(updateMenuList(QString,QStringList*,QList<Node*>*)), this, SLOT(updateMenuList(QString,QStringList*,QList<Node*>*)));

    connect(this, SIGNAL(constructNode(QString)), nodeView, SLOT(view_ConstructNode(QString)));
    connect(this, SIGNAL(constructEdge(Node*,Node*)), nodeView, SLOT(view_ConstructEdge(Node*,Node*)));
    connect(this, SIGNAL(constructComponentInstance(Node*,Node*,int)), nodeView, SLOT(view_ConstructComponentInstance(Node*,Node*,int)));

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
        definitionButton->hide();
        implementationButton->hide();

    } else {

        deleteButton->show();
        checkDefinition();
        checkImplementation();
        getLegalNodesList();

        if (nodeKind == "ComponentAssembly") {
            getComponentDefinitionsList();
        }
    }
}


/**
 * @brief ToolbarWidget::checkDefinition
 */
void ToolbarWidget::checkDefinition()
{
    if (nodeItem && nodeItem->getNode()) {
        if (nodeItem->getNode()->isDefinition()) {
            definitionButton->hide();
        } else {
            emit checkDefinition(nodeItem->getNode(), false);
        }
    }
}


/**
 * @brief ToolbarWidget::checkImplementation
 */
void ToolbarWidget::checkImplementation()
{
    if (nodeItem && nodeItem->getNode()) {
        if (nodeItem->getNode()->isImpl()) {
            implementationButton->hide();
        } else {
            emit checkImplementation(nodeItem->getNode(), false);
        }
    }
}
