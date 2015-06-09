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

    definitionNode = 0;
    implementationNode = 0;

    eventFromToolbar = false;
    showToolbar = false;
    showAlterViewFrame = false;
    showSnippetFrame = false;
    showGoToFrame = false;

    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::Popup);
    setStyleSheet("QToolButton{"
                  "background-color: white;"
                  "border: 1px solid grey;"
                  "border-radius: 10px;"
                  "}");

    // these frames, combined with the set attribute and flags, allow
    // the toolbar to have a translucent background and a mock shadow
    shadowFrame = new QFrame(this);
    shadowFrame->setStyleSheet("background-color: rgba(50,50,50,150);"
                               "border-radius: 10px;");

    mainFrame = new QFrame(this);
    mainFrame->setStyleSheet("background-color: rgba(250,250,250,200);"
                             "border-radius: 8px;");

    setupToolBar();
    setupMenus();
    makeConnections();
}


/**
 * @brief ToolbarWidget::updateSelectedNodeItem
 * This only gets called when the toolbar is about to show.
 * Set nodeItem to the currectly selected item and update applicable tool buttons and menus.
 * If there are multiple items selected, only update the buttons that are for multiple selection.
 * @param items
 */
void ToolbarWidget::updateSelectedNodeItem(QList<NodeItem*> items)
{   
    if (items.count() == 1) {
        nodeItem = items.at(0);
        alterModelToolButtons.clear();
        updateToolButtons();
        updateMenuLists();
    } else if (items.count() > 1) {
        multipleSelection(items);
    } else {
        // it shouldn't get to this function if there aren't any selected items
        qWarning() << "ToolbarWidget::updateSelectedNodeItem - There are no selected items.";
        return;
    }

    // update the toolbar & frame sizes after the tool buttons have been updated
    mainFrame->setFixedSize(layout()->sizeHint());
    shadowFrame->setFixedSize(layout()->sizeHint() + QSize(3,3));
    setFixedSize(shadowFrame->size());

    mainFrame->setVisible(showToolbar);
    shadowFrame->setVisible(showToolbar);
    setVisible(showToolbar);
    showToolbar = false;
}


/**
 * @brief ToolbarWidget::showDefinitionButton
 * This method shows/hides the definitionButton and sets the definitionNode if there is one.
 * This gets called everytime a node item is selected.
 * @param definition
 */
void ToolbarWidget::showDefinitionButton(Node *definition)
{
    if (definition) {
        definitionNode = definition;
        definitionButton->show();
        showGoToFrame = true;
        showToolbar = true;
    } else {
        definitionButton->hide();
    }
}


/**
 * @brief ToolbarWidget::showImplementationButton
 * This method shows/hides the implementationButton and sets the implementationNode if there is one.
 * This gets called everytime a node item is selected.
 * @param implementation
 */
void ToolbarWidget::showImplementationButton(Node* implementation)
{
    if (implementation) {
        implementationNode = implementation;
        implementationButton->show();
        showGoToFrame = true;
        showToolbar = true;
    } else {
        implementationButton->hide();
    }
}


/**
 * @brief ToolbarWidget::showExportSnippetButton
 * This shows/hides the export GraphML snippet button and its separator frame.
 * @param button
 * @param show
 */
void ToolbarWidget::showSnippetButton(QString button, bool show)
{
    if (button == "export") {
        exportSnippetButton->setVisible(show);
    } else if (button == "import") {
        importSnippetButton->setVisible(show);
    }
    if (show) {
        showSnippetFrame = true;
        showToolbar = true;
    }
}


/**
 * @brief ToolbarWidget::updateSeparators
 * @param snippet
 * @param goTo
 */
void ToolbarWidget::updateSeparators(bool snippet, bool goTo)
{
    // show frame if any of the snippet buttons are visible
    if (snippet) {
        snippetFrame->setVisible(showSnippetFrame);
        showSnippetFrame = false;
    }

    // show frame if any of the goto buttons are visible
    if (goTo) {
        goToFrame->setVisible(showGoToFrame);
        showGoToFrame = false;
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
 * This sends a signal to the view to center on the selected nodeitem's definition.
 */
void ToolbarWidget::goToDefinition()
{
    nodeView->goToDefinition(nodeItem->getNode());
}


/**
 * @brief ToolbarWidget::goToImplementation
 * This sends a signal to the view to center on the selected nodeitem's implementation.
 */
void ToolbarWidget::goToImplementation()
{
    nodeView->goToImplementation(nodeItem->getNode());
}


/**
 * @brief ToolbarWidget::goToInstance
 * This sends a signal to the view to center on the selected nodeitem's chosen instance.
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
 * @brief ToolbarWidget::exportGraphMLSnippet
 */
void ToolbarWidget::exportGraphMLSnippet()
{
    nodeView->exportSnippet();
}


/**
 * @brief ToolbarWidget::importGraphMLSnippet
 */
void ToolbarWidget::importGraphMLSnippet()
{
    Node* parentNode = nodeView->getSelectedNode();
    if (parentNode) {
        nodeView->view_ImportSnippet(parentNode->getDataValue("kind"));
    }
}


/**
 * @brief ToolbarWidget::addConnectedNode
 * Send a signal to the view to construct a connected node.
 * It can either be a ComponentImpl, ComponentInstance or In/Out EventPortDelegates.
 */
void ToolbarWidget::addConnectedNode()
{
    ToolbarWidgetAction* action = qobject_cast<ToolbarWidgetAction*>(QObject::sender());
    ToolbarWidgetMenu* parentMenu = qobject_cast<ToolbarWidgetMenu*>(action->parentWidget());
    ToolbarWidgetAction* menuParentAction = parentMenu->getParentAction();

    // check which action was triggered & where it originates from to determine the nodeKind to construct
    if (menuParentAction) {

        QString kindToConstruct;

        if (menuParentAction == componentImplAction) {
            kindToConstruct = "ComponentImpl";
        } else if (menuParentAction == componentInstanceAction) {
            kindToConstruct = "ComponentInstance";
        } else if (menuParentAction == inEventPortDelegateAction) {
            kindToConstruct = "InEventPortDelegate";
        } else if (menuParentAction == outEventPortDelegateAction) {
            kindToConstruct = "OutEventPortDelegate";
        }

        nodeView->constructConnectedNode(nodeItem->getNode(), action->getNode(), kindToConstruct, 1);
    }
}


/**
 * @brief ToolbarWidget::attachOptionMenu
 * This attaches the view options menu to the chosen instance from the instance menu list.
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
 * Initialise and setup the layout and tool buttons.
 */
void ToolbarWidget::setupToolBar()
{
    QHBoxLayout* layout = new QHBoxLayout();
    QSize buttonSize = QSize(39,39);

    addChildButton = new QToolButton(this);
    connectButton = new QToolButton(this);
    deleteButton = new QToolButton(this);
    showNewViewButton = new QToolButton(this);
    showConnectionsButton = new QToolButton(this);
    definitionButton = new QToolButton(this);
    implementationButton = new QToolButton(this);
    instancesButton = new QToolButton(this);
    alignVerticallyButton = new QToolButton(this);
    alignHorizontallyButton = new QToolButton(this);
    exportSnippetButton = new QToolButton(this);
    importSnippetButton = new QToolButton(this);

    addChildButton->setIcon(QIcon(":/Resources/Icons/addChildNode.png"));
    connectButton->setIcon(QIcon(":/Resources/Icons/connectNode.png"));
    deleteButton->setIcon(QIcon(":/Resources/Icons/deleteNode.png"));
    showNewViewButton->setIcon(QIcon(":/Resources/Icons/popup.png"));
    showConnectionsButton->setIcon(QIcon(":/Resources/Icons/connections.png"));
    definitionButton->setIcon(QIcon(":/Resources/Icons/definition.png"));
    implementationButton->setIcon(QIcon(":/Resources/Icons/implementation.png"));
    instancesButton->setIcon(QIcon(":/Resources/Icons/instance.png"));
    alignVerticallyButton->setIcon(QIcon(":/Resources/Icons/alignVertically.png"));
    alignHorizontallyButton->setIcon(QIcon(":/Resources/Icons/alignHorizontally.png"));
    exportSnippetButton->setIcon(QIcon(":/Resources/Icons/exportSnippet.png"));
    importSnippetButton->setIcon(QIcon(":/Resources/Icons/importSnippet.png"));

    deleteButton->setStyleSheet("padding-right: 3px;");

    addChildButton->setFixedSize(buttonSize);
    connectButton->setFixedSize(buttonSize);
    deleteButton->setFixedSize(buttonSize);
    showNewViewButton->setFixedSize(buttonSize);
    showConnectionsButton->setFixedSize(buttonSize);
    definitionButton->setFixedSize(buttonSize);
    implementationButton->setFixedSize(buttonSize);
    instancesButton->setFixedSize(buttonSize);
    alignVerticallyButton->setFixedSize(buttonSize);
    alignHorizontallyButton->setFixedSize(buttonSize);
    exportSnippetButton->setFixedSize(buttonSize);
    importSnippetButton->setFixedSize(buttonSize);

    addChildButton->setIconSize(buttonSize*0.6);
    connectButton->setIconSize(buttonSize*0.6);
    deleteButton->setIconSize(buttonSize*0.75);
    showNewViewButton->setIconSize(buttonSize*0.55);
    showConnectionsButton->setIconSize(buttonSize*0.6);
    definitionButton->setIconSize(buttonSize*0.55);
    implementationButton->setIconSize(buttonSize*0.6);
    instancesButton->setIconSize(buttonSize*0.6);
    alignVerticallyButton->setIconSize(buttonSize*0.8);
    alignHorizontallyButton->setIconSize(buttonSize*0.8);
    exportSnippetButton->setIconSize(buttonSize*0.65);
    importSnippetButton->setIconSize(buttonSize*0.65);

    addChildButton->setToolTip("Add Child Entity");
    connectButton->setToolTip("Connect Entity");
    deleteButton->setToolTip("Delete Selection");
    showNewViewButton->setToolTip("View In New Window");
    showConnectionsButton->setToolTip("View Connections");
    definitionButton->setToolTip("View Definition");
    implementationButton->setToolTip("View Implementation");
    instancesButton->setToolTip("View Instances");
    alignVerticallyButton->setToolTip("Align Selection Vertically");
    alignHorizontallyButton->setToolTip("Align Selection Horizontally");
    exportSnippetButton->setToolTip("Export GraphML Snippet");
    importSnippetButton->setToolTip("Import GraphML Snippet");

    snippetFrame = new QFrame();
    snippetFrame->setFrameShape(QFrame::VLine);
    snippetFrame->setPalette(QPalette(Qt::darkGray));

    goToFrame = new QFrame();
    goToFrame->setFrameShape(QFrame::VLine);
    goToFrame->setPalette(QPalette(Qt::darkGray));

    alterViewFrame = new QFrame();
    alterViewFrame->setFrameShape(QFrame::VLine);
    alterViewFrame->setPalette(QPalette(Qt::darkGray));

    layout->addWidget(addChildButton);
    layout->addWidget(connectButton);
    layout->addWidget(deleteButton);
    layout->addWidget(alignVerticallyButton);
    layout->addWidget(alignHorizontallyButton);
    layout->addWidget(snippetFrame);
    layout->addWidget(exportSnippetButton);
    layout->addWidget(importSnippetButton);
    layout->addWidget(goToFrame);
    layout->addWidget(definitionButton);
    layout->addWidget(implementationButton);
    layout->addWidget(instancesButton);
    layout->addWidget(alterViewFrame);
    layout->addWidget(showConnectionsButton);
    layout->addWidget(showNewViewButton);

    layout->setMargin(5);
    layout->setAlignment(Qt::AlignTop);
    setLayout(layout);

    // add tool buttons for single selection to list
    singleSelectionToolButtons.append(addChildButton);
    singleSelectionToolButtons.append(connectButton);
    singleSelectionToolButtons.append(showNewViewButton);
    singleSelectionToolButtons.append(showConnectionsButton);
    singleSelectionToolButtons.append(definitionButton);
    singleSelectionToolButtons.append(implementationButton);
    singleSelectionToolButtons.append(instancesButton);

    // add tool buttons for multiple selection to list
    multipleSelectionToolButtons.append(alignVerticallyButton);
    multipleSelectionToolButtons.append(alignHorizontallyButton);
}


/**
 * @brief ToolbarWidget::setupMenus
 * Setup menus, sub-menus and their parent ToolbarWidgetActions.
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
    defn_popup->setEnabled(false);

    implementationMenu = new ToolbarWidgetMenu(0, 0, implementationButton);
    implementationButton->setPopupMode(QToolButton::InstantPopup);
    implementationButton->setMenu(implementationMenu);

    QAction* impl_goTo = implementationMenu->addAction(QIcon(":/Resources/Icons/goto.png"), "Go to Implementation");
    QAction* impl_popup = implementationMenu->addAction(QIcon(":/Resources/Icons/popup.png"), "Popup Implementation");
    connect(impl_goTo, SIGNAL(triggered()), this, SLOT(goToImplementation()));
    connect(impl_popup, SIGNAL(triggered()), this, SLOT(makeNewView()));
    impl_popup->setEnabled(false);

    instancesMenu = new ToolbarWidgetMenu(0, 0, instancesButton);
    instanceOptionMenu = new ToolbarWidgetMenu(0, 0, instancesMenu);
    instancesButton->setPopupMode(QToolButton::InstantPopup);
    instancesButton->setMenu(instancesMenu);

    QAction* inst_goTo = instanceOptionMenu->addAction(QIcon(":/Resources/Icons/goto.png"), "Go to Instance");
    QAction* inst_popup = instanceOptionMenu->addAction(QIcon(":/Resources/Icons/popup.png"), "Popup Instance");
    connect(inst_goTo, SIGNAL(triggered()), this, SLOT(goToInstance()));
    connect(inst_popup, SIGNAL(triggered()), this, SLOT(makeNewView()));
    inst_popup->setEnabled(false);

    // this is used when ComponentInstance can be adopted by the current node
    componentImplAction = new ToolbarWidgetAction("ComponentImpl", "", addMenu);
    componentInstanceAction = new ToolbarWidgetAction("ComponentInstance", "", addMenu);
    inEventPortDelegateAction = new ToolbarWidgetAction("InEventPortDelegate", "", addMenu);
    outEventPortDelegateAction = new ToolbarWidgetAction("OutEventPortDelegate", "", addMenu);

    // default actions for when some of the menus are empty
    fileDefaultAction = new ToolbarWidgetAction("info", "There are no IDL files containing Components that are valid for this action.", this);
    inEventPort_componentInstanceDefaultAction = new ToolbarWidgetAction("info", "This Assembly does not contain any InEventPortInstances that has a definition that is connected to an Aggregate.", this);
    outEventPort_componentInstanceDefaultAction = new ToolbarWidgetAction("info", "This Assembly does not contain any OutEventPortInstances that has a definition that is connected to an Aggregate.", this);

    // hidden menus for parent nodes, ComponentInstances, ComponentImpls and In/Out EventPortDelegates
    fileMenu = new ToolbarWidgetMenu(0, fileDefaultAction, addMenu);
    inEventPort_componentInstanceMenu = new ToolbarWidgetMenu(inEventPortDelegateAction, inEventPort_componentInstanceDefaultAction, addMenu);
    outEventPort_componentInstanceMenu = new ToolbarWidgetMenu(outEventPortDelegateAction, outEventPort_componentInstanceDefaultAction, addMenu);
}


/**
 * @brief ToolbarWidget::makeConnections
 * Connect signals and slots.
 */
void ToolbarWidget::makeConnections()
{
    connect(addMenu, SIGNAL(toolbarMenu_hideToolbar(bool)), this, SLOT(hideToolbar(bool)));
    connect(connectMenu, SIGNAL(toolbarMenu_hideToolbar(bool)), this, SLOT(hideToolbar(bool)));
    connect(definitionMenu, SIGNAL(toolbarMenu_hideToolbar(bool)), this, SLOT(hideToolbar(bool)));
    connect(implementationMenu, SIGNAL(toolbarMenu_hideToolbar(bool)), this, SLOT(hideToolbar(bool)));
    connect(instancesMenu, SIGNAL(toolbarMenu_hideToolbar(bool)), this, SLOT(hideToolbar(bool)));
    connect(instanceOptionMenu, SIGNAL(triggered(QAction*)), instancesMenu, SIGNAL(triggered(QAction*)));

    connect(deleteButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(alignVerticallyButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(alignHorizontallyButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(showNewViewButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(showConnectionsButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(exportSnippetButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(importSnippetButton, SIGNAL(clicked()), this, SLOT(hide()));

    connect(deleteButton, SIGNAL(clicked()), nodeView, SLOT(deleteSelection()));
    connect(alignVerticallyButton, SIGNAL(clicked()), nodeView, SLOT(alignSelectionVertically()));
    connect(alignHorizontallyButton, SIGNAL(clicked()), nodeView, SLOT(alignSelectionHorizontally()));
    connect(showConnectionsButton, SIGNAL(clicked()), nodeView, SLOT(showConnectedNodes()));
    connect(showNewViewButton, SIGNAL(clicked()), this, SLOT(makeNewView()));
    connect(exportSnippetButton, SIGNAL(clicked()), this, SLOT(exportGraphMLSnippet()));
    connect(importSnippetButton, SIGNAL(clicked()), this, SLOT(importGraphMLSnippet()));
}


/**
 * @brief ToolbarWidget::updateToolButtons
 * Show/hide basic tool buttons for the currently selected nodeItem.
 */
void ToolbarWidget::updateToolButtons()
{
    // first, hide the buttons that are only for multilple selection
    foreach (QToolButton* button, multipleSelectionToolButtons) {
        button->hide();
    }

    // show/hide the delete button depending on the nodeKind
    QString nodeKind = nodeItem->getNodeKind();
    if (nodeKind.endsWith("Definitions") || nodeKind == "ManagementComponent") {
        deleteButton->hide();
    } else {
        deleteButton->show();
        alterModelToolButtons.append(deleteButton);
        showToolbar = true;
    }

    // check if the selected node item has other node items connected to it (edges)
    // NOTE: ComponentAssembly apparently has a connection to itself?
    if (nodeItem->getNode()->getEdges().count() > 0) {
        // DEMO CHANGE - Don't need this when the popup new window button is back
        //alterViewFrame->show();
        showConnectionsButton->show();
        showAlterViewFrame = true;
        showToolbar = true;
    } else {
        // DEMO CHANGE - Don't need this when the popup new window button is back
        //alterViewFrame->hide();
        showConnectionsButton->hide();
    }

    // always show show new view button
    //showNewViewButton->show();

    // DEMO CHANGE: hide this for now
    showNewViewButton->hide();
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
        setupFilesList(nodeView->getFiles(), "inst");
        setupChildrenComponentInstanceList(nodeItem->getNode()->getChildrenOfKind("ComponentInstance"));
    } else if (nodeItem->getNodeKind() == "BehaviourDefinitions") {
        setupFilesList(nodeView->getFiles(), "impl");
    }

    alterViewFrame->setVisible(showAlterViewFrame && alterModelToolButtons.count() > 0);
    showAlterViewFrame = false;
}


/**
 * @brief ToolbarWidget::multipleSelection
 * This is called when the user right-clicks on the view and there are
 * multiple items selected. It hides the majority of the tool buttons
 * and only displays the ones that can be used for multiple items.
 * @param items
 */
void ToolbarWidget::multipleSelection(QList<NodeItem*> items)
{
    // hide the buttons that are only for a single selection
    foreach (QToolButton* button, singleSelectionToolButtons) {
        button->hide();
    }

    // hide the frames used as separators
    snippetFrame->hide();
    goToFrame->hide();
    alterViewFrame->hide();

    // show the delete button - multiple selection can be deleted at the same time
    deleteButton->show();

    NodeItem* prevParentItem = 0;
    bool showButtons = true;

    // only show the group alignment buttons if all the selected items have the same parent
    foreach (NodeItem* item, items) {
        NodeItem* parentItem = item->getParentNodeItem();
        if (prevParentItem && (prevParentItem != parentItem)) {
            showButtons = false;
            break;
        }
        prevParentItem = parentItem;
    }

    foreach (QToolButton* button, multipleSelectionToolButtons) {
        button->setVisible(showButtons);
    }

    showToolbar = true;
}


/**
 * @brief ToolbarWidget::setupInstancesList
 * This sets up the menu list of instances of the selected node item.
 */
void ToolbarWidget::setupInstancesList(QList<Node*> instances)
{
    // if there are no instances, hide the instance button
    if (instances.count() == 0) {
        instancesButton->hide();
        return;
    } else {
        instancesButton->show();
        showGoToFrame = true;
        showToolbar = true;
    }

    updateSeparators(false);

    // create a ToolbarWidgetAction for each instance and connect it
    for (int i = 0; i < instances.count(); i++) {
        ToolbarWidgetAction* action  = new ToolbarWidgetAction(instances.at(i), this);
        connect(action, SIGNAL(toolbarAction_pressed()), this, SLOT(attachOptionMenu()));
        instancesMenu->addWidgetAction(action);
    }
}


/**
 * @brief ToolbarWidget::setupAdoptableNodesList
 * This sets up the menu list of adoptable node kinds for the selected node item.
 * @param nodeKinds
 */
void ToolbarWidget::setupAdoptableNodesList(QStringList nodeKinds)
{
    // if there are no adoptable node kinds, hide the addChild button
    if (nodeKinds.count() == 0) {
        addChildButton->hide();
        return;
    } else {
        addChildButton->show();
        alterModelToolButtons.append(addChildButton);
        showToolbar = true;
    }

    // populate addMenu
    for (int i = 0; i < nodeKinds.count(); i++) {

        QString nodeKind = nodeKinds.at(i);
        ToolbarWidgetAction* action;

        if (nodeKind == "ComponentImpl") {
            action = componentImplAction;
        } else if (nodeKind == "ComponentInstance") {
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
 * This sets up the menu list of nodes the selected node item can connect to.
 * @param nodeList
 */
void ToolbarWidget::setupLegalNodesList(QList<Node*> nodeList)
{
    // if the selected node can't connect to anything, hide the connect button
    if (nodeList.count() == 0) {
        connectButton->hide();
        return;
    } else {
        connectButton->show();
        alterModelToolButtons.append(connectButton);
        showToolbar = true;
    }

    // populate connectMenu
    for (int i = 0; i < nodeList.count(); i++) {

        Node* parentNode = nodeList.at(i)->getParentNode();

        // if the current node has a parent, check if there is already an action for it
        // if not, create one; if there is, add the node's action to the parent action's menu
        if (parentNode && parentNode->getDataValue("kind") != "Model") {

            ToolbarWidgetAction* parentAction = connectMenu->getWidgetAction(parentNode);
            ToolbarWidgetMenu* parentActionMenu = 0;

            if (parentAction) {
                parentActionMenu = parentAction->getMenu();
            } else {
                parentAction = new ToolbarWidgetAction(nodeList.at(i)->getParentNode(), connectMenu, "parent");
                parentActionMenu = new ToolbarWidgetMenu(parentAction, 0, connectMenu);
                connectMenu->addWidgetAction(parentAction);
            }

            ToolbarWidgetAction* action = new ToolbarWidgetAction(nodeList.at(i), parentActionMenu);
            parentActionMenu->addWidgetAction(action);
            connect(action, SIGNAL(triggered()), this, SLOT(connectNodes()));

        } else {
            qWarning() << "Toolbar: Current entity's parent entity is NULL.";
        }
    }
}


/**
 * @brief ToolbarWidget::setupFilesList
 * This sets up the Files menu list. It gets the updated list from the view.
 * @param files
 */
void ToolbarWidget::setupFilesList(QList<Node*> files, QString kind)
{
    // filter Files list - check which Files contain Components
    QList<Node*> fileWithComponents;
    foreach (Node* file, files) {
        if (file->getChildrenOfKind("Component").count() > 0) {
            fileWithComponents.append(file);
        }
    }

    ToolbarWidgetAction* action = 0;
    if (kind == "impl") {
        action = componentImplAction;
        fileMenu->setParentAction(componentImplAction);
    } else if (kind == "inst") {
        action = componentInstanceAction;
        fileMenu->setParentAction(componentInstanceAction);
    }

    // if the list has no contents, change componentInstanceAction checkable state
    if (fileWithComponents.count() == 0) {
        action->getButton()->setCheckable(false);
        return;
    } else {
        action->getButton()->setCheckable(true);
    }

    // populate fileMenu
    for (int i = 0; i < fileWithComponents.count(); i++) {
        ToolbarWidgetAction* fileAction = new ToolbarWidgetAction(fileWithComponents.at(i), fileMenu, "file");
        ToolbarWidgetMenu* fileActionMenu = new ToolbarWidgetMenu(fileAction, 0, fileMenu);
        fileMenu->addWidgetAction(fileAction);
    }

    setupComponentList(nodeView->getComponents(), kind);
}


/**
 * @brief ToolbarWidget::setupComponentList
 * This sets up the Components menu list. It gets the updated list from the view.
 * @param components
 */
void ToolbarWidget::setupComponentList(QList<Node*> components, QString kind)
{
    for (int i = 0; i < components.count(); i++) {

        // if selected node is the BehaviourDefinitions or an undefined ComponentImpl,
        // don't include already implemented Components in the menu
        if (kind == "impl") {
            if (components.at(i)->getImplementations().count() > 0) {
                continue;
            }
        }

        // add the Component to its parent File's menu
        foreach (QAction* action, fileMenu->actions()) {

            ToolbarWidgetAction* fileAction = qobject_cast<ToolbarWidgetAction*>(action);
            QString fileID = components.at(i)->getParentNode()->getID();

            // search for the Component's parent File action then add the Component's action to its menu
            if (fileID == fileAction->getNode()->getID()) {
                ToolbarWidgetAction* action = new ToolbarWidgetAction(components.at(i), fileMenu, "");
                connect(action, SIGNAL(triggered()), this, SLOT(addConnectedNode()));
                fileAction->getMenu()->addWidgetAction(action);
                break;
            }
        }
    }

    // if the fileAction's menu doesn't have any actions, remove it from the file menu
    foreach (QAction* action, fileMenu->actions()) {
        ToolbarWidgetAction* fileAction = qobject_cast<ToolbarWidgetAction*>(action);
        if (fileAction->getMenu() && fileAction->getMenu()->getWidgetActions().count() == 0) {
            fileMenu->removeWidgetAction(fileAction);
        }
    }
}


/**
 * @brief ToolbarWidget::setupChildrenComponentInstanceList
 * This sets up the menu list of children ComponentInstances inside the selected node item.
 * It gets the updated list from the view.
 * @param componentInstances
 */
void ToolbarWidget::setupChildrenComponentInstanceList(QList<Node*> componentInstances)
{
    // if there are children ComponentInstances, create an action and a menu for each one
    if (componentInstances.count() > 0) {

        foreach (Node* instance, componentInstances) {
            if (instance->getChildrenOfKind("InEventPortInstance").count() > 0) {
                ToolbarWidgetAction* inEvent_instanceAction = new ToolbarWidgetAction(instance, inEventPort_componentInstanceMenu, "eventPort");
                ToolbarWidgetMenu* inEventPortMenu = new ToolbarWidgetMenu(inEvent_instanceAction, 0, inEventPort_componentInstanceMenu);
                inEventPort_componentInstanceMenu->addWidgetAction(inEvent_instanceAction);
            }
            if (instance->getChildrenOfKind("OutEventPortInstance").count() > 0) {
                ToolbarWidgetAction* outEvent_instanceAction = new ToolbarWidgetAction(instance, outEventPort_componentInstanceMenu, "eventPort");
                ToolbarWidgetMenu* outEventPortMenu = new ToolbarWidgetMenu(outEvent_instanceAction, 0, outEventPort_componentInstanceMenu);
                outEventPort_componentInstanceMenu->addWidgetAction(outEvent_instanceAction);
            }
        }

        // setup menu lists for InEventPort/OutEventPort Delegates
        setupInEventPortInstanceList();
        setupOutEventPortInstanceList();
    }
}


/**
 * @brief ToolbarWidget::setupInEventPortInstanceList
 * This only gets called if there are children ComponentInstances inside the selected node item.
 * It sets up the list of InEventPortInstance from each all of the ComponentInstance children.
 */
void ToolbarWidget::setupInEventPortInstanceList()
{
    bool hasValidInEventPortInstance = false;

    foreach (ToolbarWidgetAction* instanceAction, inEventPort_componentInstanceMenu->getWidgetActions()) {

        QList<Node*> iep_instances = instanceAction->getNode()->getChildrenOfKind("InEventPortInstance");

        // for each InEventPortInstance, create an action and add it to parent ComponentInstance's menu
        foreach (Node* iep_inst, iep_instances) {
            // can only connect an EventPort Delegate to an EventPort connected to an Aggregate
            Node* defn = iep_inst->getDefinition();
            if (defn) {
                EventPort* ep = (EventPort*)defn;
                if (!ep->getAggregate()) {
                    continue;
                }
            }
            ToolbarWidgetAction* eventPortAction = new ToolbarWidgetAction(iep_inst, inEventPort_componentInstanceMenu, "");
            if (instanceAction->getMenu()) {
                instanceAction->getMenu()->addWidgetAction(eventPortAction);
                connect(eventPortAction, SIGNAL(triggered()), this, SLOT(addConnectedNode()));
            }
            hasValidInEventPortInstance = true;
        }
    }

    // if none of the ComponentInstances contain valid EventPortInstances
    // hide the ComponentInstanceMenu and show default action
    if (!hasValidInEventPortInstance) {
        inEventPort_componentInstanceMenu->clearMenu();
    }
}


/**
 * @brief ToolbarWidget::setupOutEventPortInstanceList
 * This only gets called if there are children ComponentInstances inside the selected node item.
 * It sets up the list of OutEventPortInstance from each all of the ComponentInstance children.
 */
void ToolbarWidget::setupOutEventPortInstanceList()
{
    bool hasValidOutEventPortInstance = false;

    foreach (ToolbarWidgetAction* instanceAction, outEventPort_componentInstanceMenu->getWidgetActions()) {

        QList<Node*> oep_instances = instanceAction->getNode()->getChildrenOfKind("OutEventPortInstance");

        // for each OutnEventPortInstance, create an action and add it to parent ComponentInstance's menu
        foreach (Node* oep_inst, oep_instances) {
            // can only connect an EventPort Delegate to an EventPort connected to an Aggregate
            Node* defn = oep_inst->getDefinition();
            if (defn) {
                EventPort* ep = (EventPort*)defn;
                if (!ep->getAggregate()) {
                    continue;
                }
            }
            ToolbarWidgetAction* eventPortAction = new ToolbarWidgetAction(oep_inst, outEventPort_componentInstanceMenu, "");
            if (instanceAction->getMenu()) {
                instanceAction->getMenu()->addWidgetAction(eventPortAction);
                connect(eventPortAction, SIGNAL(triggered()), this, SLOT(addConnectedNode()));
            }
            hasValidOutEventPortInstance = true;
        }
    }

    // if none of the ComponentInstances contain valid EventPortInstances
    // hide the ComponentInstanceMenu and show default action
    if (!hasValidOutEventPortInstance) {
        outEventPort_componentInstanceMenu->clearMenu();
    }
}


/**
 * @brief ToolbarWidget::clearMenus
 * Clear all of the stored menus
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
