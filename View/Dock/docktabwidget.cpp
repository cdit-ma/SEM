#include "docktabwidget.h"
#include "../theme.h"
#include "dockactionwidget.h"

#include <QToolBar>
#include <QVBoxLayout>
#include <QHBoxLayout>

#define TAB_PADDING 20
#define DOCK_SPACING 3
#define DOCK_SEPARATOR_WIDTH 5
#define MIN_WIDTH 110
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
    setStyleSheet("QPushButton{ background:" + theme->QColorToHex(theme->getAltBackgroundColor().darker(150)) + ";}"
                  "QPushButton:hover{ border-width: 2px; }"
                  "QPushButton:hover{ background:" + theme->getHighlightColorHex() + ";}"
                  "QPushButton:hover::checked{ background:" + theme->getHighlightColorHex() + ";}"
                  "QPushButton::checked{ background:" + theme->getAltBackgroundColorHex() + ";}"
                  "QStackedWidget{ border: 1px solid " + theme->getDisabledBackgroundColorHex() + "; background:" + theme->getAltBackgroundColorHex() + ";}"
                  "QScrollArea{ border: 0px; }");

    partsButton->setIcon(theme->getImage("Actions", "Plus", QSize(), theme->getTextColor(theme->CR_NORMAL)));
    hardwareButton->setIcon(theme->getImage("Actions", "Computer", QSize(), theme->getTextColor(theme->CR_NORMAL)));
}


/**
 * @brief DockTabWidget::selectionChanged
 */
void DockTabWidget::selectionChanged()
{

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
    stackedWidget->setCurrentWidget(dockWidget);
    //stackedWidget->resize(dockWidget->sizeHint());
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

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    scrollArea->setWidget(stackedWidget);
    scrollArea->setWidgetResizable(true);

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->setMargin(0);
    hLayout->setSpacing(2);
    hLayout->addWidget(partsButton, 1);
    hLayout->addWidget(hardwareButton, 1);

    QVBoxLayout* vLayout = new QVBoxLayout(this);
    vLayout->setMargin(0);
    vLayout->setSpacing(2);
    vLayout->addLayout(hLayout);
    vLayout->addWidget(scrollArea, 1);

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
        stackedWidget->setCurrentWidget(definitionsDock);
        break;
    }
    case ToolActionController::FUNCTIONS:
        stackedWidget->setCurrentWidget(functionsDock);
        break;
    default:
        break;
    }
}








