#include "toolbarwidgetnew.h"
#include "../../GUI/actionbutton.h"

#include <QDebug>
#include <QProxyStyle>

/**
 * @brief The MenuStyle class
 * This class is used to style the menu and its items (actions).
 */
class MenuStyle : public QProxyStyle
{
public:
    int pixelMetric(PixelMetric metric, const QStyleOption* option = 0, const QWidget* widget = 0) const
    {
        int s = QProxyStyle::pixelMetric(metric, option, widget);
        if (metric == QStyle::PM_SmallIconSize) {
            s = 28;
        }
        return s;
    }
};


/**
 * @brief ToolbarWidgetNew::ToolbarWidgetNew
 * @param ac
 * @param parent
 */
ToolbarWidgetNew::ToolbarWidgetNew(ViewController *vc, QWidget *parent) : QWidget(parent)
{
    // using frames, combined with the set attribute and flags, allow
    // the toolbar to have a translucent background and a mock shadow
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::Popup);

/*
#ifdef Q_OS_WIN32
    // toolbar with a Qt::Popup flag doesn't allow hover/icon state changes in Ubuntu
    setWindowFlags(Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);// | Qt::Popup);
#else
    setWindowFlags(Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::Popup | Qt::Dialog);
#endif
*/

    viewController = vc;
    actionController = vc->getActionController();
    toolbarController = vc->getToolbarController();

    setStyle(new MenuStyle);
    setupToolbar();
    setupConnections();

    themeChanged();
}


/**
 * @brief ToolbarWidgetNew::themeChanged
 */
void ToolbarWidgetNew::themeChanged()
{
    Theme* theme = Theme::theme();
    setStyleSheet(theme->getRadioButtonStyleSheet() +
                  theme->getToolBarStyleSheet() +
                  theme->getMenuStyleSheet() +
                  "QToolBar{ padding: 3px; }"
                  "QToolButton{ padding: 4px; }"
                  "QMenu::item { padding: 2px 20px 2px 40px; }");

    QColor shadowColor = theme->getBackgroundColor().darker(130);
    QString shadowColorStr = theme->QColorToHex(shadowColor);
    QColor mainColor = theme->getAltBackgroundColor().darker(130);
    QString mainBackgroundColor = theme->QColorToHex(mainColor);

    mainFrame->setStyleSheet("background:" + mainBackgroundColor + "; border-radius: 6px;");
    shadowFrame->setStyleSheet("background:" + shadowColorStr + "; border-radius: 8px;");

    addChildAction->setIcon(theme->getIcon("Actions", "Plus"));
    connectAction->setIcon(theme->getIcon("Actions", "ConnectTo"));
    hardwareAction->setIcon(theme->getIcon("Actions", "Computer"));
    instancesAction->setIcon(theme->getIcon("Actions", "Instance"));
    connectionsAction->setIcon(theme->getIcon("Actions", "Connections"));

    //applyReplicateCountButton->setIcon(QIcon(theme->getImage("Actions", "Tick", QSize(32,32))));
    applyReplicateCountButton->setIcon(theme->getIcon("Actions", "Tick"));
    applyReplicateCountButton->setStyleSheet("QToolButton{ background:" + theme->getTextColorHex(Theme::CR_SELECTED) + ";}"
                                             "QToolButton:pressed{ background:" + theme->getPressedColorHex() + ";}");
}


/**
 * @brief ToolbarWidgetNew::showToolbar
 * @param globalPos
 * @param itemPos
 */
void ToolbarWidgetNew::showToolbar(QPoint globalPos, QPointF itemPos)
{
    move(globalPos);
    this->itemPos = itemPos;
    setVisible(true);
}


/**
 * @brief ToolbarWidgetNew::setVisible
 * @param visible
 */
void ToolbarWidgetNew::setVisible(bool visible)
{
    if (visible) {
        mainGroup->updateSpacers();
        QSize toolbarSize = toolbar->sizeHint();
        if (visible && toolbarSize.width() > iconSize.width()) {
            mainFrame->setFixedSize(toolbarSize);
            shadowFrame->setFixedSize(mainFrame->size() + QSize(3,3));
            setFixedSize(shadowFrame->size());
        } else {
            visible = false;
        }
    } else {
        // clear all dynamically populated menus
        clearDynamicMenus();
    }

    mainFrame->setVisible(visible);
    shadowFrame->setVisible(visible);
    QWidget::setVisible(visible);
}


/**
 * @brief ToolbarWidgetNew::execMenu
 */
void ToolbarWidgetNew::execMenu()
{
    QAction* senderAction = qobject_cast<QAction*>(sender());
    if (senderAction) {
        if (popupMenuHash.contains(senderAction)) {
            QMenu* menu = popupMenuHash[senderAction];
            menu->popup(toolbar->mapToGlobal(toolbar->actionGeometry(senderAction).bottomLeft()));
        }
    } else {
        qWarning() << "ToolbarWidgetNew::execMenu - Sender object is not a QAction.";
    }
}


/**
 * @brief ToolbarWidgetNew::populateDynamicMenu
 */
void ToolbarWidgetNew::populateDynamicMenu()
{
    QMenu* senderMenu = qobject_cast<QMenu*>(sender());

    // check if the sender is a menu and if it's already been populated
    if (!senderMenu || !senderMenu->isEmpty()) {
        return;
    }

    QList<QAction*> actions;
    if (senderMenu == hardwareMenu) {
        actions = toolbarController->getEdgeActionsOfKind(Edge::EC_DEPLOYMENT, true);

        foreach(QString kind, toolbarController->getKindsRequiringSubActions()){
            if(senderMenu == adoptableKindsSubMenus[kind]){
                actions = toolbarController->getDefinitionNodeActions(kind);
                break;
            }
        }



    } else if (senderMenu == adoptableKindsSubMenus["BlackBoxInstance"]) {
        actions = toolbarController->getNodeActionsOfKind("BlackBox", true);
    } else if (senderMenu == adoptableKindsSubMenus["ComponentImpl"]) {
        actions = toolbarController->getDefinitionNodeActions("ComponentImpl");
    } else if (senderMenu == adoptableKindsSubMenus["ComponentInstance"]) {
        actions = toolbarController->getNodeActionsOfKind("Component", true);
    } else if (senderMenu == adoptableKindsSubMenus["InEventPort"]) {
        actions = toolbarController->getNodeActionsOfKind("Aggregate", true);
    } else if (senderMenu == adoptableKindsSubMenus["OutEventPort"]) {
        actions = toolbarController->getNodeActionsOfKind("Aggregate", true);
    } else if (senderMenu == adoptableKindsSubMenus["InEventPortDelegate"]) {
        actions = toolbarController->getNodeActionsOfKind("Aggregate", true);
    } else if (senderMenu == adoptableKindsSubMenus["OutEventPortDelegate"]) {
        actions = toolbarController->getNodeActionsOfKind("Aggregate", true);
    } else if (senderMenu == adoptableKindsSubMenus["OutEventPortImpl"]) {
        //actions = toolbarController->getNodeActionsOfKind("OutEventPort", true);
    } else if (senderMenu == adoptableKindsSubMenus["AggregateInstance"]) {
        actions = toolbarController->getNodeActionsOfKind("Aggregate", true);
    } else if (senderMenu == adoptableKindsSubMenus["VectorInstance"]) {
        actions = toolbarController->getNodeActionsOfKind("Vector", true);
    } else if (senderMenu == adoptableKindsSubMenus["WorkerProcess"]) {
        //actions = toolbarController->getNodeActionsOfKind("BlackBox", true);
    } else {
        qWarning() << "ToolbarWidgetNew::populateDynamicMenu - Sender menu not handled.";
        return;
    }

    // if the menu is empty, show its info action
    if (actions.isEmpty()) {
        senderMenu->addAction(getInfoAction(dynamicMenuHash[senderMenu]));
    } else {
        senderMenu->addActions(actions);
    }
}


/**
 * @brief ToolbarWidgetNew::menuActionTrigged
 * This is called whenever an action without a sub-menu is triggered.
 * If the action triggered is not an info action, hide the toolbar.
 * @param action
 */
void ToolbarWidgetNew::menuActionTrigged(QAction* action)
{
    if (action->property("action-type") == "info") {
        return;
    } else {
        setVisible(false);
    }
}


/**
 * @brief ToolbarWidgetNew::viewItem_Destructed
 * @param ID
 * @param viewItem
 */
void ToolbarWidgetNew::viewItem_Destructed(int ID, ViewItem* viewItem)
{

}


/**
 * @brief ToolbarWidgetNew::addChildNode
 * @param action
 */
void ToolbarWidgetNew::addChildNode(QAction* action)
{
    if (action->property("action-type") == "info") {
        return;
    }
    if (toolbarController) {
        QString nodeKind = action->property("kind").toString();
        toolbarController->addChildNode(nodeKind, itemPos);
    }
}


/**
 * @brief ToolbarWidgetNew::setupToolbar
 */
void ToolbarWidgetNew::setupToolbar()
{
    shadowFrame = new QFrame(this);
    mainFrame = new QFrame(this);

    iconSize = QSize(20,20);
    toolbar = new QToolBar(this);
    toolbar->setIconSize(iconSize);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(toolbar, 0, Qt::AlignTop);

    setupActions();
    setupMenus();
}


/**
 * @brief ToolbarWidgetNew::setupActions
 */
void ToolbarWidgetNew::setupActions()
{
    connectGroup = new ActionGroup(this);
    connectGroup->addAction(toolbarController->getEdgeActionOfKind(Edge::EC_AGGREGATE, true));
    connectGroup->addAction(toolbarController->getEdgeActionOfKind(Edge::EC_ASSEMBLY, true));
    connectGroup->addAction(toolbarController->getEdgeActionOfKind(Edge::EC_DATA, true));
    connectGroup->addAction(toolbarController->getEdgeActionOfKind(Edge::EC_DEFINITION, true));
    connectGroup->addAction(toolbarController->getEdgeActionOfKind(Edge::EC_DEPLOYMENT, true));
    connectGroup->addAction(toolbarController->getEdgeActionOfKind(Edge::EC_WORKFLOW, true));

    mainGroup = new ActionGroup(this);
    addChildAction = mainGroup->addAction(toolbarController->getAdoptableKindsAction(true));
    mainGroup->addAction(actionController->getRootAction("Delete")->constructSubAction(true));
    connectAction = mainGroup->addAction(connectGroup->getGroupVisibilityAction()->constructSubAction(true));
    hardwareAction = mainGroup->addAction(toolbarController->getToolAction("EC_DEPLOYMENT_CONNECT", true));
    disconnectHardwareAction = mainGroup->addAction(toolbarController->getToolAction("EC_DEPLOYMENT_DISCONNECT", true));
    mainGroup->addSeperator();
    mainGroup->addAction(actionController->edit_alignVertical->constructSubAction(true));
    mainGroup->addAction(actionController->edit_alignHorizontal->constructSubAction(true));
    mainGroup->addSeperator();
    mainGroup->addAction(actionController->toolbar_expand->constructSubAction(true));
    mainGroup->addAction(actionController->toolbar_contract->constructSubAction(true));
    mainGroup->addSeperator();
    mainGroup->addAction(actionController->file_importSnippet->constructSubAction(true));
    mainGroup->addAction(actionController->file_exportSnippet->constructSubAction(true));
    mainGroup->addSeperator();
    definitionAction = mainGroup->addAction(actionController->view_centerOnDefn->constructSubAction(true));
    implementationAction = mainGroup->addAction(actionController->view_centerOnImpl->constructSubAction(true));
    instancesAction = mainGroup->addAction(toolbarController->getInstancesAction(true));
    mainGroup->addSeperator();
    hardwareViewOptionAction = mainGroup->addAction(actionController->toolbar_displayedChildrenOption->constructSubAction(true));
    replicateCountAction = mainGroup->addAction(actionController->toolbar_replicateCount->constructSubAction(true));
    mainGroup->addAction(actionController->toolbar_setReadOnly->constructSubAction(true));
    mainGroup->addAction(actionController->toolbar_unsetReadOnly->constructSubAction(true));
    mainGroup->addSeperator();
    connectionsAction = mainGroup->addAction(toolbarController->getConnectedNodesAction(true));
    mainGroup->addAction(actionController->toolbar_getCPP->constructSubAction(true));
    mainGroup->addAction(actionController->view_viewInNewWindow->constructSubAction(true));
    mainGroup->addAction(actionController->help_wiki->constructSubAction(true));

    // this needs to be called before the actions are added to the toolbar for the split buttons to work
    setupSplitMenus();
    toolbar->addActions(mainGroup->actions());

    // update the tooltips for certain actions
    addChildAction->setToolTip("Add Child Entity");
    connectAction->setToolTip("Connect Selection");
    hardwareAction->setToolTip("Deploy Selection");
}


/**
 * @brief ToolbarWidgetNew::setupMenus
 */
void ToolbarWidgetNew::setupMenus()
{
    setupAddChildMenu();
    setupReplicateCountMenu();
    setupHardwareViewOptionMenu();
    hardwareMenu = constructTopMenu(hardwareAction);

    // this hash is used for clearing dynamic menus and to store their matching info action key
    dynamicMenuHash[hardwareMenu] = "INFO_NO_VALID_DEPLOYMENT_NODES";
    dynamicMenuHash[adoptableKindsSubMenus["ComponentImpl"]] = "INFO_NO_UNIMPLEMENTED_COMPONENTS";
    dynamicMenuHash[adoptableKindsSubMenus["ComponentInstance"]] = "INFO_NO_COMPONENTS";
    dynamicMenuHash[adoptableKindsSubMenus["BlackBoxInstance"]] = "INFO_NO_BLACKBOXES";
    dynamicMenuHash[adoptableKindsSubMenus["InEventPort"]] = "INFO_NO_AGGREGATES";
    dynamicMenuHash[adoptableKindsSubMenus["OutEventPort"]] = "INFO_NO_AGGREGATES";
    dynamicMenuHash[adoptableKindsSubMenus["InEventPortDelegate"]] = "INFO_NO_AGGREGATES";
    dynamicMenuHash[adoptableKindsSubMenus["OutEventPortDelegate"]] = "INFO_NO_AGGREGATES";
    dynamicMenuHash[adoptableKindsSubMenus["OutEventPortImpl"]] = "INFO_NO_OUTEVENTPORTS";
    dynamicMenuHash[adoptableKindsSubMenus["AggregateInstance"]] = "INFO_NO_AGGREGATES";
    dynamicMenuHash[adoptableKindsSubMenus["VectorInstance"]] = "INFO_NO_VECTORS";
    dynamicMenuHash[adoptableKindsSubMenus["WorkerProcess"]] = "INFO_NO_FUCNTIONS";
}


/**
 * @brief ToolbarWidgetNew::setupSplitMenus
 * In order for the split button/menu to appear correctly, this function needs
 * to be called before these menus' parent actions are added to the toolbar.
 */
void ToolbarWidgetNew::setupSplitMenus()
{
    if (!actionController) {
        return;
    }

    QMenu* menu = constructTopMenu(definitionAction, false);
    menu->addAction(actionController->view_centerOnDefn);
    menu->addAction(actionController->toolbar_popOutDefn);

    QMenu* menu2 = constructTopMenu(implementationAction, false);
    menu2->addAction(actionController->view_centerOnImpl);
    menu2->addAction(actionController->toolbar_popOutImpl);
}


/**
 * @brief ToolbarWidgetNew::setupAddChildMenu
 */
void ToolbarWidgetNew::setupAddChildMenu()
{
    if (!toolbarController) {
        return;
    }

    QStringList kindsWithSubMenus = toolbarController->getKindsRequiringSubActions();
    addMenu = constructTopMenu(addChildAction);

    foreach (QAction* action, toolbarController->getAdoptableKindsActions(true)) {
        QString kind = action->text();
        action->setProperty("kind", kind);
        if (kindsWithSubMenus.contains(kind)) {
            QMenu* menu = new QMenu(this);
            adoptableKindsSubMenus[kind] = menu;
            action->setMenu(menu);
            qDebug() << "Kind: " << kind;
        }
        addMenu->addAction(action);
    }
}


/**
 * @brief ToolbarWidgetNew::setupReplicateCountMenu
 */
void ToolbarWidgetNew::setupReplicateCountMenu()
{
    replicateCount = new QSpinBox(this);
    replicateCount->setMinimum(1);
    replicateCount->setMaximum(100000);
    replicateCount->setFixedHeight(25);

    applyReplicateCountButton = new QToolButton(this);
    applyReplicateCountButton->setFixedSize(25,25);
    applyReplicateCountButton->setToolTip("Enter Replicate Count");

    QToolBar* replicateToolbar = new QToolBar(this);
    replicateToolbar->addWidget(replicateCount);
    replicateToolbar->addWidget(applyReplicateCountButton);

    QWidgetAction* rc = new QWidgetAction(this);
    rc->setDefaultWidget(replicateToolbar);

    replicateMenu = constructTopMenu(replicateCountAction);
    replicateMenu->addAction(rc);
}


/**
 * @brief ToolbarWidgetNew::setupHardwareViewOptionMenu
 */
void ToolbarWidgetNew::setupHardwareViewOptionMenu()
{
    allNodes = new QRadioButton("All", this);
    connectedNodes = new QRadioButton("Connected", this);
    unconnectedNodes = new QRadioButton("Unconnected", this);
    QWidgetAction* a1 = new QWidgetAction(this);
    QWidgetAction* a2 = new QWidgetAction(this);
    QWidgetAction* a3 = new QWidgetAction(this);
    a1->setDefaultWidget(allNodes);
    a2->setDefaultWidget(connectedNodes);
    a3->setDefaultWidget(unconnectedNodes);

    hardwareViewOptionMenu = constructTopMenu(hardwareViewOptionAction);
    hardwareViewOptionMenu->addAction(a1);
    hardwareViewOptionMenu->addAction(a2);
    hardwareViewOptionMenu->addAction(a3);
}


/**
 * @brief ToolbarWidgetNew::setupConnections
 */
void ToolbarWidgetNew::setupConnections()
{
    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));

    // close the toolbar whenever a toolbutton without a menu is triggered
    foreach (QAction* action, mainGroup->actions()) {
        if (action->menu() || popupMenuHash.contains(action)) {
            continue;
        }
        connect(action, SIGNAL(triggered(bool)), this, SLOT(setVisible(bool)));
    }

    connect(applyReplicateCountButton, SIGNAL(clicked(bool)), this, SLOT(setVisible(bool)));
    connect(applyReplicateCountButton, SIGNAL(clicked(bool)), replicateMenu, SLOT(setVisible(bool)));

    connect(addMenu, SIGNAL(triggered(QAction*)), this, SLOT(addChildNode(QAction*)));
    connect(hardwareMenu, SIGNAL(aboutToShow()), this, SLOT(populateDynamicMenu()));

    foreach (QMenu* menu, adoptableKindsSubMenus.values()) {
        connect(menu, SIGNAL(aboutToShow()), this, SLOT(populateDynamicMenu()));
    }
}


/**
 * @brief ToolbarWidgetNew::clearDynamicMenus
 * This clears all dynamically populated menus.
 */
void ToolbarWidgetNew::clearDynamicMenus()
{
    foreach (QMenu* menu, dynamicMenuHash.keys()) {
        menu->clear();
    }
}


/**
 * @brief ToolbarWidgetNew::constructTopMenu
 * @param parentAction - the action the menu to be constructed is spawned off of
 * @param instantPopup - if true, the parentAction doesn't show its menu arrow and
 *                       this menu is executed using the toolbar's execMenu slot
 * @return
 */
QMenu *ToolbarWidgetNew::constructTopMenu(QAction* parentAction, bool instantPopup)
{
   QMenu* menu = new QMenu(this);
   connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(menuActionTrigged(QAction*)));
   if (parentAction) {
       if (instantPopup) {
           popupMenuHash[parentAction] = menu;
           connect(parentAction, SIGNAL(triggered(bool)), this, SLOT(execMenu()));
       } else {
           parentAction->setMenu(menu);
       }
   }
   return menu;
}


/**
 * @brief ToolbarWidgetNew::getInfoAction
 * @param hashKey
 * @return
 */
QAction* ToolbarWidgetNew::getInfoAction(QString hashKey)
{
    QAction* infoAction = toolbarController->getToolAction(hashKey, false);
    infoAction->setProperty("action-type", "info");
    infoAction->blockSignals(true);
    return infoAction;
}

