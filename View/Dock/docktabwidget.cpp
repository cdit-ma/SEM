#include "docktabwidget.h"
#include "../theme.h"
#include "dockactionwidget.h"

#include <QToolBar>
#include <QVBoxLayout>
#include <QHBoxLayout>

#define TAB_PADDING 20
#define DOCK_SPACING 3
#define DOCK_SEPARATOR_WIDTH 5

/**
 * @brief DockTabWidget::DockTabWidget
 * @param parent
 */
DockTabWidget::DockTabWidget(ViewController *vc, QWidget* parent) : QWidget(parent)
{
    viewController = vc;

    setupLayout();
    themeChanged();
    selectionChanged();

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    connect(viewController->getSelectionController(), SIGNAL(selectionChanged(int)), this, SLOT(selectionChanged()));
}


/**
 * @brief DockTabWidget::themeChanged
 */
void DockTabWidget::themeChanged()
{
    Theme* theme = Theme::theme();

    partsButton->setIcon(theme->getIcon("Actions", "Plus"));
    hardwareButton->setIcon(theme->getIcon("Actions", "Computer"));
}


/**
 * @brief DockTabWidget::selectionChanged
 */
void DockTabWidget::selectionChanged()
{
    updateDockHeight();
}


/**
 * @brief DockTabWidget::setupLayout
 */
void DockTabWidget::setupLayout()
{
    partsButton = new QPushButton(this);
    hardwareButton = new QPushButton(this);
    stackedWidget = new QStackedWidget(this);

    partsButton->setCheckable(true);
    hardwareButton->setCheckable(true);
    partsButton->setIconSize(QSize(24,24));
    hardwareButton->setIconSize(QSize(24,24));
    //stackedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

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
    setMinimumWidth(60);
    //setupDocks();
}


/**
 * @brief DockTabWidget::setupDocks
 */
void DockTabWidget::setupDocks()
{
    /*
    partsDock = new QToolBar();
    partsDock->setIconSize(QSize(54,54));
    partsDock->setOrientation(Qt::Vertical);
    //partsDock->addActions(viewController->getToolbarController()->getAdoptableKindsActions(true));
    */

    /*
     * This does what I want =D
     */
    QWidget* w = new QWidget(this);
    w->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    QVBoxLayout* vl = new QVBoxLayout(w);

    foreach (QAction* action, viewController->getToolbarController()->getAdoptableKindsActions(true)) {
        DockActionWidget* dockAction = new DockActionWidget(action, this);
        //partsDock->addWidget(dockAction);
        vl->addWidget(dockAction, 1, Qt::AlignHCenter);
    }

    /*
    QAction* action = new QAction(Theme::theme()->getIcon("Actions", "Parts"), "Parts", this);
    DockActionWidget* a = new DockActionWidget(action, this);
    partsDock->addWidget(a);

    partsDock->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    partsDock->setStyleSheet("QToolButton{ width: 67px; }");
    */

    //stackedWidget->addWidget(partsDock);
    stackedWidget->addWidget(w);

    /*
    dockStack = new QStackedWidget(this);
    dockStack->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    dockStack->addWidget(partsDock);
    dockStack->setStyleSheet("QStackedWidget{ margin: 0px 0px 0px 15px; }");

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setWidget(dockStack);
    scrollArea->setWidgetResizable(true);
    scrollArea->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    //addTab(scrollArea, "");
    */
}


/**
 * @brief DockTabWidget::updateDockHeight
 */
void DockTabWidget::updateDockHeight()
{
    return;
    int totalHeight = 0;
    foreach (QAction* action, partsDock->actions()) {
        if (action->isVisible()) {
            // apparently the action's geometry changes for some reason; most consistent height is 67
            //qDebug() << action->text() << " height: " << partsDock->actionGeometry(action).height();
            totalHeight += 67;
            totalHeight += DOCK_SPACING;
        }
    }
    //partsDock->setFixedHeight(totalHeight);
    partsDock->setFixedSize(partsDock->sizeHint().width(), totalHeight + 100);
}









