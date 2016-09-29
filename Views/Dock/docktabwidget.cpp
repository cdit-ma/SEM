#include "docktabwidget.h"
#include "dockwidgetactionitem.h"
#include "dockwidgetparentactionitem.h"

#include "../../Controllers/ToolbarController/nodeviewitemaction.h"
#include "../../Utils/rootaction.h"
#include "../../theme.h"

#include <QToolBar>
#include <QVBoxLayout>
#include <QHBoxLayout>

#define TAB_PADDING 20
#define DOCK_SPACING 3
#define MIN_WIDTH 130
#define MAX_WIDTH 250


/**
 * @brief DockTabWidget::DockTabWidget
 * @param vc
 * @param parent
 */
DockTabWidget::DockTabWidget(ViewController *vc, QWidget* parent) : QWidget(parent)
{
    viewController = vc;
    toolActionController = viewController->getToolbarController();

    setupLayout();
    setupConnections();

    themeChanged();
    selectionChanged();
}


/**
 * @brief DockTabWidget::themeChanged
 */
void DockTabWidget::themeChanged()
{    
    Theme* theme = Theme::theme();
    setStyleSheet(theme->getToolBarStyleSheet() +
                  "QWidget{ color:" + theme->getTextColorHex() + ";}"
                  "QToolButton{ border-radius: " % theme->getSharpCornerRadius() % "; background:" + theme->getAltBackgroundColorHex() + ";}"
                  "QToolButton::checked:!hover{ background:" + theme->getBackgroundColorHex() + ";}"
                  "QStackedWidget{ border: 0px; background:" + theme->getBackgroundColorHex() + ";}");

    QIcon partIcon;
    partIcon.addPixmap(theme->getImage("Actions", "Plus", QSize(), theme->getMenuIconColor()));
    partIcon.addPixmap(theme->getImage("Actions", "Plus", QSize(), theme->getMenuIconColor(Theme::CR_SELECTED)), QIcon::Active);

    QIcon hardwareIcon;
    hardwareIcon.addPixmap(theme->getImage("Actions", "Computer", QSize(), theme->getMenuIconColor()));
    hardwareIcon.addPixmap(theme->getImage("Actions", "Computer", QSize(), theme->getMenuIconColor(Theme::CR_SELECTED)), QIcon::Active);

    partsButton->setIcon(partIcon);
    hardwareButton->setIcon(hardwareIcon);
}


/**
 * @brief DockTabWidget::selectionChanged
 */
void DockTabWidget::selectionChanged()
{
    refreshDock();
}


/**
 * @brief DockTabWidget::tabClicked
 * This slot treats the dock buttons like tabs; it toggles between the parts and hardware docks.
 * @param checked
 */
void DockTabWidget::tabClicked(bool checked)
{
    QToolButton* senderButton = qobject_cast<QToolButton*>(sender());

    // disallow sender button from being un-checked; ignore action
    if (!checked) {
        senderButton->setChecked(true);
        return;
    }

    QToolButton* otherButton = 0;
    DockWidget* dockWidget = 0;

    if (senderButton == partsButton) {
        otherButton = hardwareButton;
        dockWidget = partsDock;
    } else if (senderButton == hardwareButton) {
        otherButton = partsButton;
        dockWidget = hardwareDock;
    } else {
        qWarning() << "DockTabWidget::tabClicked - Tab button not handled.";
        return;
    }

    // if sender button is checked, un-check the other button
    otherButton->setChecked(false);
    openRequiredDock(dockWidget);
}


/**
 * @brief DockTabWidget::dockActionClicked
 * This is called when a DockWidgetActionItem is clicked.
 * @param action
 */
void DockTabWidget::dockActionClicked(DockWidgetActionItem* action)
{
    DockWidget* dock = qobject_cast<DockWidget*>(sender());
    ToolbarController::DOCK_TYPE dt = dock->getDockType();
    triggeredAdoptableKind = "";

    switch (dt) {
    case ToolbarController::PARTS:
    {
        triggeredAdoptableKind = action->getProperty("kind").toString();;
        if (action->requiresSubAction()) {
            if (triggeredAdoptableKind == "WorkerProcess") {
                openRequiredDock(functionsDock);
            } else {
                openRequiredDock(definitionsDock);
            }
        } else {
            toolActionController->addChildNode(triggeredAdoptableKind, QPoint(0,0));
        }
        break;
    }
    case ToolbarController::DEFINITIONS:
    {
        QVariant ID = action->getProperty("ID");
        QVariant parentKind = action->getProperty("parent-kind");
        toolActionController->addConnectedChildNode(ID.toInt(), parentKind.toString(), QPointF());
        // re-open the parts dock
        openRequiredDock(partsDock);
        break;
    }
    case ToolbarController::FUNCTIONS:{
        // construct WorkerProcess
        QVariant ID = action->getProperty("ID");
        toolActionController->addWorkerProcess(ID.toInt(), QPointF());
        // re-open the parts dock
        openRequiredDock(partsDock);
        break;
    }
    case ToolbarController::HARDWARE:
    {
        int ID = action->getProperty("ID").toInt();
        toolActionController->removeEdge(-1, Edge::EC_DEPLOYMENT);

        if(!action->isHighlighted()){
            toolActionController->addEdge(ID, Edge::EC_DEPLOYMENT);
        }
        break;
    }
    }
}


/**
 * @brief DockTabWidget::dockBackButtonClicked
 * This is triggered whenever the back button of the definitions or functions dock is clicked.
 * It returns the list of displayed dock items to the adoptable list.
 */
void DockTabWidget::dockBackButtonClicked()
{
    openRequiredDock(partsDock);
}


/**
 * @brief DockTabWidget::onActionFinished
 */
void DockTabWidget::onActionFinished()
{
    refreshDock();
}


/**
 * @brief DockTabWidget::setupLayout
 */
void DockTabWidget::setupLayout()
{
    partsButton = new QToolButton(this);
    hardwareButton = new QToolButton(this);
    stackedWidget = new QStackedWidget(this);

    partsButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    partsButton->setToolTip("Parts Dock");
    partsButton->setIconSize(QSize(24,24));
    partsButton->setCheckable(true);
    partsButton->setChecked(true);

    hardwareButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    hardwareButton->setToolTip("Hardware Dock");
    hardwareButton->setIconSize(QSize(24,24));
    hardwareButton->setCheckable(true);
    hardwareButton->setChecked(false);

    QToolBar* toolbar = new QToolBar(this);
    toolbar->addWidget(partsButton);
    toolbar->addWidget(hardwareButton);

    QVBoxLayout* vLayout = new QVBoxLayout(this);
    vLayout->setMargin(0);
    vLayout->setSpacing(2);
    vLayout->addWidget(toolbar);
    vLayout->addWidget(stackedWidget, 1);

    setContentsMargins(1,2,1,1);
    setMinimumWidth(MIN_WIDTH);
    setMaximumWidth(MAX_WIDTH);

    setupDocks();
}


/**
 * @brief DockTabWidget::setupDocks
 */
void DockTabWidget::setupDocks()
{
    if (!toolActionController) {
        return;
    }

    partsDock = new DockWidget(toolActionController, ToolbarController::PARTS, this);
    definitionsDock = new DockWidget(toolActionController, ToolbarController::DEFINITIONS, this);
    functionsDock = new DockWidget(toolActionController, ToolbarController::FUNCTIONS, this);
    hardwareDock = new DockWidget(toolActionController, ToolbarController::HARDWARE, this);

    stackedWidget->addWidget(partsDock);
    stackedWidget->addWidget(definitionsDock);
    stackedWidget->addWidget(functionsDock);
    stackedWidget->addWidget(hardwareDock);

    // information shown when the dock is empty
    partsDock->updateInfoLabel("The selected entity does not have any adoptable entities");
    functionsDock->updateInfoLabel("No worker definitions have been imported");
    hardwareDock->updateInfoLabel("There are no available hardware nodes");

    // populate the parts dock
    adoptableKindAction = toolActionController->getAdoptableKindsAction(false);
    foreach (QAction* action, toolActionController->getAdoptableKindsActions(true)) {
        DockWidgetActionItem* dockItem = new DockWidgetActionItem(action, this);
        QString actionKind = action->text();
        if (toolActionController->kindsWithSubActions.contains(actionKind)) {
            dockItem->setSubActionRequired(true);
        }
        partsDock->addItem(dockItem);
    }
}


/**
 * @brief DockTabWidget::setupConnections
 */
void DockTabWidget::setupConnections()
{
    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    connect(viewController, SIGNAL(vc_actionFinished()), this, SLOT(onActionFinished()));
    connect(viewController->getSelectionController(), SIGNAL(selectionChanged(int)), this, SLOT(selectionChanged()));

    connect(partsButton, SIGNAL(clicked(bool)), this, SLOT(tabClicked(bool)));
    connect(hardwareButton, SIGNAL(clicked(bool)), this, SLOT(tabClicked(bool)));

    connect(partsDock, SIGNAL(actionClicked(DockWidgetActionItem*)), this, SLOT(dockActionClicked(DockWidgetActionItem*)));
    connect(definitionsDock, SIGNAL(actionClicked(DockWidgetActionItem*)), this, SLOT(dockActionClicked(DockWidgetActionItem*)));
    connect(functionsDock, SIGNAL(actionClicked(DockWidgetActionItem*)), this, SLOT(dockActionClicked(DockWidgetActionItem*)));
    connect(hardwareDock, SIGNAL(actionClicked(DockWidgetActionItem*)), this, SLOT(dockActionClicked(DockWidgetActionItem*)));

    connect(definitionsDock, SIGNAL(backButtonClicked()), this, SLOT(dockBackButtonClicked()));
    connect(functionsDock, SIGNAL(backButtonClicked()), this, SLOT(dockBackButtonClicked()));

    connect(toolActionController, SIGNAL(hardwareCreated(int)), hardwareDock, SLOT(viewItemConstructed(int)));
    connect(toolActionController, SIGNAL(hardwareDestructed(int)), hardwareDock, SLOT(viewItemDestructed(int)));
}


/**
 * @brief DockTabWidget::openRequiredDock
 * @param dockWidget
 */
void DockTabWidget::openRequiredDock(DockWidget* dockWidget)
{
    if (dockWidget) {

        ToolbarController::DOCK_TYPE dockType = dockWidget->getDockType();
        bool isDefinitionsDock = false;
        bool showInfoLabel = false;

        switch (dockType) {
        case ToolbarController::PARTS:
            showInfoLabel = !adoptableKindAction->isEnabled();
            break;
        case ToolbarController::DEFINITIONS:
        {
            // update header text; update entity kind to construct
            dockWidget->updateHeaderText(triggeredAdoptableKind);
            isDefinitionsDock = true;
            // get definitions list from controller
            QList<NodeViewItemAction*> actions = toolActionController->getDefinitionNodeActions(triggeredAdoptableKind);
            populateDock(dockWidget, actions, true);
            showInfoLabel = dockWidget->isEmpty();
            break;
        }
        case ToolbarController::FUNCTIONS:{
            // update header text; update entity kind to construct
            dockWidget->updateHeaderText(triggeredAdoptableKind);
            // get functions list from controller
            QList<NodeViewItemAction*> actions = toolActionController->getWorkerFunctions();
            populateDock(dockWidget, actions, true);
            showInfoLabel = dockWidget->isEmpty();
            break;
        }
        case ToolbarController::HARDWARE:
            showInfoLabel = dockWidget->isEmpty();
            refreshDock(hardwareDock);
            break;
        default:
            break;
        }

        // if the dock is empty, show its information label
        if (showInfoLabel && isDefinitionsDock) {
            QString hashKey = toolActionController->getInfoActionKeyForAdoptableKind(triggeredAdoptableKind);
            QAction* infoAction = toolActionController->getToolAction(hashKey, true);
            if (infoAction) {
                dockWidget->updateInfoLabel(infoAction->text());
            }
        }
        dockWidget->displayInfoLabel(showInfoLabel);

        // set the required dock
        stackedWidget->setCurrentWidget(dockWidget);
    }
}


/**
 * @brief DockTabWidget::populateDock
 * @param dockWidget
 * @param actions
 */
void DockTabWidget::populateDock(DockWidget* dockWidget, QList<NodeViewItemAction*> actions, bool groupByParent)
{
    // clear the dock first
    dockWidget->clearDock();

    if (groupByParent) {

        QHash<NodeViewItemAction*, QList<DockWidgetActionItem*> > dockItemsHash;

        // group by the parent view item actions
        foreach (NodeViewItemAction* action, actions) {
            NodeViewItemAction* parentViewItemAction = action->getParentViewItemAction();
            if (parentViewItemAction) {
                DockWidgetActionItem* dockItem = constructDockActionItem(action);
                dockItemsHash[parentViewItemAction].append(dockItem);
            }
        }

        // add the parent item and then its children to the dock
        foreach (NodeViewItemAction* parentViewItemAction, dockItemsHash.keys()) {
            QAction* parentAction = parentViewItemAction->constructSubAction(false);
            parentAction->setProperty("ID", parentViewItemAction->getID());
            DockWidgetParentActionItem* parentItem = new DockWidgetParentActionItem(parentAction, this);
            dockWidget->addItem(parentItem);
            foreach (DockWidgetActionItem* item, dockItemsHash.value(parentViewItemAction)) {
                dockWidget->addItem(item);
                parentItem->addToChildrenActions(item->getAction());
            }
            // this initially contracts the parent dock items
            parentItem->setToggledState(false);
        }

    } else {
        foreach (NodeViewItemAction* action, actions) {
            DockWidgetActionItem* dockItem = constructDockActionItem(action);
            dockWidget->addItem(dockItem);
        }
    }
}


/**
 * @brief DockTabWidget::refreshDock
 * This is called whenever the selection has changed and when actionFinished is called.
 * It's also used to update the hardware dock when it is opened.
 * @param dockWidget
 */
void DockTabWidget::refreshDock(DockWidget* dockWidget)
{
    bool updatePartsDock = false;
    if (dockWidget) {
        updatePartsDock = dockWidget == partsDock;
    } else {
        updatePartsDock = partsButton->isChecked();
    }

    if (updatePartsDock) {
        // if either of the definitions or functions list is displayed, close them and re-open the parts list
        if (stackedWidget->currentWidget() != partsDock) {
            openRequiredDock(partsDock);
        } else {
            partsDock->displayInfoLabel(!adoptableKindAction->isEnabled());
        }
    } else {
        // update hardware dock; update highlighted dock item
        QList<ViewItem*> connectedHardwareItems = viewController->getExistingEdgeEndPointsForSelection(Edge::EC_DEPLOYMENT);
        // clear previous highlighted item
        hardwareDock->highlightItem();
        if (connectedHardwareItems.count() == 1) {
            int connectedItemID = connectedHardwareItems.at(0)->getID();
            hardwareDock->highlightItem(connectedItemID);
        }
    }
}


/**
 * @brief DockTabWidget::getDock
 * @param dt
 * @return
 */
DockWidget* DockTabWidget::getDock(ToolbarController::DOCK_TYPE dt)
{
    switch (dt) {
    case ToolbarController::PARTS:
        return partsDock;
    case ToolbarController::DEFINITIONS:
        return definitionsDock;
    case ToolbarController::FUNCTIONS:
        return functionsDock;
    case ToolbarController::HARDWARE:
        return hardwareDock;
    default:
        qWarning() << "DockTabWidget::getDock - Dock type is unknown.";
        return 0;
    }
}


/**
 * @brief DockTabWidget::constructDockActionItem
 * @param action
 * @return
 */
DockWidgetActionItem *DockTabWidget::constructDockActionItem(NodeViewItemAction* action)
{
    // construct a sub-action for the nodeviewitemaction and use that action for the dock item
    QAction* subAction = action->constructSubAction(false);
    subAction->setProperty("ID", action->getID());
    if (!triggeredAdoptableKind.isEmpty()) {
        subAction->setProperty("parent-kind", triggeredAdoptableKind);
    }
    DockWidgetActionItem* dockItem = new DockWidgetActionItem(subAction, this);
    return dockItem;
}








