#include "docktabwidget.h"
#include "dockwidgetactionitem.h"
#include "../theme.h"

#include <QToolBar>
#include <QVBoxLayout>
#include <QHBoxLayout>

#define TAB_PADDING 20
#define DOCK_SPACING 3
#define DOCK_SEPARATOR_WIDTH 5
#define MIN_WIDTH 130
#define MAX_WIDTH 200

/**
 * @brief DockTabWidget::DockTabWidget
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
                  "QToolButton{ border-radius: 2px; background:" + theme->QColorToHex(theme->getAltBackgroundColor().darker(150)) + ";}"
                  "QToolButton::checked:!hover{ background:" + theme->getAltBackgroundColorHex() + ";}"
                  "QStackedWidget{ border: 0px; background:" + theme->getAltBackgroundColorHex() + ";}");

    partsButton->setIcon(theme->getIcon("Actions", "Plus"));
    hardwareButton->setIcon(theme->getIcon("Actions", "Computer"));
}


/**
 * @brief DockTabWidget::selectionChanged
 */
void DockTabWidget::selectionChanged()
{
    // if either of the definitions or functions list is displayed, close them and re-open the parts list
    if (partsButton->isChecked()) {
        if (stackedWidget->currentWidget() != partsDock) {
            stackedWidget->setCurrentWidget(partsDock);
        }
    }
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
    openRequiredDock(dockWidget->getDockType());
}


/**
 * @brief DockTabWidget::dockActionClicked
 * @param action
 */
void DockTabWidget::dockActionClicked(DockWidgetActionItem* action)
{
    DockWidget* dock = qobject_cast<DockWidget*>(sender());
    ToolActionController::DOCK_TYPE dt = dock->getDockType();
    triggeredAdoptableKind = "";

    switch (dt) {
    case ToolActionController::PARTS:
    {
        triggeredAdoptableKind = action->getProperty("kind").toString();;
        if (action->requiresSubAction()) {
            if (triggeredAdoptableKind == "WorkerProcess") {
                openRequiredDock(ToolActionController::FUNCTIONS);
            } else {
                openRequiredDock(ToolActionController::DEFINITIONS);
            }
        } else {
            toolActionController->addChildNode(triggeredAdoptableKind, QPoint(0,0));
        }
        break;
    }
    case ToolActionController::DEFINITIONS:
    {
        QVariant ID = action->property("ID");
        QVariant parentKind = action->property("parent-kind");
        toolActionController->addConnectedChildNode(ID.toInt(), parentKind.toString(), QPointF());
        break;
    }
    case ToolActionController::FUNCTIONS:
        // construct WorkerProcess
        break;
    case ToolActionController::HARDWARE:
    {
        QString kind = action->property("parent-kind").toString();
        Edge::EDGE_CLASS edgeKind = Edge::getEdgeClass(kind);
        int ID = action->property("ID").toInt();
        toolActionController->addEdge(ID, edgeKind);
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
    stackedWidget->setCurrentWidget(partsDock);
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

    partsDock = new DockWidget(toolActionController, ToolActionController::PARTS, this);
    definitionsDock = new DockWidget(toolActionController, ToolActionController::DEFINITIONS, this);
    functionsDock = new DockWidget(toolActionController, ToolActionController::FUNCTIONS, this);
    hardwareDock = new DockWidget(toolActionController, ToolActionController::HARDWARE, this);

    stackedWidget->addWidget(partsDock);
    stackedWidget->addWidget(definitionsDock);
    stackedWidget->addWidget(functionsDock);
    stackedWidget->addWidget(hardwareDock);

    partsDock->addActionItems(toolActionController->getAdoptableKindsActions(true));

    QAction* testAction = new QAction(Theme::theme()->getIcon("Actions", "Help"), "", this);
    hardwareDock->addActionItem(testAction);

    hardwareDock->addItem("Hello");
    hardwareDock->addItem("Test");
    hardwareDock->addItem("DockItem");
    hardwareDock->addItem("gsdgksdjhgsdghgggeEND");
}


/**
 * @brief DockTabWidget::setupConnections
 */
void DockTabWidget::setupConnections()
{
    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    connect(viewController->getSelectionController(), SIGNAL(selectionChanged(int)), this, SLOT(selectionChanged()));

    connect(partsButton, SIGNAL(clicked(bool)), this, SLOT(tabClicked(bool)));
    connect(hardwareButton, SIGNAL(clicked(bool)), this, SLOT(tabClicked(bool)));

    connect(partsDock, SIGNAL(actionClicked(DockWidgetActionItem*)), this, SLOT(dockActionClicked(DockWidgetActionItem*)));
    connect(definitionsDock, SIGNAL(actionClicked(DockWidgetActionItem*)), this, SLOT(dockActionClicked(DockWidgetActionItem*)));
    connect(functionsDock, SIGNAL(actionClicked(DockWidgetActionItem*)), this, SLOT(dockActionClicked(DockWidgetActionItem*)));
    connect(hardwareDock, SIGNAL(actionClicked(DockWidgetActionItem*)), this, SLOT(dockActionClicked(DockWidgetActionItem*)));

    connect(definitionsDock, SIGNAL(backButtonClicked()), this, SLOT(dockBackButtonClicked()));
    connect(functionsDock, SIGNAL(backButtonClicked()), this, SLOT(dockBackButtonClicked()));
}


/**
 * @brief DockTabWidget::openRequiredDock
 * @param dt
 */
void DockTabWidget::openRequiredDock(ToolActionController::DOCK_TYPE dt)
{
    DockWidget* dockWidget = getDock(dt);
    if (dockWidget) {
        switch (dt) {
        case ToolActionController::DEFINITIONS:
        {
            QList<NodeViewItemAction*> actions = toolActionController->getDefinitionNodeActions(triggeredAdoptableKind);
            populateDock(dockWidget, actions);
            break;
        }
        case ToolActionController::HARDWARE:
        {
            break;
            QList<NodeViewItemAction*> actions = toolActionController->getEdgeActionsOfKind(Edge::EC_DEPLOYMENT);
            populateDock(dockWidget, actions);
            break;
        }
        default:
            break;
        }
        stackedWidget->setCurrentWidget(dockWidget);
    }
}


/**
 * @brief DockTabWidget::populateDock
 * @param dockWidget
 * @param actions
 */
void DockTabWidget::populateDock(DockWidget* dockWidget, QList<NodeViewItemAction*> actions)
{
    // clear the dock first
    dockWidget->clearDock();

    foreach (NodeViewItemAction* action, actions) {
        DockWidgetActionItem* actionWidget = dockWidget->addActionItem(action->constructSubAction(false));
        actionWidget->setProperty("ID", action->getID());
        if (!triggeredAdoptableKind.isEmpty()) {
            actionWidget->setProperty("parent-kind", triggeredAdoptableKind);
        }
    }

    // update header text; update entity kind to construct
    dockWidget->updateHeaderText(triggeredAdoptableKind);
}


/**
 * @brief DockTabWidget::getDock
 * @param dt
 * @return
 */
DockWidget* DockTabWidget::getDock(ToolActionController::DOCK_TYPE dt)
{
    switch (dt) {
    case ToolActionController::PARTS:
        return partsDock;
    case ToolActionController::DEFINITIONS:
        return definitionsDock;
    case ToolActionController::FUNCTIONS:
        return functionsDock;
    case ToolActionController::HARDWARE:
        return hardwareDock;
    default:
        qWarning() << "DockTabWidget::getDock - Dock type is unknown.";
        return 0;
    }
}





