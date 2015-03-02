#include "toolbarwidget.h"
#include "toolbarwidgetmenu.h"

#include <QDebug>
#include <QHBoxLayout>


/**
 * @brief ToolbarWidget::ToolbarWidget
 * @param parent
 */
ToolbarWidget::ToolbarWidget(NodeView *parent) :
    QWidget(parent)
{
    parentNodeView = parent;
    nodeItem = 0;
    prevNodeItem = 0;
    eventFromToolbar = false;

    definitionNode = 0;
    implementationNode = 0;

    setBackgroundRole(QPalette::Dark);
    setWindowFlags(windowFlags() | Qt::Popup);

    /*
    setStyleSheet("ToolbarWidget{"
                  "background-color: rgba(230,230,230,250);"
                  "}"

                  "QToolButton{"
                  "background-color: rgba(250,250,250,200);"
                  "}");
    */

    setupToolBar();
    setupMenus();
    makeConnections();
}

void ToolbarWidget::setCurrentNodeItem(NodeItem *item)
{
    //Dans method.
    currentSelectedItem = item;

    Node* selectedNode = item->getNode();

    Node* itemDefinition = parentNodeView->hasDefinition(selectedNode);
    Node* itemImplementation = parentNodeView->hasDefinition(selectedNode);

    showDefinitionButton(itemDefinition);
    showImplementationButton(itemImplementation);

    QStringList adoptableNodeList = parentNodeView->getAdoptableNodeList(selectedNode);
    setupAdoptableNodesList(&adoptableNodeList);

    QList<Node*> connectableNodeList = parentNodeView->getConnectableNodes(selectedNode);
    setupLegalNodesList(&connectableNodeList);


    if(selectedNode->getDataValue("kind") == "ComponentAssembly"){
        QList<Node*> outEventPortDelegates = selectedNode->getChildrenOfKind("OutEventPort", 1);
        qCritical() << selectedNode->toString() << " Has " << outEventPortDelegates.size() << " OEP's it could use!";
        QList<Node*> inEventPortDelegates = selectedNode->getChildrenOfKind("InEventPort", 1);
        qCritical() << selectedNode->toString() << " Has " << inEventPortDelegates.size() << " IEP's it could use!";
    }
}


/**
 * @brief ToolbarWidget::setNodeItem
 * This only gets called when the toolbar is about to show.
 * Set nodeItem to the currectly selected item.
 * Update applicable tool buttons only if prev != item.
 * @param item
 */
void ToolbarWidget::setNodeItem(NodeItem *item)
{
    nodeItem = item;
    if (prevNodeItem != nodeItem) {
        clearMenus();
        updateToolButtons();
    }
    prevNodeItem = nodeItem;
}


/**
 * @brief ToolbarWidget::showDefinitionButton
 * This method shows/hides the definitionButton and sets the definitionNode if there is one.
 * @param show
 */
void ToolbarWidget::showDefinitionButton(Node *definition)
{
    if (definition) {
        definitionNode = definition;
        definitionButton->show();
    } else {
        definitionButton->hide();
    }
}


/**
 * @brief ToolbarWidget::showImplementationButton
 * This method shows/hides the implementationButton and sets the implementationNode if there is one.
 * @param show
 */
void ToolbarWidget::showImplementationButton(Node* implementation)
{
    if (implementation) {
        implementationNode = implementation;
        implementationButton->show();
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
    //emit goToDefinition(currentSelectedItem->getNode());
    emit goToDefinition(nodeItem->getNode());
}


/**
 * @brief ToolbarWidget::goToImplementation
 * This sends a signal to the view to center the selected nodeitem's implementation.
 */
void ToolbarWidget::goToImplementation()
{
    //emit goToImplementation(currentSelectedItem->getNode());
    emit goToImplementation(nodeItem->getNode());
}


/**
 * @brief ToolbarWidget::getAdoptableNodeList
 * This sends a signal to the view to get the selected nodeitem's adoptable nodes list.
 */
void ToolbarWidget::getAdoptableNodesList()
{
    emit updateMenuList("add", nodeItem->getNode());
}


/**
 * @brief ToolbarWidget::getLegalNodesList
 * This sends a signal to the view to get the selected nodeitem's list of nodes it can connect to.
 */
void ToolbarWidget::getLegalNodesList()
{
    emit updateMenuList("connect", nodeItem->getNode());
}


/**
 * @brief ToolbarWidget::getFilesList
 * This sends a signal to NewMedea to get the list of Files from the NodeView.
 */
void ToolbarWidget::getFilesList()
{
    emit updateMenuList("files", nodeItem->getNode());
}


/**
 * @brief ToolbarWidget::getComponenttInstanceList
 * This only gets called if the selected node is a ComponentAssembly.
 * It gets all of its ComponentInstance children and it is used to setup
 * the InEventPort/OutEventPort menu lists for the Delegates.
 */
void ToolbarWidget::getComponentInstanceList()
{
    QList<Node*> children = nodeItem->getNode()->getChildren(0);

    if (children.count() > 0) {

        QList<Node*> *componentInstances = new QList<Node*>();

        foreach (Node* child, children) {
            QString childKind = child->getDataValue("kind");
            if (childKind == "ComponentInstance") {
                componentInstances->append(child);
            }
        }

        setupChildrenComponentInstanceList(componentInstances);
    }
}


/**
 * @brief ToolbarWidget::updateMenuList
 * @param action - add/connect/addInstance
 * @param nodeKinds - if action = add; list of adoptable node kinds
 * @param nodeList - if action = connect; list of nodes the selected item can connect to
 *                 - if action = addInstance; list of existing Component definitions
 */
void ToolbarWidget::updateMenuList(QString action, QStringList* stringList, QList<Node*>* nodeList)
{
    if (action == "add" && stringList != 0) {
        setupAdoptableNodesList(stringList);
    } else if (action == "connect" && nodeList != 0) {
        setupLegalNodesList(nodeList);
    } else if (action == "addInstance" && nodeList != 0) {
        setupComponentInstanceList(nodeList);
    } else if (action == "files" && nodeList != 0) {
        setupFilesList(nodeList);
    }
}


/**
 * @brief ToolbarWidget::constructNode
 * Send a signal to the view to construct a new node with the specified kind.
 */
void ToolbarWidget::addChildNode()
{
    ToolbarWidgetAction* action = qobject_cast<ToolbarWidgetAction*>(QObject::sender());
    emit constructNode(action->getKind());
}


/**
 * @brief ToolbarWidget::connectNodes
 * Send a signal to the view to construct an edge between the selected node and chosen node.
 */
void ToolbarWidget::connectNodes()
{
    ToolbarWidgetAction* action = qobject_cast<ToolbarWidgetAction*>(QObject::sender());
    emit constructEdge(nodeItem->getNode(), action->getNode());
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
        emit constructNewView(parentNodeView->getSelectedNode());
        return;
    }

    // pop up the selected node's definition/implementation into a new window
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
 * Send a signal to the view to construct a ComponentInstance of the chosen action's node.
 */
void ToolbarWidget::addComponentInstance()
{
    ToolbarWidgetAction* action = qobject_cast<ToolbarWidgetAction*>(QObject::sender());
    emit constructComponentInstance(nodeItem->getNode(), action->getNode(), 1);
}


/**
 * @brief ToolbarWidget::hideToolbar
 * This method checks if hiding the menu was triggered by the toolbar.
 * If the event came from outside the toolbar, hide the toolbar and all visible menus.
 */
void ToolbarWidget::hideToolbar()
{
    if (!eventFromToolbar) {
        hide();
        emit toolbarClosed();
    }
}


/**
 * @brief ToolbarWidget::setupToolBar
 */
void ToolbarWidget::setupToolBar()
{
    QHBoxLayout* layout = new QHBoxLayout();
    QSize buttonSize = QSize(35,35);

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

    addChildButton->setIconSize(buttonSize*0.8);
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
    connect(addMenu, SIGNAL(hideToolbar()), this, SLOT(hideToolbar()));
    connect(connectMenu, SIGNAL(hideToolbar()), this, SLOT(hideToolbar()));
    connect(definitionMenu, SIGNAL(hideToolbar()), this, SLOT(hideToolbar()));
    connect(implementationMenu, SIGNAL(hideToolbar()), this, SLOT(hideToolbar()));

    connect(deleteButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(showNewViewButton, SIGNAL(clicked()), this, SLOT(makeNewView()));

    connect(inEventPortDelegateAction, SIGNAL(triggered()), this, SLOT(getComponentInstanceList()));
    connect(outEventPortDelegateAction, SIGNAL(triggered()), this, SLOT(getComponentInstanceList()));

    connectToView();
}


/**
 * @brief ToolbarWidget::connectToView
 */
void ToolbarWidget::connectToView()
{
    connect(this, SIGNAL(toolbarClosed()), parentNodeView, SLOT(toolbarClosed()));

    connect(deleteButton, SIGNAL(clicked()), parentNodeView, SLOT(view_deleteSelectedNode()));

    connect(this, SIGNAL(goToDefinition(Node*)), parentNodeView, SLOT(goToDefinition(Node*)));
    connect(this, SIGNAL(goToImplementation(Node*)), parentNodeView, SLOT(goToImplementation(Node*)));

    connect(this, SIGNAL(updateMenuList(QString,Node*)), parentNodeView, SLOT(updateToolbarMenuList(QString,Node*)));
    connect(parentNodeView, SIGNAL(updateMenuList(QString,QStringList*,QList<Node*>*)), this, SLOT(updateMenuList(QString,QStringList*,QList<Node*>*)));

    connect(this, SIGNAL(constructNode(QString)), parentNodeView, SLOT(view_ConstructNode(QString)));
    connect(this, SIGNAL(constructEdge(Node*,Node*)), parentNodeView, SLOT(view_ConstructEdge(Node*,Node*)));
    connect(this, SIGNAL(constructComponentInstance(Node*,Node*,int)), parentNodeView, SLOT(view_ConstructComponentInstance(Node*,Node*,int)));
    connect(this, SIGNAL(constructNewView(Node*)), parentNodeView, SLOT(constructNewView(Node*)));
}


/**
 * @brief ToolbarWidget::updateToolButtons
 * Hide the tool buttons that don't apply to the selected node item.
 */
void ToolbarWidget::updateToolButtons()
{
    QString nodeKind = nodeItem->getNodeKind();
    bool showFrame = true;

    // this can be removed when the returned adoptable node list is updated
    if (nodeKind == "ComponentInstance") {
        addChildButton->hide();
    } else {
        getAdoptableNodesList();
    }

    if (nodeKind.endsWith("Definitions") || nodeKind == "ManagementComponent") {
        deleteButton->hide();
        connectButton->hide();
        showFrame = false;
    } else {
        deleteButton->show();
        getLegalNodesList();
        if (nodeKind == "ComponentAssembly") {
            getFilesList();
        }
    }

    // hide frame if nodeKind.startsWith("Hardware") & showNewView is the only button visible
    if (nodeKind.startsWith("Hardware") || nodeKind == "ManagementComponent" && !showFrame) {
        frame->hide();
    } else {
        frame->show();
    }
}


/**
 * @brief ToolbarWidget::setupAdoptableNodesList
 * @param nodeKinds
 */
void ToolbarWidget::setupAdoptableNodesList(QStringList* nodeKinds)
{
    // if the list has no contents, hide the button
    if (nodeKinds->count() == 0) {
        addChildButton->hide();
        return;
    } else {
        addChildButton->show();
    }


    //DAN ADDED
    // Dan, I commented out the connect() for the EventPortDelegates because they're already
    // being connected to a different slot in makeConnections to getComponentInstanceList.
    // I wanted to show which ComponentInstance the event ports belonged to.
    for (int i=0; i<nodeKinds->count(); i++) {

        QString nodeKind = nodeKinds->at(i);
        ToolbarWidgetAction* action;

        if (nodeKind == "ComponentInstance") {
            action = componentInstanceAction;
        } else if (nodeKind == "InEventPortDelegate") {
            action = inEventPortDelegateAction;
            //connect(action, SIGNAL(triggered()), this, SLOT(setupInEventPortInstanceList()));
        } else if (nodeKind == "OutEventPortDelegate") {
            action = outEventPortDelegateAction;
            //connect(action, SIGNAL(triggered()), this, SLOT(setupOutEventPortInstanceList()));
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
void ToolbarWidget::setupLegalNodesList(QList<Node*> *nodeList)
{
    // if the list has no contents, hide the button
    if (nodeList->count() == 0) {
        connectButton->hide();
        return;
    } else {
        connectButton->show();
    }

    //DAN ADDED
    connectMenu->clear();

    for (int i=0; i<nodeList->count(); i++) {
        ToolbarWidgetAction* action = new ToolbarWidgetAction(nodeList->at(i), this);
        connectMenu->addAction(action);
        connect(action, SIGNAL(triggered()), this, SLOT(connectNodes()));
    }
}


/**
 * @brief ToolbarWidget::setupFilesList
 * @param files
 */
void ToolbarWidget::setupFilesList(QList<Node*> *files)
{
    fileMenu->clearMenu();

    // if the list has no contents, change componentInstanceAction checkable state
    if (files->count() == 0) {
        componentInstanceAction->getButton()->setCheckable(false);
        return;
    } else {
        componentInstanceAction->getButton()->setCheckable(true);
    }

    for (int i=0; i< files->count(); i++) {
        ToolbarWidgetAction* fileAction = new ToolbarWidgetAction(files->at(i), fileMenu, "file");
        ToolbarWidgetMenu* fileActionMenu = new ToolbarWidgetMenu(fileAction, componentInstanceDefaultAction, fileMenu);
        connect(fileAction, SIGNAL(triggered()), this, SLOT(getFilesList()));
        fileMenu->addWidgetAction(fileAction);
    }

    emit updateMenuList("addInstance", nodeItem->getNode());
}


/**
 * @brief ToolbarWidget::setupComponentInstanceList
 * @param instances
 */
void ToolbarWidget::setupComponentInstanceList(QList<Node*> *components)
{
    for (int i=0; i<components->count(); i++) {
        foreach (QAction* action, fileMenu->actions()) {

            ToolbarWidgetAction* fileAction = qobject_cast<ToolbarWidgetAction*>(action);
            QString fileID = components->at(i)->getParentNode()->getID();

            if (fileID == fileAction->getNode()->getID()) {
                ToolbarWidgetAction* action = new ToolbarWidgetAction(components->at(i), this, "instance");
                connect(action, SIGNAL(triggered()), this, SLOT(addComponentInstance()));
                fileAction->getMenu()->addWidgetAction(action);
                break;
            }
        }
    }
}


/**
 * @brief ToolbarWidget::setupComponentInstanceList_EventPorts
 * @param components
 */
void ToolbarWidget::setupChildrenComponentInstanceList(QList<Node*> *componentInstances)
{
    inEventPort_componentInstanceMenu->clear();
    outEventPort_componentInstanceMenu->clear();

    if (componentInstances->count() > 0) {

        foreach (Node* instance, *componentInstances) {

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
 * @param instances
 */
void ToolbarWidget::setupInEventPortInstanceList()
{
    foreach (ToolbarWidgetAction* action, inEventPort_componentInstanceMenu->getWidgetActions()) {

        QList<Node*> children = action->getNode()->getChildren(0);

        foreach (Node* child, children) {
            if (child->getDataValue("kind") == "InEventPortInstance") {
                ToolbarWidgetAction* action = new ToolbarWidgetAction(child, inEventPort_componentInstanceMenu, "");
                action->getMenu()->addWidgetAction(action);
            }
        }
    }
}


/**
 * @brief ToolbarWidget::setupOutEventPortInstanceList
 * @param instances
 */
void ToolbarWidget::setupOutEventPortInstanceList()
{
    foreach (ToolbarWidgetAction* action, outEventPort_componentInstanceMenu->getWidgetActions()) {

        QList<Node*> children = action->getNode()->getChildren(0);

        foreach (Node* child, children) {
            if (child->getDataValue("kind") == "OutEventPortInstance") {
                ToolbarWidgetAction* action = new ToolbarWidgetAction(child, outEventPort_componentInstanceMenu, "");
                action->getMenu()->addWidgetAction(action);
            }
        }
    }
}


/**
 * @brief ToolbarWidget::clearMenus
 * Clear the stored menus.
 */
void ToolbarWidget::clearMenus()
{
    addMenu->clearMenu();
    connectMenu->clearMenu();
    fileMenu->clearMenu();
    inEventPort_componentInstanceMenu->clearMenu();
    outEventPort_componentInstanceMenu->clearMenu();
}
