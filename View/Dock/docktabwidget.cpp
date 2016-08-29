#include "docktabwidget.h"
#include "../theme.h"
#include "dockactionwidget.h"

#include <QToolBar>
#include <QVBoxLayout>
#include <QHBoxLayout>

#define TAB_PADDING 20
#define DOCK_SPACING 3
#define DOCK_SEPARATOR_WIDTH 5
#define MIN_WIDTH 120
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
    setStyleSheet("QWidget{ color:" + theme->getTextColorHex() + ";}"
                  "QPushButton{ background:" + theme->QColorToHex(theme->getAltBackgroundColor().darker(150)) + ";}"
                  "QPushButton:hover{ border-width: 2px; }"
                  "QPushButton:hover{ background:" + theme->getHighlightColorHex() + ";}"
                  "QPushButton:hover::checked{ background:" + theme->getHighlightColorHex() + ";}"
                  "QPushButton::checked{ background:" + theme->getAltBackgroundColorHex() + ";}"
                  "QStackedWidget{ border: 0px; background:" + theme->getAltBackgroundColorHex() + ";}"
                  "QScrollArea{ border: 0px; background:" + theme->getAltBackgroundColorHex() + ";}");

    partsButton->setIcon(theme->getImage("Actions", "Plus", QSize(), theme->getTextColor(theme->CR_NORMAL)));
    hardwareButton->setIcon(theme->getImage("Actions", "Computer", QSize(), theme->getTextColor(theme->CR_NORMAL)));
}


/**
 * @brief DockTabWidget::selectionChanged
 */
void DockTabWidget::selectionChanged()
{
    // if either of the definitions or functions list is displayed, close them and re-open the parts list
    if (partsButton->isChecked()) {
        if (stackedWidget->currentWidget() != partsDock) {
            changeDisplayedDockWidget(partsDock);
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
    QPushButton* senderButton = qobject_cast<QPushButton*>(sender());

    // disallow sender button from being un-checked; ignore action
    if (!checked) {
        senderButton->setChecked(true);
        return;
    }

    QPushButton* otherButton = 0;
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
    changeDisplayedDockWidget(dockWidget);
}


/**
 * @brief DockTabWidget::dockActionClicked
 * @param action
 */
void DockTabWidget::dockActionClicked(DockActionWidget* action)
{
    DockWidget* dock = qobject_cast<DockWidget*>(sender());
    ToolActionController::DOCK_TYPE dt = dock->getDockType();

    switch (dt) {
    case ToolActionController::PARTS:
    {
        QString actionKind = action->getProperty("kind").toString();
        if (action->requiresSubAction()) {
            if (actionKind == "WorkerProcess") {
                openRequiredDock(ToolActionController::FUNCTIONS, actionKind);
            } else {
                openRequiredDock(ToolActionController::DEFINITIONS, actionKind);
            }
        } else {
            toolActionController->addChildNode(actionKind, QPoint(0,0));
        }
        break;
    }
    case ToolActionController::DEFINITIONS:
        //toolbarController->addConnectedChildNode(ID.toInt(), parentKind.toString(), itemPos);
        break;
    case ToolActionController::FUNCTIONS:

        break;
    case ToolActionController::HARDWARE:

        break;
    }
}


/**
 * @brief DockTabWidget::dockBackButtonClicked
 * This is triggered whenever the back button of the definitions or functions dock is clicked.
 * It returns the list of displayed dock items to the adoptable list.
 */
void DockTabWidget::dockBackButtonClicked()
{
    changeDisplayedDockWidget(partsDock);
}


/**
 * @brief DockTabWidget::setupLayout
 */
void DockTabWidget::setupLayout()
{
    partsButton = new QPushButton(this);
    hardwareButton = new QPushButton(this);
    stackedWidget = new QStackedWidget(this);

    partsButton->setToolTip("Parts Dock");
    partsButton->setIconSize(QSize(24,24));
    partsButton->setCheckable(true);
    partsButton->setChecked(true);

    hardwareButton->setToolTip("Hardware Dock");
    hardwareButton->setIconSize(QSize(24,24));
    hardwareButton->setCheckable(true);
    hardwareButton->setChecked(false);

    dockScrollArea = new QScrollArea(this);
    dockScrollArea->setSizeAdjustPolicy(QScrollArea::AdjustToContents);
    dockScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    dockScrollArea->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    dockScrollArea->setWidget(stackedWidget);
    dockScrollArea->setWidgetResizable(true);

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->setMargin(0);
    hLayout->setSpacing(2);
    hLayout->addWidget(partsButton, 1);
    hLayout->addWidget(hardwareButton, 1);

    QVBoxLayout* vLayout = new QVBoxLayout(this);
    vLayout->setMargin(0);
    vLayout->setSpacing(2);
    vLayout->addLayout(hLayout);
    vLayout->addWidget(dockScrollArea, 1);

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

    partsDock->addActions(toolActionController->getAdoptableKindsActions(true));

    QAction* testAction = new QAction(Theme::theme()->getIcon("Actions", "Help"), "", this);
    hardwareDock->addAction(testAction);
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

    connect(partsDock, SIGNAL(actionClicked(DockActionWidget*)), this, SLOT(dockActionClicked(DockActionWidget*)));
    connect(definitionsDock, SIGNAL(backButtonClicked()), this, SLOT(dockBackButtonClicked()));
    connect(functionsDock, SIGNAL(backButtonClicked()), this, SLOT(dockBackButtonClicked()));
}


/**
 * @brief DockTabWidget::openRequiredDock
 * @param dt
 * @param action
 */
void DockTabWidget::openRequiredDock(ToolActionController::DOCK_TYPE dt, QString actionKind)
{
    switch (dt) {
    case ToolActionController::DEFINITIONS:
    {
        QList<NodeViewItemAction*> actions = toolActionController->getDefinitionNodeActions(actionKind);
        definitionsDock->clearDock();
        foreach (NodeViewItemAction* a, actions) {
            definitionsDock->addAction(qobject_cast<QAction*>(a));
        }
        changeDisplayedDockWidget(definitionsDock);
        break;
    }
    case ToolActionController::FUNCTIONS:
        changeDisplayedDockWidget(functionsDock);
        break;
    default:
        break;
    }
}


/**
 * @brief DockTabWidget::changeDisplayedDockWidget
 * @param dockWidget
 */
void DockTabWidget::changeDisplayedDockWidget(DockWidget* dockWidget)
{
    if (dockWidget) {
        stackedWidget->setCurrentWidget(dockWidget);
        stackedWidget->setFixedHeight(dockWidget->sizeHint().height());
    }
}








