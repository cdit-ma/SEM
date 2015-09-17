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

    eventFromToolbar = false;

    showDeleteToolButton = false;
    showImportSnippetToolButton = false;
    showExportSnippetToolButton = false;
    showDefinitionToolButton = false;
    showImplementationToolButton = false;

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
    resetButtonGroupFlags();
}


/**
 * @brief ToolbarWidget::updateToolbar
 * This only gets called when the toolbar is about to show.
 * Set nodeItem to the currectly selected item and update applicable tool buttons and menus.
 * If there are multiple items selected, only update the buttons that are for multiple selection.
 * @param nodeItems
 * @param edgeItems
 */
void ToolbarWidget::updateToolbar(QList<NodeItem*> nodeItems, QList<EdgeItem*> edgeItems)
{
    resetButtonGroupFlags();
    hideButtons();
    hideSeparators();

    // these buttons are enabled/disabled by NodeView whenever an item is selected/deselected
    deleteButton->setVisible(showDeleteToolButton);
    exportSnippetButton->setVisible(showExportSnippetToolButton);
    importSnippetButton->setVisible(showImportSnippetToolButton);

    //showToolbar = showDeleteToolButton || showExportSnippetToolButton || showImportSnippetToolButton;
    alterModelButtonsVisible = showDeleteToolButton;
    snippetButtonsVisible = showExportSnippetToolButton || showImportSnippetToolButton;

    // update the rest of the tool buttons and menus if only node items are selected
    if (edgeItems.isEmpty()) {
        updateButtonsAndMenus(nodeItems);
    }

    // update button group separators after the buttons and menus have been updated
    updateSeparators();
}


/**
 * @brief ToolbarWidget::enterEvent
 * This is called when the mouse is hovering over the toolbar.
 * @param event
 */
void ToolbarWidget::enterEvent(QEvent* event)
{
    eventFromToolbar = true;
    QWidget::enterEvent(event);
}


/**
 * @brief ToolbarWidget::leaveEvent
 * This is called when the mouse is no longer hovering over the toolbar.
 * @param event
 */
void ToolbarWidget::leaveEvent(QEvent* event)
{
    eventFromToolbar = false;
    QWidget::leaveEvent(event);
}


/**
 * @brief ToolbarWidget::updateActionEnabled
 * @param actionName
 * @param enabled
 */
void ToolbarWidget::updateActionEnabledState(QString actionName, bool enabled)
{
    if (actionName == "delete") {
        showDeleteToolButton = enabled;
    } else if (actionName == "definition") {
        showDefinitionToolButton = enabled;
    } else if (actionName == "implementation") {
        showImplementationToolButton = enabled;
    } else if (actionName == "exportSnippet") {
        showExportSnippetToolButton = enabled;
    } else if (actionName == "importSnippet") {
        showImportSnippetToolButton = enabled;
    }
}


/**
 * @brief ToolbarWidget::updateDisplayedChildren
 * This sends a signal to the view to display all/connected/unconnected children nodes of the selected hardware cluster(s).
 */
void ToolbarWidget::updateDisplayedChildren()
{
    QRadioButton* rb = qobject_cast<QRadioButton*>(QObject::sender());
    if (rb == allNodes) {
        nodeView->updateDisplayedChildrenNodes(0);
    } else if (rb == connectedNodes) {
        nodeView->updateDisplayedChildrenNodes(1);
    } else if (rb == unconnectedNodes) {
        nodeView->updateDisplayedChildrenNodes(2);
    }
}


/**
 * @brief ToolbarWidget::hardwareClusterMenuClicked
 * This slot is called everytime a HardwareCluster's menu is triggered.
 * It updates this toolbar's radio button set corresponding to the menu's actions.
 * This slot is also used to update the radio button set when there are multiple items selected.
 * @param viewMode
 */
void ToolbarWidget::hardwareClusterMenuClicked(int viewMode)
{
    switch (viewMode) {
    case 0:
        allNodes->setChecked(true);
        break;
    case 1:
        connectedNodes->setChecked(true);
        break;
    case 2:
        unconnectedNodes->setChecked(true);
        break;
    default:
        allNodes->setAutoExclusive(false);
        connectedNodes->setAutoExclusive(false);
        unconnectedNodes->setAutoExclusive(false);
        allNodes->setChecked(false);
        connectedNodes->setChecked(false);
        unconnectedNodes->setChecked(false);
        allNodes->setAutoExclusive(true);
        connectedNodes->setAutoExclusive(true);
        unconnectedNodes->setAutoExclusive(true);
        break;
    }
}


/**
 * @brief ToolbarWidget::addChildNode
 * Send a signal to the view to construct a new node with the specified kind.
 */
void ToolbarWidget::addChildNode()
{
    ToolbarWidgetAction* action = qobject_cast<ToolbarWidgetAction*>(QObject::sender());
    nodeView->constructNode(action->getActionKind(), 1);
}


/**
 * @brief ToolbarWidget::addConnectedNode
 * Send a signal to the view to construct a connected node.
 * It can either be a ComponentImpl, ComponentInstance or In/Out EventPortDelegates.
 */
void ToolbarWidget::addConnectedNode()
{
    ToolbarWidgetAction* action = qobject_cast<ToolbarWidgetAction*>(QObject::sender());
    ToolbarWidgetAction* topMostParentAction = action->getTopMostParentAction();

    // check which action was triggered & where it originated from to determine the nodeKind to construct
    if (topMostParentAction) {

        QString kindToConstruct;

        if (topMostParentAction == componentImplAction) {
            kindToConstruct = "ComponentImpl";
        } else if (topMostParentAction == componentInstanceAction) {
            kindToConstruct = "ComponentInstance";
        } else if (topMostParentAction == inEventPortDelegateAction) {
            kindToConstruct = "InEventPortDelegate";
        } else if (topMostParentAction == outEventPortDelegateAction) {
            kindToConstruct = "OutEventPortDelegate";
        } else if (topMostParentAction == blackBoxInstanceAction) {
            kindToConstruct = "BlackBoxInstance";
        } else {
            qWarning() << "ToolbarWidget::addConnectedNode - Action not handled.";
        }

        nodeView->constructConnectedNode(nodeItem->getID(), action->getNodeItemID(), kindToConstruct, 1);

    }
}


/**
 * @brief ToolbarWidget::connectNodes
 * Send a signal to the view to construct an edge between the selected node and chosen node.
 */
void ToolbarWidget::connectNodes()
{
    ToolbarWidgetAction* action = qobject_cast<ToolbarWidgetAction*>(QObject::sender());
    nodeView->constructDestructEdges(QStringList(), action->getNodeItemID());
}


/**
 * @brief ToolbarWidget::goToConnectedNode
 * This sends a signal to the view to center on the selected node item's definition, implementation or chosen instance.
 */
void ToolbarWidget::goToConnectedNode()
{
    QAction* action = qobject_cast<QAction*>(QObject::sender());
    if (action) {
        if (action->parentWidget() == definitionMenu) {
            nodeView->centerDefinition(nodeItem->getID());
        } else if (action->parentWidget() == implementationMenu) {
            nodeView->centerImplementation(nodeItem->getID());
        } else if (action->parentWidget() == instanceOptionMenu) {
            nodeView->centerInstance(instanceOptionMenu->getParentAction()->getNodeItemID());
        }
    }
}


/**
 * @brief ToolbarWidget::constructNewView
 * This pops up a new window. The node centered in the new window depends on the signal sender.
 */
void ToolbarWidget::constructNewView()
{
    // pop up the selected node in a new window
    QToolButton* button = qobject_cast<QToolButton*>(QObject::sender());
    if (button) {
        nodeView->constructNewView();
        return;
    }

    // pop up the selected node's definition/implementation/instance in a new window
    QAction* action = qobject_cast<QAction*>(QObject::sender());
    if (action) {
        if (action->parentWidget() == definitionMenu) {
            nodeView->constructNewView(1);
        } else if (action->parentWidget() == implementationMenu) {
            nodeView->constructNewView(2);
        } else if (action->parentWidget() == instanceOptionMenu) {
            nodeView->constructNewView(instanceOptionMenu->getParentAction()->getNodeItemID());
        }
    }
}


/**
 * @brief ToolbarWidget::attachOptionMenu
 * This attaches the view options menu to the chosen instance from the instance menu list.
 * This is called when an instance action is pressed. When that action is clicked, the menu is executed.
 */
void ToolbarWidget::attachOptionMenu()
{
    ToolbarWidgetAction* action = qobject_cast<ToolbarWidgetAction*>(QObject::sender());
    instanceOptionMenu->setParentAction(action);
}


/**
 * @brief ToolbarWidget::setVisible
 * @param visible
 */
void ToolbarWidget::setVisible(bool visible)
{
    bool toolbarVisible = visible && (alterModelButtonsVisible || alignButtonsVisible || snippetButtonsVisible
                                      || goToButtonsVisible || alterViewButtonsVisible);

    // update the toolbar & frame sizes
    if (toolbarVisible) {
        mainFrame->setFixedSize(toolbarLayout->sizeHint());
        shadowFrame->setFixedSize(toolbarLayout->sizeHint() + QSize(3,3));
        setFixedSize(shadowFrame->size());
    }

    mainFrame->setVisible(toolbarVisible);
    shadowFrame->setVisible(toolbarVisible);
    QWidget::setVisible(toolbarVisible);
}


/**
 * @brief ToolbarWidget::hide
 */
void ToolbarWidget::hide()
{
    setVisible(false);
}


/**
 * @brief ToolbarWidget::hideToolbar
 * This method checks if hiding the menu was triggered by the toolbar.
 * If the event came from outside the toolbar, hide the toolbar and all visible menus.
 * @param actionTriggered
 */
void ToolbarWidget::hideToolbar(bool actionTriggered)
{
    hide();
    if (!actionTriggered) {
        nodeView->toolbarClosed();
    }
}


/**
 * @brief ToolbarWidget::setupComponentList
 */
void ToolbarWidget::setupComponentList()
{
    ToolbarWidgetAction* action = qobject_cast<ToolbarWidgetAction*>(QObject::sender());
    if (action == componentImplAction) {
        setupComponentList("impl");
    } else if (action == componentInstanceAction) {
        setupComponentList("inst");
    } else {
        qWarning() << "ToolbarWidget::setupComponentList - Sender action not handled.";
    }
}


/**
 * @brief ToolbarWidget::setupBlackBoxList
 */
void ToolbarWidget::setupBlackBoxList()
{
    if (blackBoxMenuDone) {
        return;
    } else {
        blackBoxMenuDone = true;
    }

    // set up an action for each BlackBox definition
    QList<NodeItem*> blackBoxes = nodeView->getNodeItemsOfKind("BlackBox");
    for (int i = 0; i < blackBoxes.count(); i++) {
        ToolbarWidgetAction* action = constructSubMenuAction(blackBoxes.at(i), blackBoxDefinitionsMenu);
        if (action) {
            connect(action, SIGNAL(triggered()), this, SLOT(addConnectedNode()));
        } else {
            qWarning() << "ToolbarWidget::setupBlackBoxList - action not constructed.";
        }
    }
}


/**
 * @brief ToolbarWidget::setupEventPortInstanceList
 */
void ToolbarWidget::setupEventPortInstanceList()
{
    ToolbarWidgetAction* action = qobject_cast<ToolbarWidgetAction*>(QObject::sender());
    if (action == inEventPortDelegateAction) {
        setupEventPortInstanceList("InEventPortInstance");
    } else if (action == outEventPortDelegateAction) {
        setupEventPortInstanceList("OutEventPortInstance");
    } else {
        qWarning() << "ToolbarWidget::setupEventPortInstanceList - Sender action not handled.";
    }
}


/**
 * @brief ToolbarWidget::setupToolBar
 * Initialise and setup the layout and tool buttons and separators.
 */
void ToolbarWidget::setupToolBar()
{
    toolbarLayout = new QHBoxLayout();
    toolbarLayout->setMargin(5);
    toolbarLayout->setAlignment(Qt::AlignTop);
    setLayout(toolbarLayout);

    QSize buttonSize = QSize(39,39);

    // construct tool buttons and separators and add them to the toolbar's layout
    addChildButton = constructToolButton(buttonSize, "Plus", 0.8, "Add Child Entity");
    connectButton = constructToolButton(buttonSize, "ConnectTo", 0.7, "Connect Selection");
    deleteButton = constructToolButton(buttonSize, "Delete", 0.65, "Delete Selection");
    alignFrame = constructFrameSeparator();
    alignVerticallyButton = constructToolButton(buttonSize, "Align_Vertical", 0.6, "Align Selection Vertically");
    alignHorizontallyButton = constructToolButton(buttonSize, "Align_Horizontal", 0.6, "Align Selection Horizontally");
    snippetFrame = constructFrameSeparator();
    importSnippetButton = constructToolButton(buttonSize, "ImportSnippet", 0.6, "Import GraphML Snippet");
    exportSnippetButton = constructToolButton(buttonSize, "ExportSnippet", 0.6, "Export GraphML Snippet");
    goToFrame = constructFrameSeparator();
    definitionButton = constructToolButton(buttonSize, "Definition", 0.55, "View Definition");
    implementationButton = constructToolButton(buttonSize, "Implementation", 0.6, "View Implementation");
    instancesButton = constructToolButton(buttonSize, "Instance", 0.6, "View Instances");
    alterViewFrame = constructFrameSeparator();
    connectionsButton = constructToolButton(buttonSize, "Connections", 0.6, "View Connections");
    popupNewWindow = constructToolButton(buttonSize, "Popup", 0.55, "View In New Window");
    displayedChildrenOptionButton = constructToolButton(buttonSize, "MenuCluster", 0.7, "Change Displayed Nodes");

    deleteButton->setStyleSheet("padding-right: 3px;");
    setStyleSheet("QToolButton{"
                  "border: 1px solid;"
                  "border-color: rgba(160,160,160,250);"
                  "background-color: rgba(250,250,250,240);"
                  "}"
                  "QToolButton:hover{"
                  "border: 2px solid;"
                  "border-color: rgba(140,140,140,250);"
                  "background-color: rgba(255,255,255,255);"
                  "}"
                  //"QToolButton:pressed{"
                  //"background-color: rgba(240,240,240,240);"
                  //"}"
                  );
}


/**
 * @brief ToolbarWidget::setupMenus
 * Setup menus, sub-menus and their parent and default ToolbarWidgetActions.
 */
void ToolbarWidget::setupMenus()
{
    // construct main menus
    addMenu = constructToolMenu(addChildButton);
    connectMenu = constructToolMenu(connectButton);
    definitionMenu = constructToolMenu(definitionButton);
    implementationMenu = constructToolMenu(implementationButton);
    instancesMenu = constructToolMenu(instancesButton);
    instanceOptionMenu = new ToolbarWidgetMenu(0, 0, instancesMenu);
    displayedChildrenOptionMenu = constructToolMenu(displayedChildrenOptionButton);

    // setup widgets for the displayed children option menu for HardwareClusters
    allNodes = new QRadioButton("All", this);
    connectedNodes = new QRadioButton("Connected", this);
    unconnectedNodes = new QRadioButton("Unconnected", this);
    QWidgetAction* a1 = new QWidgetAction(this);
    QWidgetAction* a2 = new QWidgetAction(this);
    QWidgetAction* a3 = new QWidgetAction(this);
    a1->setDefaultWidget(allNodes);
    a2->setDefaultWidget(connectedNodes);
    a3->setDefaultWidget(unconnectedNodes);
    displayedChildrenOptionMenu->addAction(a1);
    displayedChildrenOptionMenu->addAction(a2);
    displayedChildrenOptionMenu->addAction(a3);

    // setup menu actions for the definition, implementation and instances menus
    QAction* goToDefn = definitionMenu->addAction(QIcon(":/Actions/Goto.png"), "Go to Definition");
    QAction* popupDefn = definitionMenu->addAction(QIcon(":/Actions/Popup.png"), "Popup Definition");
    connect(goToDefn, SIGNAL(triggered()), this, SLOT(goToConnectedNode()));
    connect(popupDefn, SIGNAL(triggered()), this, SLOT(constructNewView()));

    QAction* goToImpl = implementationMenu->addAction(QIcon(":/Actions/Goto.png"), "Go to Implementation");
    QAction* popupImpl = implementationMenu->addAction(QIcon(":/Actions/Popup.png"), "Popup Implementation");
    connect(goToImpl, SIGNAL(triggered()), this, SLOT(goToConnectedNode()));
    connect(popupImpl, SIGNAL(triggered()), this, SLOT(constructNewView()));

    QAction* goToInst = instanceOptionMenu->addAction(QIcon(":/Actions/Goto.png"), "Go to Instance");
    QAction* popupInst = instanceOptionMenu->addAction(QIcon(":/Actions/Popup.png"), "Popup Instance");
    connect(goToInst, SIGNAL(triggered()), this, SLOT(goToConnectedNode()));
    connect(popupInst, SIGNAL(triggered()), this, SLOT(constructNewView()));

    // these widget actions are not deletable - when their parent menu is cleared, they're only hidden
    componentImplAction = new ToolbarWidgetAction("ComponentImpl", "", addMenu);
    componentInstanceAction = new ToolbarWidgetAction("ComponentInstance", "", addMenu);
    inEventPortDelegateAction = new ToolbarWidgetAction("InEventPortDelegate", "", addMenu);
    outEventPortDelegateAction = new ToolbarWidgetAction("OutEventPortDelegate", "", addMenu);
    blackBoxInstanceAction = new ToolbarWidgetAction("BlackBoxInstance", "", addMenu);

    // default actions to display information for when visible menus are empty
    componentInstanceDefaultAction = new ToolbarWidgetAction("Info", "There are no IDL files containing Components that are valid for this action.", addMenu);
    blackBoxInstanceDefaultAction = new ToolbarWidgetAction("Info", "There are no IDL files containing BlackBoxes.", addMenu);
    iep_definitionInstanceDefaultAction = new ToolbarWidgetAction("Info", "This Assembly does not contain any InEventPortInstances that has a definition that is connected to an Aggregate.", addMenu);
    oep_definitionInstanceDefaultAction = new ToolbarWidgetAction("Info", "This Assembly does not contain any OutEventPortInstances that has a definition that is connected to an Aggregate.", addMenu);

    // hidden menus for ComponentInstances, ComponentImpls, In/Out EventPortDelegates and BlackBoxInstances
    componentDefinitionsMenu = new ToolbarWidgetMenu(0, componentInstanceDefaultAction, addMenu);
    blackBoxDefinitionsMenu = new ToolbarWidgetMenu(blackBoxInstanceAction, blackBoxInstanceDefaultAction, addMenu);
    iep_definitionInstanceMenu = new ToolbarWidgetMenu(inEventPortDelegateAction, iep_definitionInstanceDefaultAction, addMenu);
    oep_definitionInstanceMenu = new ToolbarWidgetMenu(outEventPortDelegateAction, oep_definitionInstanceDefaultAction, addMenu);
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
    connect(popupNewWindow, SIGNAL(clicked()), this, SLOT(hide()));
    connect(connectionsButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(exportSnippetButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(importSnippetButton, SIGNAL(clicked()), this, SLOT(hide()));

    connect(deleteButton, SIGNAL(clicked()), nodeView, SLOT(deleteSelection()));
    connect(alignVerticallyButton, SIGNAL(clicked()), nodeView, SLOT(alignSelectionVertically()));
    connect(alignHorizontallyButton, SIGNAL(clicked()), nodeView, SLOT(alignSelectionHorizontally()));
    connect(connectionsButton, SIGNAL(clicked()), nodeView, SLOT(showConnectedNodes()));
    connect(popupNewWindow, SIGNAL(clicked()), this, SLOT(constructNewView()));

    connect(allNodes, SIGNAL(clicked()), this, SLOT(updateDisplayedChildren()));
    connect(connectedNodes, SIGNAL(clicked()), this, SLOT(updateDisplayedChildren()));
    connect(unconnectedNodes, SIGNAL(clicked()), this, SLOT(updateDisplayedChildren()));

    connect(allNodes, SIGNAL(clicked()), displayedChildrenOptionMenu, SLOT(hide()));
    connect(connectedNodes, SIGNAL(clicked()), displayedChildrenOptionMenu, SLOT(hide()));
    connect(unconnectedNodes, SIGNAL(clicked()), displayedChildrenOptionMenu, SLOT(hide()));
    connect(displayedChildrenOptionMenu, SIGNAL(toolbarMenu_hideToolbar(bool)), this, SLOT(hideToolbar(bool)));

    connect(importSnippetButton, SIGNAL(clicked()), nodeView, SLOT(request_ImportSnippet()));
    connect(exportSnippetButton, SIGNAL(clicked()), nodeView, SLOT(exportSnippet()));

    connect(blackBoxInstanceAction, SIGNAL(toolbarAction_pressed()), SLOT(setupBlackBoxList()));
    connect(componentImplAction, SIGNAL(toolbarAction_pressed()), SLOT(setupComponentList()));
    connect(componentInstanceAction, SIGNAL(toolbarAction_pressed()), SLOT(setupComponentList()));
    connect(inEventPortDelegateAction, SIGNAL(toolbarAction_pressed()), SLOT(setupEventPortInstanceList()));
    connect(outEventPortDelegateAction, SIGNAL(toolbarAction_pressed()), SLOT(setupEventPortInstanceList()));
}


/**
 * @brief ToolbarWidget::updateButtonsAndMenus
 * Show/hide tool buttons for the currently selected nodeItem(s) and update the menu lists.
 * @param nodeItems
 */
void ToolbarWidget::updateButtonsAndMenus(QList<NodeItem*> nodeItems)
{
    if (nodeItems.isEmpty()) {
        return;
    }

    // need to clear menus before updating them
    clearMenus();

    QList<NodeItem*> legalNodes;

    if (nodeItems.count() == 1) {

        nodeItem = nodeItems.at(0);

        if (nodeItem) {

            // these buttons are only available for a single selection
            popupNewWindow->show();
            definitionButton->setVisible(showDefinitionToolButton);
            implementationButton->setVisible(showImplementationToolButton);
            goToButtonsVisible = showDefinitionToolButton || showImplementationToolButton;

            // check if the selected node item has other node items connected to it (edges)
            if (nodeItem->getNode()->getEdges(0).count() > 0) {
                connectionsButton->show();
            }

            // only show the displayed children option button if the selected item is a HardwareCluster
            if (nodeItem->getNodeKind() == "HardwareCluster") {
                displayedChildrenOptionButton->show();
            }

            // setup selected item's adoptable nodes menu and sub-menus, and its instances menu
            setupAdoptableNodesList(nodeView->getAdoptableNodeList(nodeItem->getID()));
            setupInstancesList(nodeView->getNodeInstances(nodeItem->getID()));

            legalNodes = nodeView->getConnectableNodeItems(nodeItem->getID());
            alterViewButtonsVisible = true;
        }

    } else {

        NodeItem* prevParentItem = 0;
        bool shareParent = true;
        bool allClusters = true;

        foreach (NodeItem* item, nodeItems) {
            NodeItem* parentItem = item->getParentNodeItem();
            if (prevParentItem && (prevParentItem != parentItem)) {
                shareParent = false;
            }
            if (item->getNodeKind() != "HardwareCluster") {
                allClusters = false;
            }
            prevParentItem = parentItem;
        }

        // only show the group alignment buttons if all the selected items have the same parent
        if (shareParent) {
            alignVerticallyButton->show();
            alignHorizontallyButton->show();
            alignButtonsVisible = true;
        }

        // if all selected node items are Hardware clusters, show diplay option button
        if (allClusters) {
            displayedChildrenOptionButton->show();
            alterViewButtonsVisible = true;
        }
    }

    // Added this so that you can change the hardware link using the context toolbar.
    bool deployable = true;
    foreach (NodeItem* item, nodeItems) {
        if (!nodeView->isNodeKindDeployable(item->getNodeKind())) {
            deployable = false;
            break;
        }
    }
    if (deployable) {
        QList<NodeItem*> hardware = nodeView->getHardwareList();
        foreach (NodeItem* item, hardware) {
            if (!legalNodes.contains(item)) {
                legalNodes.append(item);
            }
        }
    }

    // setup connectable nodes menu for the selected item(s)
    setupLegalNodesList(legalNodes);
}


/**
 * @brief ToolbarWidget::updateSeparators
 */
void ToolbarWidget::updateSeparators()
{
    // TODO: The separators still need fixing.
    alignFrame->setVisible(alignButtonsVisible && alterModelButtonsVisible);
    snippetFrame->setVisible(snippetButtonsVisible && (alterModelButtonsVisible || alignButtonsVisible));
    goToFrame->setVisible(goToButtonsVisible && (alterModelButtonsVisible || snippetButtonsVisible));
    alterViewFrame->setVisible(alterViewButtonsVisible && (alterModelButtonsVisible || alignButtonsVisible || snippetButtonsVisible || goToButtonsVisible));
}


/**
 * @brief ToolbarWidget::hideAllButtons
 */
void ToolbarWidget::hideButtons()
{
    addChildButton->hide();
    connectButton->hide();
    deleteButton->hide();
    alignVerticallyButton->hide();
    alignHorizontallyButton->hide();
    exportSnippetButton->hide();
    importSnippetButton->hide();
    definitionButton->hide();
    implementationButton->hide();
    instancesButton->hide();
    connectionsButton->hide();
    popupNewWindow->hide();
    displayedChildrenOptionButton->hide();
}


/**
 * @brief ToolbarWidget::hideSeparators
 */
void ToolbarWidget::hideSeparators()
{
    snippetFrame->hide();
    goToFrame->hide();
    alterViewFrame->hide();
}


/**
 * @brief ToolbarWidget::clearMenus
 * Clear all of the stored menus.
 */
void ToolbarWidget::clearMenus()
{
    addMenu->clearMenu();
    connectMenu->clearMenu();
    componentDefinitionsMenu->clearMenu();
    iep_definitionInstanceMenu->clearMenu();
    oep_definitionInstanceMenu->clearMenu();
    blackBoxDefinitionsMenu->clearMenu();
    instancesMenu->clearMenu();
}


/**
 * @brief ToolbarWidget::resetButtonGroupFlags
 */
void ToolbarWidget::resetButtonGroupFlags()
{
    alterModelButtonsVisible = false;
    alignButtonsVisible = false;
    snippetButtonsVisible = false;
    goToButtonsVisible = false;
    alterViewButtonsVisible = false;

    blackBoxMenuDone = false;
    componentImplMenuDone = false;
    componentInstMenuDone = false;
    inEventPortInstanceMenuDone = false;
    outEventPortInstanceMenuDone = false;
}


/**
 * @brief ToolbarWidget::constructToolButton
 * This constructs a QToolButton with the provided size, icon and tooltip.
 * It adds it to the toolbar's layout and then returns it.
 * @param size - button size
 * @param iconPng - button icon's png filename without the extension
 * @param iconSizeRatio - button icon's size ratio
 * @param tooltip - button tooltip
 * @return
 */
QToolButton* ToolbarWidget::constructToolButton(QSize size, QString iconPng, double iconSizeRatio, QString tooltip)
{
    QToolButton* button = new QToolButton(this);
    button->setFixedSize(size);
    button->setIcon(nodeView->getImage("Actions", iconPng));
    button->setIconSize(size * iconSizeRatio);
    button->setToolTip(tooltip);
    toolbarLayout->addWidget(button);
    return button;
}


/**
 * @brief ToolbarWidget::constructFrameSeparator
 * This constructs a QFrame used as a separator, adds it to the toolbar's layout and then returns it.
 * @return
 */
QFrame* ToolbarWidget::constructFrameSeparator()
{
    QFrame* frame = new QFrame(this);
    frame->setFrameShape(QFrame::VLine);
    frame->setPalette(QPalette(Qt::darkGray));
    toolbarLayout->addWidget(frame);
    return frame;
}


/**
 * @brief ToolbarWidget::constructToolMenu
 * This constructs and returns a ToolbarWidgetMenu that's directly attached to one of the QToolButtons.
 * @param parent - QToolButton the returned menu is attached to
 * @return
 */
ToolbarWidgetMenu* ToolbarWidget::constructToolMenu(QToolButton* parentButton)
{
    if (parentButton) {
        ToolbarWidgetMenu* menu = new ToolbarWidgetMenu(0, 0, parentButton);
        parentButton->setPopupMode(QToolButton::InstantPopup);
        parentButton->setMenu(menu);
        return menu;
    }
    return 0;
}


/**
 * @brief ToolbarWidget::constructSubMenuAction
 * This constructs an action for nodeItem and adds it to its parent action's menu if there is one.
 * If there isn't, it constructs the parent action and its menu and attaches it to parentMenu.
 * @param nodeItem - nodeItem attached to the action that's being constructed
 * @param parentMenu - top most parent menu that the newly constructed action should be attached to
 * @return
 */
ToolbarWidgetAction* ToolbarWidget::constructSubMenuAction(NodeItem* nodeItem, ToolbarWidgetMenu* parentMenu)
{
    if (!nodeItem || !parentMenu) {
        return 0;
    }

    NodeItem* parentNode = nodeItem->getParentNodeItem();

    // if nodeItem has a parent, check if there is already an action for it in parentMenu
    // if not, create one and attach a menu to it
    if (parentNode && parentNode->getNodeKind() != "Model") {

        ToolbarWidgetAction* parentAction = parentMenu->getWidgetAction(parentNode);
        ToolbarWidgetMenu* parentActionMenu = 0;

        if (parentAction) {
            parentActionMenu = parentAction->getMenu();
        } else {
            parentAction = new ToolbarWidgetAction(parentNode, parentMenu, true);
            parentActionMenu = new ToolbarWidgetMenu(parentAction, 0, parentMenu);
            parentMenu->addWidgetAction(parentAction);
        }

        // construct and add nodeItem's action to the parent action's menu
        if (parentActionMenu) {
            ToolbarWidgetAction* action = new ToolbarWidgetAction(nodeItem, parentActionMenu);
            parentActionMenu->addWidgetAction(action);
            return action;
        }
    }

    return 0;
}


/**
 * @brief ToolbarWidget::setupAdoptableNodesList
 * This sets up the menu list of adoptable node kinds for the selected node item.
 * @param nodeKinds
 */
void ToolbarWidget::setupAdoptableNodesList(QStringList nodeKinds)
{
    // if there are no adoptable node kinds, hide the addChild button
    if (nodeKinds.isEmpty()) {
        addChildButton->hide();
        return;
    } else {
        addChildButton->show();
        alterModelButtonsVisible = true;
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
        } else if (nodeKind == "BlackBoxInstance") {
            action = blackBoxInstanceAction;
        } else {
            action  = new ToolbarWidgetAction(nodeKind, "", addMenu);
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
void ToolbarWidget::setupLegalNodesList(QList<NodeItem*> nodeList)
{
    // if the selected node can't connect to anything, hide the connect button
    if (nodeList.isEmpty()) {
        connectButton->hide();
        return;
    } else {
        connectButton->show();
        alterModelButtonsVisible = true;
    }

    // populate connectMenu
    for (int i = 0; i < nodeList.count(); i++) {
        ToolbarWidgetAction* action = constructSubMenuAction(nodeList.at(i), connectMenu);
        if (action) {
            connect(action, SIGNAL(triggered()), this, SLOT(connectNodes()));
        } else {
            qWarning() << "ToolbarWidget::setupLegalNodesList - action not constructed.";
        }
    }
}


/**
 * @brief ToolbarWidget::setupInstancesList
 * This sets up the menu list of instances of the selected node item.
 */
void ToolbarWidget::setupInstancesList(QList<NodeItem *> instances)
{
    // if there are no instances, hide the instance button
    if (instances.isEmpty()) {
        instancesButton->hide();
        return;
    } else {
        instancesButton->show();
        goToButtonsVisible = true;
    }

    // create a ToolbarWidgetAction for each instance and connect it
    for (int i = 0; i < instances.count(); i++) {
        ToolbarWidgetAction* action  = new ToolbarWidgetAction(instances.at(i), instancesMenu);
        connect(action, SIGNAL(toolbarAction_pressed()), this, SLOT(attachOptionMenu()));
        instancesMenu->addWidgetAction(action);
    }
}


/**
 * @brief ToolbarWidget::setupComponentList
 * This sets up the Components menu list. It gets the updated list from the view.
 * @param components
 */
void ToolbarWidget::setupComponentList(QString actionKind)
{
    if (actionKind == "impl") {
        if (componentImplMenuDone) {
            return;
        } else {
            componentImplMenuDone = true;
            componentDefinitionsMenu->setParentAction(componentImplAction);
        }
    } else if (actionKind == "inst") {
        if (componentInstMenuDone) {
            return;
        } else {
            componentInstMenuDone = true;
            componentDefinitionsMenu->setParentAction(componentInstanceAction);
        }
    } else {
        qWarning() << "ToolbarWidget::setupFilesList - Action kind not handled.";
        return;
    }

    QList<NodeItem*> components = nodeView->getNodeItemsOfKind("Component");

    for (int i = 0; i < components.count(); i++) {

        // if selected node is the BehaviourDefinitions or an undefined ComponentImpl,
        // don't include already implemented Components in the menu
        if (actionKind == "impl" && nodeView->getImplementation(components.at(i)->getID())) {
            continue;
        }

        // set up an action for each Component definition
        ToolbarWidgetAction* action = constructSubMenuAction(components.at(i), componentDefinitionsMenu);
        if (action) {
            connect(action, SIGNAL(triggered()), this, SLOT(addConnectedNode()));
        } else {
            qWarning() << "ToolbarWidget::setupComponentList - Action not constructed.";
        }
    }
}


/**
 * @brief ToolbarWidget::setupEventPortInstanceList
 * @param eventPortKind
 */
void ToolbarWidget::setupEventPortInstanceList(QString eventPortKind)
{
    ToolbarWidgetMenu* instanceMenu = 0;

    if (eventPortKind == "InEventPortInstance") {
        if (inEventPortInstanceMenuDone) {
            return;
        } else {
            inEventPortInstanceMenuDone = true;
            instanceMenu = iep_definitionInstanceMenu;
        }
    } else if (eventPortKind == "OutEventPortInstance") {
        if (outEventPortInstanceMenuDone) {
            return;
        } else {
            outEventPortInstanceMenuDone = true;
            instanceMenu = oep_definitionInstanceMenu;
        }
    } else {
        qWarning() << "ToolbarWidget::setupEventPortInstanceList - Event port kind not handled.";
        return;
    }

    // setup combined list of both Component and BlackBox instances for the In/Out EventPortDelegate menus
    QList<NodeItem*> definitionInstances = nodeView->getNodeItemsOfKind("ComponentInstance", nodeItem->getID(), 0);
    definitionInstances.append(nodeView->getNodeItemsOfKind("BlackBoxInstance", nodeItem->getID(), 0));

    foreach (NodeItem* defnInst, definitionInstances) {

        QList<NodeItem*> epInstances = nodeView->getNodeItemsOfKind(eventPortKind, defnInst->getID());

        foreach (NodeItem* epInst, epInstances) {

            // can only connect an EventPort Delegate to an EventPort connected to an Aggregate
            NodeItem* epDefn = nodeView->getDefinition(epInst->getID());
            if (epDefn && !nodeView->getAggregate(epDefn->getID())) {
                continue;
            }

            ToolbarWidgetAction* epInstAction = constructSubMenuAction(epInst, instanceMenu);
            if (epInstAction) {
                connect(epInstAction, SIGNAL(triggered()), this, SLOT(addConnectedNode()));
            } else {
                qWarning() << "ToolbarWidget::setupEventPortInstanceList - Action not constructed.";
            }
        }
    }
}

