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
}


/**
 * @brief ToolbarWidgetNew::setVisible
 * @param visible
 */
void ToolbarWidgetNew::setVisible(bool visible)
{
    if (visible) {
        //actionController->contextToolbar->updateSpacers();
        mainGroup->updateSpacers();
        QSize toolbarSize = toolbar->sizeHint();
        if (visible && toolbarSize.width() > iconSize.width()) {
            mainFrame->setFixedSize(toolbarSize);
            shadowFrame->setFixedSize(mainFrame->size() + QSize(3,3));
            setFixedSize(shadowFrame->size());
        } else {
            visible = false;
        }
    }

    mainFrame->setVisible(visible);
    shadowFrame->setVisible(visible);
    QWidget::setVisible(visible);
}


/**
 * @brief ToolbarWidgetNew::viewItem_Destructed
 * @param ID
 * @param viewItem
 */
void ToolbarWidgetNew::viewItem_Destructed(int ID, ViewItem *viewItem)
{

}

void ToolbarWidgetNew::execMenu()
{
    QAction* senderAction = qobject_cast<QAction*>(sender());
    if (senderAction == addChildAction) {
        //addMenu->popup(QCursor::pos());
        addMenu->popup(toolbar->mapToGlobal(toolbar->actionGeometry(addChildAction).bottomLeft()));
    } else if (senderAction == hardwareAction) {
        //hardwareMenu->popup(QCursor::pos());
        hardwareMenu->popup(toolbar->mapToGlobal(toolbar->actionGeometry(hardwareAction).bottomLeft()));
    } else if (senderAction == replicateCountAction) {
        //hardwareMenu->popup(QCursor::pos());
        //replicateMenu->popup(toolbar->mapToGlobal(toolbar->actionGeometry(replicateCountAction).bottomLeft()));
    } else {
        qWarning() << "ToolbarWidgetNew::execMenu - Action's menu is not dealt with.";
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

    /*
    if (actionController) {
        toolbar->addActions(actionController->contextToolbar->actions());
    }
    */

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
    hardwareAction = mainGroup->addAction(toolbarController->getHardwareAction(true));
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
    setupHardwareMenu();
    //setupReplicateCountMenu();
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

    QMenu* menu = new QMenu(this);
    menu->addAction(actionController->view_centerOnDefn);
    menu->addAction(actionController->toolbar_popOutDefn);
    definitionAction->setMenu(menu);

    QMenu* menu2 = new QMenu(this);
    menu2->addAction(actionController->view_centerOnImpl);
    menu2->addAction(actionController->toolbar_popOutImpl);
    implementationAction->setMenu(menu2);
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
    addMenu = new QMenu(this);

    foreach (QAction* action, toolbarController->getAdoptableKindsActions(true)) {
        QString kind = action->text();
        action->setProperty("kind", kind);
        if (kindsWithSubMenus.contains(kind)) {
            QMenu* menu = new QMenu(this);
            adoptableKindsSubMenus[kind] = menu;
            action->setMenu(menu);
        } else {
            connect(action, SIGNAL(triggered(bool)), toolbarController, SLOT(addChildNode()));
        }
        addMenu->addAction(action);
    }

    // TODO - replace this with the menu's own execMenu() slot
    connect(addChildAction, SIGNAL(triggered(bool)), this, SLOT(execMenu()));
}

void ToolbarWidgetNew::setupHardwareMenu()
{
    if (!toolbarController) {
        return;
    }

    hardwareMenu = new QMenu(this);
    hardwareMenu->addActions(toolbarController->getHardwareActions(true));

    // TODO - replace this with the menu's own execMenu() slot
    connect(hardwareAction, SIGNAL(triggered(bool)), this, SLOT(execMenu()));
}

void ToolbarWidgetNew::setupReplicateCountMenu()
{
    replicateCount = new QSpinBox(this);
    replicateCount->setMinimum(1);
    replicateCount->setMaximum(100000);

    applyReplicateCountButton = new QToolButton(this);

    QToolBar* replicateToolbar = new QToolBar(this);
    replicateToolbar->setStyleSheet("spacing:2px;");
    replicateToolbar->addWidget(replicateCount);
    replicateToolbar->addWidget(applyReplicateCountButton);
    //applyReplicateCountButton->setFixedHeight(25);
    replicateCount->setFixedHeight(25);

    QWidgetAction* rc = new QWidgetAction(this);
    rc->setDefaultWidget(replicateToolbar);
    replicateMenu->addAction(rc);

    connect(replicateCountAction, SIGNAL(triggered(bool)), this, SLOT(execMenu()));
}

