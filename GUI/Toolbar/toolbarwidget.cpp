#include "toolbarwidget.h"
#include "toolbarwidgetmenu.h"

#include <QApplication>
#include <QClipboard>
#include <QHBoxLayout>
#include <QDebug>


/**
 * @brief ToolbarWidget::ToolbarWidget
 * @param parent
 */
ToolbarWidget::ToolbarWidget(NodeView *parent) :
    QWidget(parent)
{
    nodeView = parent;
    nodeItem = 0;
    prevNodeItem = 0;

    frameVisibilityCount = 0;
    eventFromToolbar = false;

    definitionNode = 0;
    implementationNode = 0;

    setBackgroundRole(QPalette::Dark);
    setWindowFlags(windowFlags() | Qt::Popup);

    setStyleSheet("ToolbarWidget{"
                  "background-color: rgba(240,240,240,240);"
                  "}"

                  "QToolButton{"
                  "background-color: white;"
                  "border: 1px solid grey;"
                  "border-radius: 10px;"
                  "}");

    setupToolBar();
    setupMenus();
    makeConnections();
}


/**
 * @brief ToolbarWidget::setNodeItem
 * This only gets called when the toolbar is about to show.
 * Set nodeItem to the currectly selected item and update applicable tool buttons and menus.
 * @param item
 */
void ToolbarWidget::setNodeItem(NodeItem *item)
{
    nodeItem = item;
    updateMenuLists();
    updateToolButtons();
}


/**
 * @brief ToolbarWidget::showDefinitionButton
 * This method shows/hides the definitionButton and sets the definitionNode if there is one.
 * This gets called everytime a node item is selected.
 * @param show
 */
void ToolbarWidget::showDefinitionButton(Node *definition)
{
    if (definition) {
        definitionNode = definition;
        definitionButton->show();
        frameVisibilityCount++;
    } else {
        definitionButton->hide();
    }
}


/**
 * @brief ToolbarWidget::showImplementationButton
 * This method shows/hides the implementationButton and sets the implementationNode if there is one.
 * This gets called everytime a node item is selected.
 * @param show
 */
void ToolbarWidget::showImplementationButton(Node* implementation)
{
    if (implementation) {
        implementationNode = implementation;
        implementationButton->show();
        frameVisibilityCount++;
    } else {
        implementationButton->hide();
    }
}


/**
 * @brief ToolbarWidget::enterEvent
 * This is called when the mouse is hovering over the toolbar.
 * @param event
 */
void ToolbarWidget::enterEvent(QEvent* e)
{
    eventFromToolbar = true;
    QWidget::enterEvent(e);
}


/**
 * @brief ToolbarWidget::leaveEvent
 * This is called when the mouse is no longer hovering over the toolbar.
 * @param event
 */
void ToolbarWidget::leaveEvent(QEvent* e)
{
    eventFromToolbar = false;
    QWidget::leaveEvent(e);
}


/**
 * @brief ToolbarWidget::goToDefinition
 * This sends a signal to the view to center the selected nodeitem's definition.
 */
void ToolbarWidget::goToDefinition()
{
    nodeView->goToDefinition(nodeItem->getNode());
}


/**
 * @brief ToolbarWidget::goToImplementation
 * This sends a signal to the view to center the selected nodeitem's implementation.
 */
void ToolbarWidget::goToImplementation()
{
    nodeView->goToImplementation(nodeItem->getNode());
}


/**
 * @brief ToolbarWidget::goToInstance
 */
void ToolbarWidget::goToInstance()
{
    nodeView->goToInstance(instanceOptionMenu->getParentAction()->getNode());
}


/**
 * @brief ToolbarWidget::constructNode
 * Send a signal to the view to construct a new node with the specified kind.
 */
void ToolbarWidget::addChildNode()
{
    ToolbarWidgetAction* action = qobject_cast<ToolbarWidgetAction*>(QObject::sender());
    nodeView->constructNode(action->getKind(), 1);
}


/**
 * @brief ToolbarWidget::connectNodes
 * Send a signal to the view to construct an edge between the selected node and chosen node.
 */
void ToolbarWidget::connectNodes()
{
    ToolbarWidgetAction* action = qobject_cast<ToolbarWidgetAction*>(QObject::sender());
    nodeView->constructEdge(nodeItem->getNode(), action->getNode());
}


/**
 * @brief ToolbarWidget::makeNewView
 * This pops up a new window. The node centered in the new window depends on the signal sender.
 */
void ToolbarWidget::makeNewView()
{
    // pop up the selected node into a new window
    QToolButton* button = qobject_cast<QToolButton*>(QObject::sender());
    if (button) {
        nodeView->constructNewView(nodeView->getSelectedNode());
        return;
    }

    // pop up the selected node's definition/implementation into a new window
    QAction* action = qobject_cast<QAction*>(QObject::sender());
    if (action) {
        if (action->parentWidget() == definitionMenu) {
            nodeView->constructNewView(definitionNode);
        } else if (action->parentWidget() == implementationMenu) {
            nodeView->constructNewView(implementationNode);
        } else if (action->parentWidget() == instanceOptionMenu) {
            nodeView->constructNewView(instanceOptionMenu->getParentAction()->getNode());
        }
    }
}


/**
 * @brief ToolbarWidget::addComponentInstance
 * Send a signal to the view to construct a ComponentInstance of the chosen action's node.
 */
void ToolbarWidget::addComponentInstance()
{
    ToolbarWidgetAction* action = qobject_cast<ToolbarWidgetAction*>(QObject::sender());
    nodeView->constructComponentInstance(nodeItem->getNode(), action->getNode(), 1);
}


/**
 * @brief ToolbarWidget::addEventPorDelegate
 */
void ToolbarWidget::addEventPorDelegate()
{
    ToolbarWidgetAction* action = qobject_cast<ToolbarWidgetAction*>(QObject::sender());

    Node* actionNode = action->getNode();
    QString nodeKind = actionNode->getDataValue("kind");

    if(actionNode->getDataValue("kind").startsWith("InEvent")){
        nodeKind = "InEventPortDelegate";
    }
    if(actionNode->getDataValue("kind").startsWith("OutEvent")){
        nodeKind = "OutEventPortDelegate";
    }

    nodeView->constructConnectedNode(nodeItem->getNode(), actionNode, nodeKind, 1);
}


/**
 * @brief ToolbarWidget::attachOptionMenu
 */
void ToolbarWidget::attachOptionMenu()
{
    ToolbarWidgetAction* action = qobject_cast<ToolbarWidgetAction*>(QObject::sender());
    instanceOptionMenu->setParentAction(action);
}


/**
 * @brief ToolbarWidget::hideToolbar
 * This method checks if hiding the menu was triggered by the toolbar.
 * If the event came from outside the toolbar, hide the toolbar and all visible menus.
 */
void ToolbarWidget::hideToolbar(bool actionTriggered)
{
    if (!eventFromToolbar) {
        hide();
        if (!actionTriggered) {
            nodeView->toolbarClosed();
        }
    }
}


/**
 * @brief ToolbarWidget::setupToolBar
 */
void ToolbarWidget::setupToolBar()
{
    QHBoxLayout* layout = new QHBoxLayout();
    QSize buttonSize = QSize(39,39);

    addChildButton = new QToolButton(this);
    connectButton = new QToolButton(this);
    deleteButton = new QToolButton(this);
    showNewViewButton = new QToolButton(this);
    definitionButton = new QToolButton(this);
    implementationButton = new QToolButton(this);
    instancesButton = new QToolButton(this);

    addChildButton->setIcon(QIcon(":/Resources/Icons/addChildNode.png"));
    connectButton->setIcon(QIcon(":/Resources/Icons/connectNode.png"));
    deleteButton->setIcon(QIcon(":/Resources/Icons/deleteNode.png"));
    showNewViewButton->setIcon(QIcon(":/Resources/Icons/popup.png"));
    definitionButton->setIcon(QIcon(":/Resources/Icons/definition.png"));
    implementationButton->setIcon(QIcon(":/Resources/Icons/implementation.png"));
    instancesButton->setIcon(QIcon(":/Resources/Icons/instance.png"));

    addChildButton->setFixedSize(buttonSize);
    connectButton->setFixedSize(buttonSize);
    deleteButton->setFixedSize(buttonSize);
    showNewViewButton->setFixedSize(buttonSize);
    definitionButton->setFixedSize(buttonSize);
    implementationButton->setFixedSize(buttonSize);
    instancesButton->setFixedSize(buttonSize);

    addChildButton->setIconSize(buttonSize*0.65);
    connectButton->setIconSize(buttonSize*0.6);
    deleteButton->setIconSize(buttonSize*0.75);
    showNewViewButton->setIconSize(buttonSize*0.6);
    definitionButton->setIconSize(buttonSize);
    implementationButton->setIconSize(buttonSize);
    instancesButton->setIconSize(buttonSize*0.65);

    addChildButton->setToolTip("Add Child Node");
    connectButton->setToolTip("Connect Node");
    deleteButton->setToolTip("Delete Node");
    showNewViewButton->setToolTip("Show in New Window");
    definitionButton->setToolTip("Show Definition");
    implementationButton->setToolTip("Show Implementation");
    instancesButton->setToolTip("Show Instances");

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
    layout->addWidget(instancesButton);

    layout->setMargin(5);
    setLayout(layout);
}


/**
 * @brief ToolbarWidget::setupMenus
 */
void ToolbarWidget::setupMenus()
{
    addMenu = new ToolbarWidgetMenu(0, 0, addChildButton);
    addChildButton->setPopupMode(QToolButton::InstantPopup);
    addChildButton->setMenu(addMenu);

    connectMenu = new ToolbarWidgetMenu(0, 0, connectButton);
    connectButton->setPopupMode(QToolButton::InstantPopup);
    connectButton->setMenu(connectMenu);

    definitionMenu = new ToolbarWidgetMenu(0, 0, definitionButton);
    definitionButton->setPopupMode(QToolButton::InstantPopup);
    definitionButton->setMenu(definitionMenu);

    QAction* defn_goTo = definitionMenu->addAction(QIcon(":/Resources/Icons/goto.png"), "Go to Definition");
    QAction* defn_popup = definitionMenu->addAction(QIcon(":/Resources/Icons/popup.png"), "Popup Definition");
    connect(defn_goTo, SIGNAL(triggered()), this, SLOT(goToDefinition()));
    connect(defn_popup, SIGNAL(triggered()), this, SLOT(makeNewView()));

    implementationMenu = new ToolbarWidgetMenu(0, 0, implementationButton);
    implementationButton->setPopupMode(QToolButton::InstantPopup);
    implementationButton->setMenu(implementationMenu);

    QAction* impl_goTo = implementationMenu->addAction(QIcon(":/Resources/Icons/goto.png"), "Go to Implementation");
    QAction* impl_popup = implementationMenu->addAction(QIcon(":/Resources/Icons/popup.png"), "Popup Implementation");
    connect(impl_goTo, SIGNAL(triggered()), this, SLOT(goToImplementation()));
    connect(impl_popup, SIGNAL(triggered()), this, SLOT(makeNewView()));

    instancesMenu = new ToolbarWidgetMenu(0, 0, instancesButton);
    instanceOptionMenu = new ToolbarWidgetMenu(0, 0, instancesMenu);
    instancesButton->setPopupMode(QToolButton::InstantPopup);
    instancesButton->setMenu(instancesMenu);

    QAction* inst_goTo = instanceOptionMenu->addAction(QIcon(":/Resources/Icons/goto.png"), "Go to Instance");
    QAction* inst_popup = instanceOptionMenu->addAction(QIcon(":/Resources/Icons/popup.png"), "Popup Instance");
    connect(inst_goTo, SIGNAL(triggered()), this, SLOT(goToInstance()));
    connect(inst_popup, SIGNAL(triggered()), this, SLOT(makeNewView()));

    // this is used when ComponentInstance can be adopted by the current node
    componentInstanceAction = new ToolbarWidgetAction("ComponentInstance", "", addMenu);
    inEventPortDelegateAction = new ToolbarWidgetAction("InEventPortDelegate", "", addMenu);
    outEventPortDelegateAction = new ToolbarWidgetAction("OutEventPortDelegate", "", addMenu);

    // default actions for when some of the menus are empty
    fileDefaultAction = new ToolbarWidgetAction("info", "There are no Files.", this);
    componentInstanceDefaultAction = new ToolbarWidgetAction("info", "This File has no Components.", this);
    eventPort_componentInstanceDefaultAction = new ToolbarWidgetAction("info", "This Assembly has no ComponentInstances.", this);
    inEventPortDefaultAction = new ToolbarWidgetAction("info", "There are no InEventPorts.", this);
    outEventPortDefaultAction = new ToolbarWidgetAction("info", "There are no OutEventPorts.", this);

    // hidden menus for COmponentInstances and InEvent/OutEvent PortDelegates
    fileMenu = new ToolbarWidgetMenu(componentInstanceAction, fileDefaultAction, addMenu);
    inEventPort_componentInstanceMenu = new ToolbarWidgetMenu(inEventPortDelegateAction, eventPort_componentInstanceDefaultAction, addMenu);
    outEventPort_componentInstanceMenu = new ToolbarWidgetMenu(outEventPortDelegateAction, eventPort_componentInstanceDefaultAction, addMenu);
}


/**
 * @brief ToolbarWidget::makeConnections
 */
void ToolbarWidget::makeConnections()
{
    connect(addMenu, SIGNAL(toolbarMenu_hideToolbar(bool)), this, SLOT(hideToolbar(bool)));
    connect(connectMenu, SIGNAL(toolbarMenu_hideToolbar(bool)), this, SLOT(hideToolbar(bool)));
    connect(definitionMenu, SIGNAL(toolbarMenu_hideToolbar(bool)), this, SLOT(hideToolbar(bool)));
    connect(implementationMenu, SIGNAL(toolbarMenu_hideToolbar(bool)), this, SLOT(hideToolbar(bool)));
    connect(instancesMenu, SIGNAL(toolbarMenu_hideToolbar(bool)), this, SLOT(hideToolbar(bool)));

    connect(deleteButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(showNewViewButton, SIGNAL(clicked()), this, SLOT(makeNewView()));

    connect(deleteButton, SIGNAL(clicked()), nodeView, SLOT(deleteSelection()));
}


/**
 * @brief ToolbarWidget::updateToolButtons
 * Show/hide basic tool buttons for the currently selected nodeItem.
 */
void ToolbarWidget::updateToolButtons()
{
    // show/hide the delete button depending on the nodeKind
    QString nodeKind = nodeItem->getNodeKind();
    if (nodeKind.endsWith("Definitions") || nodeKind == "ManagementComponent") {
        deleteButton->hide();
    } else {
        deleteButton->show();
    }

    // if any of the defn/impl/inst buttons are visible, frameVisibilityCount > 0
    if (frameVisibilityCount > 0) {
        frame->show();
    } else {
        frame->hide();
    }
    frameVisibilityCount = 0;
}


/**
 * @brief ToolbarWidget::updateMenuLists
 * This updates the lists for all the applicable menus for the selected node item.
 */
void ToolbarWidget::updateMenuLists()
{
    // TODO: Do check here for differences between current list and
    // new list for menus instead of just clearing them
    clearMenus();

    setupAdoptableNodesList(nodeView->getAdoptableNodeList(nodeItem->getNode()));
    setupLegalNodesList(nodeView->getConnectableNodes(nodeItem->getNode()));
    setupInstancesList(nodeItem->getNode()->getInstances());

    // if selected node is a ComponentAssembly, get the Files and ComponentInstance lists
    // for the add ComponentInstance and In/OutEventPortDelegate menus respectively
    if (nodeItem->getNodeKind() == "ComponentAssembly") {
        setupFilesList(nodeView->getFiles());
        setupChildrenComponentInstanceList(nodeItem->getNode()->getChildrenOfKind("ComponentInstance"));
    }
}


/**
 * @brief ToolbarWidget::setupInstancesList
 */
void ToolbarWidget::setupInstancesList(QList<Node*> instances)
{
    if (instances.count() == 0) {
        instancesButton->hide();
        return;
    } else {
        instancesButton->show();
        frameVisibilityCount++;
    }

    for (int i=0; i<instances.count(); i++) {
        ToolbarWidgetAction* action  = new ToolbarWidgetAction(instances.at(i), this);
        connect(action, SIGNAL(toolbarAction_pressed()), this, SLOT(attachOptionMenu()));
        instancesMenu->addWidgetAction(action);
    }
}


/**
 * @brief ToolbarWidget::setupAdoptableNodesList
 * @param nodeKinds
 */
void ToolbarWidget::setupAdoptableNodesList(QStringList nodeKinds)
{
    // if the list has no contents, hide the button
    if (nodeKinds.count() == 0) {
        addChildButton->hide();
        return;
    } else {
        addChildButton->show();
    }

    for (int i=0; i<nodeKinds.count(); i++) {

        QString nodeKind = nodeKinds.at(i);
        ToolbarWidgetAction* action;

        if (nodeKind == "ComponentInstance") {
            action = componentInstanceAction;
        } else if (nodeKind == "InEventPortDelegate") {
            action = inEventPortDelegateAction;
        } else if (nodeKind == "OutEventPortDelegate") {
            action = outEventPortDelegateAction;
        } else {
            action  = new ToolbarWidgetAction(nodeKind, "", this);
            connect(action, SIGNAL(triggered()), this, SLOT(addChildNode()));
        }

        addMenu->addWidgetAction(action);
    }
}


/**
 * @brief ToolbarWidget::setupLegalNodesList
 * @param nodeList
 */
void ToolbarWidget::setupLegalNodesList(QList<Node*> nodeList)
{
    // if the list has no contents, hide the button
    if (nodeList.count() == 0) {
        connectButton->hide();
        return;
    } else {
        connectButton->show();
    }

    for (int i=0; i<nodeList.count(); i++) {
        ToolbarWidgetAction* action = new ToolbarWidgetAction(nodeList.at(i), this);
        connectMenu->addWidgetAction(action);
        connect(action, SIGNAL(triggered()), this, SLOT(connectNodes()));
    }
}


/**
 * @brief ToolbarWidget::setupFilesList
 * @param files
 */
void ToolbarWidget::setupFilesList(QList<Node*> files)
{
    // if the list has no contents, change componentInstanceAction checkable state
    if (files.count() == 0) {
        componentInstanceAction->getButton()->setCheckable(false);
        return;
    } else {
        componentInstanceAction->getButton()->setCheckable(true);
    }

    for (int i=0; i< files.count(); i++) {
        ToolbarWidgetAction* fileAction = new ToolbarWidgetAction(files.at(i), fileMenu, "file");
        ToolbarWidgetMenu* fileActionMenu = new ToolbarWidgetMenu(fileAction, componentInstanceDefaultAction, fileMenu);
        fileMenu->addWidgetAction(fileAction);
    }

    setupComponentInstanceList(nodeView->getComponents());
}


/**
 * @brief ToolbarWidget::setupComponentInstanceList
 * @param components
 */
void ToolbarWidget::setupComponentInstanceList(QList<Node*> components)
{
    for (int i=0; i<components.count(); i++) {
        foreach (QAction* action, fileMenu->actions()) {

            ToolbarWidgetAction* fileAction = qobject_cast<ToolbarWidgetAction*>(action);
            QString fileID = components.at(i)->getParentNode()->getID();

            if (fileID == fileAction->getNode()->getID()) {
                ToolbarWidgetAction* action = new ToolbarWidgetAction(components.at(i), this, "instance");
                connect(action, SIGNAL(triggered()), this, SLOT(addComponentInstance()));
                fileAction->getMenu()->addWidgetAction(action);
                break;
            }
        }
    }
}


/**
 * @brief ToolbarWidget::setupChildrenComponentInstanceList
 * @param componentInstances
 */
void ToolbarWidget::setupChildrenComponentInstanceList(QList<Node*> componentInstances)
{
    if (componentInstances.count() > 0) {

        foreach (Node* instance, componentInstances) {

            ToolbarWidgetAction* inEvent_instanceAction = new ToolbarWidgetAction(instance, inEventPort_componentInstanceMenu, "eventPort");
            ToolbarWidgetMenu* inEventPortMenu = new ToolbarWidgetMenu(inEvent_instanceAction, inEventPortDefaultAction, inEventPort_componentInstanceMenu);

            ToolbarWidgetAction* outEvent_instanceAction = new ToolbarWidgetAction(instance, outEventPort_componentInstanceMenu, "eventPort");
            ToolbarWidgetMenu* outEventPortMenu = new ToolbarWidgetMenu(outEvent_instanceAction, outEventPortDefaultAction, outEventPort_componentInstanceMenu);

            inEventPort_componentInstanceMenu->addWidgetAction(inEvent_instanceAction);
            outEventPort_componentInstanceMenu->addWidgetAction(outEvent_instanceAction);
        }

        // setup menu lists for InEventPort/OutEventPort Delegates
        setupInEventPortInstanceList();
        setupOutEventPortInstanceList();
    }
}


/**
 * @brief ToolbarWidget::setupInEventPortInstanceList
 */
void ToolbarWidget::setupInEventPortInstanceList()
{
    foreach (ToolbarWidgetAction* instanceAction, inEventPort_componentInstanceMenu->getWidgetActions()) {

        QList<Node*> children = instanceAction->getNode()->getChildren(0);

        foreach (Node* child, children) {
            if (child->getDataValue("kind") == "InEventPortInstance") {
                ToolbarWidgetAction* eventPortAction = new ToolbarWidgetAction(child, inEventPort_componentInstanceMenu, "");
                if (instanceAction->getMenu()) {
                    instanceAction->getMenu()->addWidgetAction(eventPortAction);
                    connect(eventPortAction, SIGNAL(triggered()), this, SLOT(addEventPorDelegate()));
                } else {
                    qDebug() << "setupInEventPortInstanceList - ComponentInstance doesn't have a menu!";
                }
            }
        }
    }
}


/**
 * @brief ToolbarWidget::setupOutEventPortInstanceList
 */
void ToolbarWidget::setupOutEventPortInstanceList()
{
    foreach (ToolbarWidgetAction* instanceAction, outEventPort_componentInstanceMenu->getWidgetActions()) {

        QList<Node*> children = instanceAction->getNode()->getChildren(0);

        foreach (Node* child, children) {
            if (child->getDataValue("kind") == "OutEventPortInstance") {
                ToolbarWidgetAction* eventPortAction = new ToolbarWidgetAction(child, outEventPort_componentInstanceMenu, "");
                if (instanceAction->getMenu()) {
                    instanceAction->getMenu()->addWidgetAction(eventPortAction);
                    connect(eventPortAction, SIGNAL(triggered()), this, SLOT(addEventPorDelegate()));
                } else {
                    qDebug() << "setupOutEventPortInstanceList - ComponentInstance doesn't have a menu!";
                }
            }
        }
    }
}


/**
 * @brief ToolbarWidget::clearMenus
 * Clear all of the stored menus.
 */
void ToolbarWidget::clearMenus()
{
    addMenu->clearMenu();
    connectMenu->clearMenu();
    fileMenu->clearMenu();
    inEventPort_componentInstanceMenu->clearMenu();
    outEventPort_componentInstanceMenu->clearMenu();
    instancesMenu->clearMenu();
}
