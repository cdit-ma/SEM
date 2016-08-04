#include "toolbarwidgetnew.h"
#include <QDebug>
#include <QProxyStyle>

#include "../../GUI/actionbutton.h"

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
    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
}

void ToolbarWidgetNew::addChildNode()
{
    if(sender()){
        QString nodeKind = sender()->property("kind").toString();
        if(toolbarController){
            toolbarController->addChildNode(nodeKind, itemPos);
        }
    }
}

void ToolbarWidgetNew::showToolbar(QPoint globalPos, QPointF itemPos)
{
    move(globalPos);
    this->itemPos = itemPos;
    setVisible(true);
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
    //applyReplicateCountButton->setIcon(theme->getIcon("Actions", "Tick"));

    //applyReplicateCountButton->setStyleSheet("background:" + theme->getHighlightColorHex() + ";");
    //applyReplicateCountButton->setStyleSheet(theme->getToolBarStyleSheet());
    //replicateToolbar->setStyleSheet(theme->getToolBarStyleSheet());
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
 * @brief ToolbarWidgetNew::populateDeploymentMenu
 */
void ToolbarWidgetNew::populateDeploymentMenu()
{
    QMenu* senderMenu = qobject_cast<QMenu*>(sender());

    // check if the sender is a menu and if it's already been populated
    if (!senderMenu || !senderMenu->isEmpty()) {
        return;
    }

    QList<QAction*> actions;
    if (senderMenu == hardwareMenu) {
        actions = toolbarController->getEdgeActionsOfKind(Edge::EC_DEPLOYMENT, true);
        if (actions.isEmpty()) {
            senderMenu->addAction(getInfoAction("NO_EC_DEPLOYMENT_CONNECT"));
            return;
        }
    } else {
        qWarning() << "ToolbarWidgetNew::populateDeploymentMenu - Sender menu not handled.";
        return;
    }

    senderMenu->addActions(actions);
}


/**
 * @brief ToolbarWidgetNew::menuActionTrigged
 * @param action
 */
void ToolbarWidgetNew::menuActionTrigged(QAction* action)
{
    qDebug() << "HELLO";
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
void ToolbarWidgetNew::viewItem_Destructed(int ID, ViewItem *viewItem)
{

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
    setupConnections();
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
    mainGroup->addAction(actionController->toolbar_displayedChildrenOption->constructSubAction(true));
    replicateCountAction = mainGroup->addAction(actionController->toolbar_replicateCount->constructSubAction(true));
    mainGroup->addAction(actionController->toolbar_setReadOnly->constructSubAction(true));
    mainGroup->addAction(actionController->toolbar_unsetReadOnly->constructSubAction(true));
    mainGroup->addSeperator();
    connectionsAction = mainGroup->addAction(toolbarController->getConnectedNodesAction(true));
    mainGroup->addAction(actionController->toolbar_getCPP->constructSubAction(true));
    mainGroup->addAction(actionController->view_viewInNewWindow->constructSubAction(true));
    mainGroup->addAction(actionController->help_wiki->constructSubAction(true));

    setupSplitMenus();

    toolbar->addActions(mainGroup->actions());

    addChildAction->setText("Add Child Entity");
    connectAction->setText("Connect Selection");
    hardwareAction->setText("Deploy Selection");
}


/**
 * @brief ToolbarWidgetNew::setupMenus
 */
void ToolbarWidgetNew::setupMenus()
{
    setupAddChildMenu();
    setupReplicateCountMenu();

    hardwareMenu = constructTopMenu(hardwareAction);

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
        } else {
            connect(action, SIGNAL(triggered(bool)), this, SLOT(addChildNode()));
        }
        addMenu->addAction(action);
    }
}


void ToolbarWidgetNew::setupReplicateCountMenu()
{
    replicateCount = new QSpinBox(this);
    replicateCount->setMinimum(1);
    replicateCount->setMaximum(100000);
    replicateCount->setFixedHeight(25);

    //applyReplicateCountButton = toolbarController->getToolAction("APPLY_REPLICATE_COUNT", false);
    applyReplicateCountButton = new QToolButton(this);
    applyReplicateCountButton->setIcon(QIcon(Theme::theme()->getImage("Actions", "Tick", QSize(), Qt::darkGreen)));
    //applyReplicateCountButton->setStyleSheet("background: rgb(128,255,0);");
    applyReplicateCountButton->setStyleSheet("background: white;");
    //applyReplicateCountButton->setStyleSheet("QToolButton:hover{ background: red; }");
    applyReplicateCountButton->setFixedSize(25,25);
    applyReplicateCountButton->setToolTip("Enter Replicate Count");

    //replicateToolbar = new QToolBar(this);
    QToolBar* replicateToolbar = new QToolBar(this);
    //replicateToolbar->setStyleSheet("spacing:2px;");
    replicateToolbar->addWidget(replicateCount);
    //applyReplicateCountAction = replicateToolbar->addWidget(applyReplicateCountButton);
    //replicateToolbar->addAction(applyReplicateCountButton);

    QWidgetAction* rc = new QWidgetAction(this);
    rc->setDefaultWidget(replicateToolbar);
    //rc->sets

    replicateMenu = constructTopMenu(replicateCountAction);
    replicateMenu->addAction(rc);
}


/**
 * @brief ToolbarWidgetNew::setupConnections
 */
void ToolbarWidgetNew::setupConnections()
{
    // close the toolbar whenever a toolbutton without a menu is triggered
    foreach (QAction* action, mainGroup->actions()) {
        if (action->menu() || popupMenuHash.contains(action)) {
            continue;
        }
        connect(action, SIGNAL(triggered(bool)), this, SLOT(setVisible(bool)));
    }

    connect(applyReplicateCountButton, SIGNAL(clicked(bool)), this, SLOT(setVisible(bool)));
    connect(applyReplicateCountButton, SIGNAL(clicked(bool)), replicateMenu, SLOT(setVisible(bool)));
    //connect(applyReplicateCountButton, SIGNAL(triggered(QAction*)), b, {qDebug() << "replicateMenu: "  << replicateMenu << " " << });

    connect(hardwareMenu, SIGNAL(aboutToShow()), this, SLOT(populateDeploymentMenu()));
}


/**
 * @brief ToolbarWidgetNew::clearDynamicMenus
 */
void ToolbarWidgetNew::clearDynamicMenus()
{
    hardwareMenu->clear();
    foreach (QMenu* menu, adoptableKindsSubMenus.values()) {
        menu->clear();
    }
}


/**
 * @brief ToolbarWidgetNew::constructTopMenu
 * @param parentAction
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
    return infoAction;
}

