#include "toolbarwidget.h"
#include "toolbarmenu.h"

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

    showDeleteToolButton = false;
    showImportSnippetToolButton = false;
    showExportSnippetToolButton = false;
    showDefinitionToolButton = false;
    showImplementationToolButton = false;

    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::Popup);

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
        nodeView->hardwareClusterMenuClicked(0);
    } else if (rb == connectedNodes) {
        nodeView->hardwareClusterMenuClicked(1);
    } else if (rb == unconnectedNodes) {
        nodeView->hardwareClusterMenuClicked(2);
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
 * @param action
 */
void ToolbarWidget::addChildNode(ToolbarMenuAction* action)
{
    nodeView->constructNode(action->getActionKind(), 1);
}


/**
 * @brief ToolbarWidget::addConnectedNode
 * Send a signal to the view to construct a connected node.
 * It can either be a ComponentImpl, ComponentInstance, BlackBoxInstance or In/Out EventPortDelegates.
 * @param action
 */
void ToolbarWidget::addConnectedNode(ToolbarMenuAction* action)
{
    ToolbarMenu* menu = qobject_cast<ToolbarMenu*>(QObject::sender());
    QString kindToConstruct;

    if (menu == componentImplDefinitionsMenu) {
        kindToConstruct = "ComponentImpl";
    } else if (menu == componentInstDefinitionsMenu) {
        kindToConstruct = "ComponentInstance";
    } else if (menu == iep_definitionInstanceMenu) {
        kindToConstruct = "InEventPortDelegate";
    } else if (menu == oep_definitionInstanceMenu) {
        kindToConstruct = "OutEventPortDelegate";
    } else if (menu == blackBoxDefinitionsMenu) {
        kindToConstruct = "BlackBoxInstance";
    } else {
        qWarning() << "ToolbarWidget::addConnectedNode - Action not handled.";
        return;
    }

    nodeView->constructConnectedNode(nodeItem->getID(), action->getNodeItemID(), kindToConstruct, 1);
}


/**
 * @brief ToolbarWidget::connectNodes
 * Send a signal to the view to construct an edge between the selected node and chosen node.
 */
void ToolbarWidget::connectNodes(ToolbarMenuAction* action)
{
    nodeView->constructDestructEdges(QStringList(), action->getNodeItemID());
}


/**
 * @brief ToolbarWidget::displayConnectedNode
 * This sends a signal to the view to either center on or pop up a new window centered on
 * the selected node item's definition, implementation or chosen instance.
 * @param action
 */
void ToolbarWidget::displayConnectedNode(ToolbarMenuAction* action)
{
    // if there is no action, the sender is a QToolButton
    if (!action) {
        QToolButton* button = qobject_cast<QToolButton*>(QObject::sender());
        if (button == definitionButton) {
            nodeView->centerDefinition(nodeItem->getID());
        } else if (button == implementationButton) {
            nodeView->centerImplementation(nodeItem->getID());
        }
        return;
    }

    QString actionKind = action->getActionKind();
    QWidget* parentWidget = action->parentWidget();

    if (actionKind == "Goto") {
        if (parentWidget == definitionMenu) {
            nodeView->centerDefinition(nodeItem->getID());
        } else if (parentWidget == implementationMenu) {
            nodeView->centerImplementation(nodeItem->getID());
        } else if (parentWidget == instancesMenu) {
            nodeView->centerInstance(chosenInstanceID);
        }
    } else if (actionKind == "Popup") {
        if (parentWidget == definitionMenu) {
            nodeView->constructNewView(1);
        } else if (parentWidget == implementationMenu) {
            nodeView->constructNewView(2);
        } else if (parentWidget == instancesMenu) {
            nodeView->constructNewView(chosenInstanceID);
        }
    } else {
        qWarning() << "ToolbarWidget::displayConnectedNode -  Action kind not handled.";
    }
}


/**
 * @brief ToolbarWidget::constructNewView
 * This sends a signal to the view to pop up a new window centered on the selected node.
 */
void ToolbarWidget::constructNewView()
{
    nodeView->constructNewView();
}


/**
 * @brief ToolbarWidget::hideToolbar
 * This slot is called when either one of the opened menus are triggered or the mouse was pressed outside of them.
 * If the menu was not triggered and the mouse press happened inside the toolbar, don't hide the toolbar.
 * @param action
 */
void ToolbarWidget::hideToolbar(QAction* action)
{
    if (!action) {
        QPoint cursorPos = mapFromGlobal(QCursor::pos());
        if (rect().contains(cursorPos)) {
            return;
        }
    }
    hide();
}


/**
 * @brief ToolbarWidget::hide
 */
void ToolbarWidget::hide()
{
    setVisible(false);
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
        //shadowFrame->setFixedSize(toolbarLayout->sizeHint() + QSize(6,6));
        //mainFrame->move(3,3);
        setFixedSize(shadowFrame->size());
    } else {
        closeOpenMenus();
    }

    mainFrame->setVisible(toolbarVisible);
    shadowFrame->setVisible(toolbarVisible);
    QWidget::setVisible(toolbarVisible);
}


/**
 * @brief ToolbarWidget::appendToOpenedMenusList
 */
void ToolbarWidget::appendToOpenMenusList()
{
    ToolbarMenu* menu = qobject_cast<ToolbarMenu*>(QObject::sender());
    openMenus.append(menu);
}


/**
 * @brief ToolbarWidget::removeFromOpenedMenusList
 */
void ToolbarWidget::removeFromOpenMenusList()
{
    ToolbarMenu* menu = qobject_cast<ToolbarMenu*>(QObject::sender());
    openMenus.removeAll(menu);

    QPoint cursorPos = mapFromGlobal(QCursor::pos());
    if (rect().contains(cursorPos)) {
        closeOpenMenus();
    }
}


/**
 * @brief ToolbarWidget::setInstanceID
 * This slot is called when an action from the instancesMenu is hovered over.
 * It updates chosenInstanceID which is used for displaying the selected node's chosen instance.
 */
void ToolbarWidget::setInstanceID()
{
    ToolbarMenuAction* action = qobject_cast<ToolbarMenuAction*>(QObject::sender());
    chosenInstanceID = action->getNodeItemID();
}


/**
 * @brief ToolbarWidget::setConnectModeOn
 */
void ToolbarWidget::setConnectModeOn()
{
    nodeView->setConnectModeFromToolbar(true, legalNodeItems);
}


/**
 * @brief ToolbarWidget::setupLegalNodesList
 */
void ToolbarWidget::setupLegalNodesList()
{
    if (connectMenuDone) {
        return;
    } else {
        connectMenuDone = true;
    }

    foreach (NodeItem* item, legalNodeItems) {
        ToolbarMenuAction* action = constructSubMenuAction(item, connectMenu);
        if (!action) {
            qWarning() << "ToolbarWidget::setupLegalNodesList - Action not constructed.";
        }
    }
}


/**
 * @brief ToolbarWidget::setupComponentList
 */
void ToolbarWidget::setupComponentList()
{
    ToolbarMenuAction* action = qobject_cast<ToolbarMenuAction*>(QObject::sender());
    if (action == componentImplAction) {
        setupComponentList("impl");
    } else if (action == componentInstAction) {
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

    QList<NodeItem*> blackBoxes = nodeView->getNodeItemsOfKind("BlackBox");
    foreach (NodeItem* blackBox, blackBoxes) {
        ToolbarMenuAction* action = constructSubMenuAction(blackBox, blackBoxDefinitionsMenu);
        if (!action) {
            qWarning() << "ToolbarWidget::setupBlackBoxList - Action not constructed.";
        }
    }
}


/**
 * @brief ToolbarWidget::setupEventPortInstanceList
 */
void ToolbarWidget::setupEventPortInstanceList()
{
    ToolbarMenuAction* action = qobject_cast<ToolbarMenuAction*>(QObject::sender());
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
    setStyleSheet("QToolButton {"
                  "border: 1px solid;"
                  "border-color: rgba(160,160,160,250);"
                  "background-color: rgba(250,250,250,240);"
                  "}"
                  "QToolButton:hover {"
                  "border: 1.5px solid;"
                  "border-color: rgba(170,170,170,250);"
                  "background-color: rgba(255,255,255,255);"
                  "}"
                  "QToolButton[popupMode=\"1\"] {"
                  "padding-right: 15px;"
                  "}"
                  "QToolButton::menu-button {"
                  "border-left: 1px solid gray;"
                  "border-top-right-radius: 10px;"
                  "border-bottom-right-radius: 10px;"
                  "width: 15px;"
                  "}"
                  "QRadioButton {"
                  "padding: 8px 10px 8px 8px;"
                  "}"
                  "QRadioButton::checked {"
                  "color: green;"
                  "}"
                  );

    /*
     * This is what makes the tool buttons with a split menu button look good on a Mac
     *
    "QToolButton[popupMode=\"1\"] {"
    "padding-right: 12px;"
    "}"
    "QToolButton::menu-button {"
    "border-left: 1px solid gray;"
    "border-top-right-radius: 10px;"
    "border-bottom-right-radius: 10px;"
    "padding-right: 4px;"
    "width: 10px;"
    "}"
    */
}


/**
 * @brief ToolbarWidget::setupMenus
 * Setup menus, sub-menus and their parent and default ToolbarWidgetActions.
 */
void ToolbarWidget::setupMenus()
{
    // construct main menus
    addMenu = constructToolButtonMenu(addChildButton);
    connectMenu = constructToolButtonMenu(connectButton, false);
    definitionMenu = constructToolButtonMenu(definitionButton, false);
    implementationMenu = constructToolButtonMenu(implementationButton, false);
    instancesMenu = constructToolButtonMenu(instancesButton);
    displayedChildrenOptionMenu = constructToolButtonMenu(displayedChildrenOptionButton);

    // setup menu actions for the definition and implementation menus
    definitionMenu->addAction(new ToolbarMenuAction("Goto", definitionMenu, "Go to Definition", ":/Actions/"));
    definitionMenu->addAction(new ToolbarMenuAction("Popup", definitionMenu, "Popup Definition", ":/Actions/"));
    implementationMenu->addAction(new ToolbarMenuAction("Goto", implementationMenu, "Go to Implementation", ":/Actions/"));
    implementationMenu->addAction(new ToolbarMenuAction("Popup", implementationMenu, "Popup Implementation", ":/Actions/"));

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
    displayedChildrenOptionMenu->QMenu::addAction(a1);
    displayedChildrenOptionMenu->QMenu::addAction(a2);
    displayedChildrenOptionMenu->QMenu::addAction(a3);

    // these actions are not deletable - when their parent menu is cleared, they're only hidden
    componentImplAction = new ToolbarMenuAction("ComponentImpl", this);
    componentInstAction = new ToolbarMenuAction("ComponentInstance", this);
    inEventPortDelegateAction = new ToolbarMenuAction("InEventPortDelegate", this);
    outEventPortDelegateAction = new ToolbarMenuAction("OutEventPortDelegate", this);
    blackBoxInstanceAction = new ToolbarMenuAction("BlackBoxInstance", this);

    // default actions to display information for when their parent menus are empty
    componentImplMenuInfoAction = new ToolbarMenuAction("Info", this, "There are no IDL files containing unimplemented Components.", ":/Actions/");
    componentInstMenuInfoAction = new ToolbarMenuAction("Info", this, "There are no IDL files containing Components.", ":/Actions/");
    blackBoxMenuInfoAction = new ToolbarMenuAction("Info", this, "There are no IDL files containing BlackBoxes.", ":/Actions/");
    iep_menuInfoAction = new ToolbarMenuAction("Info", this, "This Assembly does not contain any InEventPortInstances that has a definition that is connected to an Aggregate.", ":/Actions/");
    oep_menuInfoAction = new ToolbarMenuAction("Info", this, "This Assembly does not contain any OutEventPortInstances that has a definition that is connected to an Aggregate.", ":/Actions/");

    // hidden menus for ComponentInstances, ComponentImpls, In/Out EventPortDelegates and BlackBoxInstances
    componentImplDefinitionsMenu = new ToolbarMenu(this, componentImplMenuInfoAction);
    componentInstDefinitionsMenu = new ToolbarMenu(this, componentInstMenuInfoAction);
    blackBoxDefinitionsMenu = new ToolbarMenu(this, blackBoxMenuInfoAction);
    iep_definitionInstanceMenu = new ToolbarMenu(this, iep_menuInfoAction);
    oep_definitionInstanceMenu = new ToolbarMenu(this, oep_menuInfoAction);

    componentImplAction->setMenu(componentImplDefinitionsMenu);
    componentInstAction->setMenu(componentInstDefinitionsMenu);
    inEventPortDelegateAction->setMenu(iep_definitionInstanceMenu);
    outEventPortDelegateAction->setMenu(oep_definitionInstanceMenu);
    blackBoxInstanceAction->setMenu(blackBoxDefinitionsMenu);
}


/**
 * @brief ToolbarWidget::makeConnections
 * Connect signals and slots.
 */
void ToolbarWidget::makeConnections()
{
    connect(connectButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(deleteButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(alignVerticallyButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(alignHorizontallyButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(popupNewWindow, SIGNAL(clicked()), this, SLOT(hide()));
    connect(connectionsButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(exportSnippetButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(importSnippetButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(definitionButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(implementationButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(allNodes, SIGNAL(clicked()), displayedChildrenOptionMenu, SLOT(hide()));
    connect(connectedNodes, SIGNAL(clicked()), displayedChildrenOptionMenu, SLOT(hide()));
    connect(unconnectedNodes, SIGNAL(clicked()), displayedChildrenOptionMenu, SLOT(hide()));

    connect(connectButton, SIGNAL(clicked()), this, SLOT(setConnectModeOn()));
    connect(deleteButton, SIGNAL(clicked()), nodeView, SLOT(deleteSelection()));
    connect(alignVerticallyButton, SIGNAL(clicked()), nodeView, SLOT(alignSelectionVertically()));
    connect(alignHorizontallyButton, SIGNAL(clicked()), nodeView, SLOT(alignSelectionHorizontally()));
    connect(popupNewWindow, SIGNAL(clicked()), this, SLOT(constructNewView()));
    connect(connectionsButton, SIGNAL(clicked()), nodeView, SLOT(showConnectedNodes()));
    connect(exportSnippetButton, SIGNAL(clicked()), nodeView, SLOT(exportSnippet()));
    connect(importSnippetButton, SIGNAL(clicked()), nodeView, SLOT(request_ImportSnippet()));
    connect(definitionButton, SIGNAL(clicked()), this, SLOT(displayConnectedNode()));
    connect(implementationButton, SIGNAL(clicked()), this, SLOT(displayConnectedNode()));
    connect(allNodes, SIGNAL(clicked()), this, SLOT(updateDisplayedChildren()));
    connect(connectedNodes, SIGNAL(clicked()), this, SLOT(updateDisplayedChildren()));
    connect(unconnectedNodes, SIGNAL(clicked()), this, SLOT(updateDisplayedChildren()));

    connect(addMenu, SIGNAL(toolbarMenu_triggered(ToolbarMenuAction*)), this, SLOT(addChildNode(ToolbarMenuAction*)));

    connect(connectMenu, SIGNAL(aboutToShow()), this, SLOT(setupLegalNodesList()));
    connect(connectMenu, SIGNAL(toolbarMenu_triggered(ToolbarMenuAction*)), this, SLOT(connectNodes(ToolbarMenuAction*)));

    connect(definitionMenu, SIGNAL(toolbarMenu_triggered(ToolbarMenuAction*)), this, SLOT(displayConnectedNode(ToolbarMenuAction*)));
    connect(implementationMenu, SIGNAL(toolbarMenu_triggered(ToolbarMenuAction*)), this, SLOT(displayConnectedNode(ToolbarMenuAction*)));
    connect(instancesMenu, SIGNAL(toolbarMenu_triggered(ToolbarMenuAction*)), this, SLOT(displayConnectedNode(ToolbarMenuAction*)));

    connect(componentImplAction, SIGNAL(hovered()), this, SLOT(setupComponentList()));
    connect(componentInstAction, SIGNAL(hovered()), this, SLOT(setupComponentList()));
    connect(inEventPortDelegateAction, SIGNAL(hovered()), this, SLOT(setupEventPortInstanceList()));
    connect(outEventPortDelegateAction, SIGNAL(hovered()), this, SLOT(setupEventPortInstanceList()));
    connect(blackBoxInstanceAction, SIGNAL(hovered()), this, SLOT(setupBlackBoxList()));

    connect(componentImplDefinitionsMenu, SIGNAL(toolbarMenu_triggered(ToolbarMenuAction*)), this, SLOT(addConnectedNode(ToolbarMenuAction*)));
    connect(componentInstDefinitionsMenu, SIGNAL(toolbarMenu_triggered(ToolbarMenuAction*)), this, SLOT(addConnectedNode(ToolbarMenuAction*)));
    connect(iep_definitionInstanceMenu, SIGNAL(toolbarMenu_triggered(ToolbarMenuAction*)), this, SLOT(addConnectedNode(ToolbarMenuAction*)));
    connect(oep_definitionInstanceMenu, SIGNAL(toolbarMenu_triggered(ToolbarMenuAction*)), this, SLOT(addConnectedNode(ToolbarMenuAction*)));
    connect(blackBoxDefinitionsMenu, SIGNAL(toolbarMenu_triggered(ToolbarMenuAction*)), this, SLOT(addConnectedNode(ToolbarMenuAction*)));
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

    QList<NodeItem*> legalNodes;

    // need to clear menus before updating them
    clearMenus();

    if (nodeItems.count() == 1) {

        nodeItem = nodeItems.at(0);

        if (!nodeItem) {
            return;
        }

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
            hardwareClusterMenuClicked(nodeItem->getHardwareClusterChildrenViewMode());
            displayedChildrenOptionButton->show();
        }

        // setup selected item's adoptable nodes menu and sub-menus, and its instances menu
        setupAdoptableNodesList(nodeView->getAdoptableNodeList(nodeItem->getID()));
        setupInstancesList(nodeView->getNodeInstances(nodeItem->getID()));

        legalNodes = nodeView->getConnectableNodeItems(nodeItem->getID());
        alterViewButtonsVisible = true;

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

    // added this so that you can change the hardware link using the context toolbar.
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
    instancesMenu->clearMenu();
    componentImplDefinitionsMenu->clearMenu();
    componentInstDefinitionsMenu->clearMenu();
    iep_definitionInstanceMenu->clearMenu();
    oep_definitionInstanceMenu->clearMenu();
    blackBoxDefinitionsMenu->clearMenu();

    // this list needs to be cleared as well - used to populate the connect menu
    legalNodeItems.clear();
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

    connectMenuDone = false;
    chosenInstanceID = "";
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

    foreach (QString kind, nodeKinds) {

        ToolbarMenuAction* action;

        if (kind == "ComponentImpl") {
            action = componentImplAction;
        } else if (kind == "ComponentInstance") {
            action = componentInstAction;
        } else if (kind == "InEventPortDelegate") {
            action = inEventPortDelegateAction;
        } else if (kind == "OutEventPortDelegate") {
            action = outEventPortDelegateAction;
        } else if (kind == "BlackBoxInstance") {
            action = blackBoxInstanceAction;
        } else {
            action  = new ToolbarMenuAction(kind, this);
        }

        addMenu->addAction(action);
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
        legalNodeItems = nodeList;
    }
}


/**
 * @brief ToolbarWidget::setupInstancesList
 * This sets up the menu list of instances of the selected node item.
 */
void ToolbarWidget::setupInstancesList(QList<NodeItem*> instances)
{
    // if there are no instances, hide the instance button
    if (instances.isEmpty()) {
        instancesButton->hide();
        return;
    } else {
        instancesButton->show();
        goToButtonsVisible = true;
    }

    foreach (NodeItem* instance, instances) {
        // construct a display option menu
        ToolbarMenu* menu = new ToolbarMenu(this);
        menu->addAction(new ToolbarMenuAction("Goto", instancesMenu, "Go to Instance", ":/Actions/"));
        menu->addAction(new ToolbarMenuAction("Popup", instancesMenu, "Popup Instance", ":/Actions/"));

        // setup an action for each of the selected entity's instances and attach the display option menu
        ToolbarMenuAction* action = new ToolbarMenuAction(instance, this);
        connect(action, SIGNAL(hovered()), this, SLOT(setInstanceID()));
        action->setMenu(menu);
        instancesMenu->addAction(action);
    }
}


/**
 * @brief ToolbarWidget::setupComponentList
 * @param kind
 */
void ToolbarWidget::setupComponentList(QString actionKind)
{
    ToolbarMenu* actionMenu = 0;

    if (actionKind == "impl") {
        if (componentImplMenuDone) {
            return;
        } else {
            componentImplMenuDone = true;
            actionMenu = componentImplDefinitionsMenu;
        }
    } else if (actionKind == "inst") {
        if (componentInstMenuDone) {
            return;
        } else {
            componentInstMenuDone = true;
            actionMenu = componentInstDefinitionsMenu;
        }
    } else {
        qWarning() << "ToolbarWidget::setupCompList - Action kind not handled.";
        return;
    }

    QList<NodeItem*> components = nodeView->getNodeItemsOfKind("Component");

    foreach (NodeItem* component, components) {

        // if selected node is the BehaviourDefinitions or an undefined ComponentImpl,
        // don't include already implemented Components in the menu
        if (actionKind == "impl" && nodeView->getImplementation(component->getID())) {
            continue;
        }

        // set up an action for each Component definition
        ToolbarMenuAction* action = constructSubMenuAction(component, actionMenu);
        if (!action) {
            qWarning() << "ToolbarWidget::setupCompList - Action not constructed.";
        }
    }
}


/**
 * @brief ToolbarWidget::setupEventPortInstanceList
 * @param eventPortKind
 */
void ToolbarWidget::setupEventPortInstanceList(QString eventPortKind)
{
    ToolbarMenu* actionMenu = 0;

    if (eventPortKind == "InEventPortInstance") {
        if (inEventPortInstanceMenuDone) {
            return;
        } else {
            inEventPortInstanceMenuDone = true;
            actionMenu = iep_definitionInstanceMenu;
        }
    } else if (eventPortKind == "OutEventPortInstance") {
        if (outEventPortInstanceMenuDone) {
            return;
        } else {
            outEventPortInstanceMenuDone = true;
            actionMenu = oep_definitionInstanceMenu;
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

            // set up an action for each In/Out EventPortInstance
            ToolbarMenuAction* epInstAction = constructSubMenuAction(epInst, actionMenu);
            if (!epInstAction) {
                qWarning() << "ToolbarWidget::setupEventPortInstanceList - Action not constructed.";
            }
        }
    }
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
 * @brief ToolbarWidget::constructToolButtonMenu
 * The connections in here don't need to be applied to all the toolbar menus.
 * Only the top level menus - the ones directly attached to a QToolButton.
 * @param parentButton
 * @return
 */
ToolbarMenu* ToolbarWidget::constructToolButtonMenu(QToolButton* parentButton, bool instantPopup)
{
    if (parentButton) {
        ToolbarMenu* menu = new ToolbarMenu(this);
        connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(hideToolbar(QAction*)));
        connect(menu, SIGNAL(aboutToHide()), this, SLOT(hideToolbar()));
        if (instantPopup) {
            parentButton->setPopupMode(QToolButton::InstantPopup);
        } else {
            parentButton->setPopupMode(QToolButton::MenuButtonPopup);
            parentButton->setFixedWidth(55);
        }
        parentButton->setMenu(menu);
        return menu;
    }
    return 0;
}


/**
 * @brief ToolbarWidget::constructSubMenuAction
 * @param nodeItem
 * @param parentMenu
 * @return
 */
ToolbarMenuAction* ToolbarWidget::constructSubMenuAction(NodeItem* nodeItem, ToolbarMenu* parentMenu)
{
    if (!nodeItem || !parentMenu) {
        return 0;
    }

    NodeItem* parentNode = nodeItem->getParentNodeItem();

    // if nodeItem has a parent, check if there is already an action for it in parentMenu
    // if not, create one and attach a menu to it
    if (parentNode && parentNode->getNodeKind() != "Model") {

        ToolbarMenuAction* parentAction = parentMenu->getAction(parentNode);
        ToolbarMenu* parentActionMenu = 0;

        if (parentAction) {
            parentActionMenu = qobject_cast<ToolbarMenu*>(parentAction->menu());
        } else {
            parentAction = new ToolbarMenuAction(parentNode, this);
            parentActionMenu = new ToolbarMenu(this);
            parentAction->setMenu(parentActionMenu);
            parentMenu->addAction(parentAction);
        }

        // construct and add nodeItem's action to the parent action's menu
        if (parentActionMenu) {
            ToolbarMenuAction* action = new ToolbarMenuAction(nodeItem, this);
            parentActionMenu->addAction(action);
            return action;
        }
    }

    return 0;
}


/**
 * @brief ToolbarWidget::closeOpenMenus
 */
void ToolbarWidget::closeOpenMenus()
{
    foreach (ToolbarMenu* menu, openMenus) {
        menu->close();
    }
}

