#include "docktabwidget.h"
#include "../../theme.h"
#include "../../Views/ContextToolbar/contextmenu.h"

#include <QToolBar>
#include <QVBoxLayout>

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
    view_controller = vc;

    setupLayout();
    setupDocks();

    connect(Theme::theme(), &Theme::theme_Changed, this, &DockTabWidget::themeChanged);

    auto theme = Theme::theme();
    theme->setIconToggledImage("DockIcons", "PartsDock", "Icons", "plus", "Icons", "eyeStriked", false);
    theme->setIconToggledImage("DockIcons", "DeployDock", "Icons", "screen", "Icons", "eyeStriked", false);
    themeChanged();
    refreshSize();

    connect(vc->getSelectionController(), &SelectionController::selectionChanged, [=](){refreshSize();});

    dockActionTriggered(parts_action);
}


/**
 * @brief DockTabWidget::themeChanged
 */
void DockTabWidget::themeChanged()
{
    Theme* theme = Theme::theme();
    /*setStyleSheet(theme->getToolBarStyleSheet() +
                  "QWidget{ color:" + theme->getTextColorHex() + ";}"
                  "QToolButton {"
                  "background:" + theme->getAltBackgroundColorHex() + ";"
                  "border-color:" + theme->getBackgroundColorHex() + ";"
                  "border-radius: " + theme->getSharpCornerRadius() + ";"
                  "}"
                  "QToolButton#DOCK_BUTTON {"
                  "border-radius: " + theme->getCornerRadius() + ";"
                  "border-bottom-left-radius: 0px;"
                  "border-bottom-right-radius: 0px;"
                  "}"
                  "QToolButton::checked:!hover {"
                  "background:" + theme->getActiveWidgetBorderColorHex() + ";"
                  "border-color:" + theme->getDisabledBackgroundColorHex() + ";"
                  "}"
                  "QStackedWidget{ border: 0px; background:" + theme->getBackgroundColorHex() + ";}");
    */
    parts_action->setIcon(theme->getIcon("Icons", "plus"));
    deploy_action->setIcon(theme->getIcon("Icons", "screen"));

    setStyleSheet(
        theme->getToolBarStyleSheet() +
        "QScrollArea {background: rgba(0,0,0,0);} "
        "QToolButton{"
        "border-radius: " + theme->getCornerRadius() + ";"
        "border-bottom-left-radius: 0px;"
        "border-bottom-right-radius: 0px;"
        
        "}"
    );
}

/**
 * @brief DockTabWidget::setupLayout
 */
void DockTabWidget::setupLayout()
{
    stack_widget = new QStackedWidget(this);

    toolbar = new QToolBar(this);
    toolbar->setStyleSheet("QToolBar{ spacing: 2px; padding: 2px; }");
    {
        auto button = new QToolButton(this);
        button->setToolButtonStyle(Qt::ToolButtonIconOnly);
        button->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        parts_action = toolbar->addWidget(button);
        //Connect the button to it's action so we don't need to worry about QToolButton stuff
        button->setDefaultAction(parts_action);
        parts_action->setCheckable(true);
        parts_action->setChecked(false);
    }

    {
        auto button = new QToolButton(this);
        button->setToolButtonStyle(Qt::ToolButtonIconOnly);
        button->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        deploy_action = toolbar->addWidget(button);
        //Connect the button to it's action so we don't need to worry about QToolButton stuff
        button->setDefaultAction(deploy_action);
        parts_action->setCheckable(true);
        parts_action->setChecked(false);
    }

    toolbar->setIconSize(QSize(32,32));


    auto layout = new QVBoxLayout(this);
    
    layout->setMargin(0);
    layout->setSpacing(2);
    layout->addWidget(toolbar);
    layout->addWidget(stack_widget, 1);

   
}


void DockTabWidget::dockActionTriggered(QAction* action){
    auto current_menu = action == parts_action ? add_part_menu : deploy_menu;
    auto current_dock = action == parts_action ? parts_dock : deploy_dock;
    auto other = action == parts_action ? deploy_action : parts_action;
    if(action && other){
        action->setChecked(true);
        other->setChecked(false);
        stack_widget->setCurrentWidget(current_dock);
        current_menu->show();
    }
}

/**
 * @brief DockTabWidget::setupDocks
 */
void DockTabWidget::setupDocks()
{
    add_part_menu = view_controller->getContextMenu()->getAddMenu();
    add_part_menu->setFixedWidth(width());
    add_part_menu->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    //add_part_menu->setFixedWidth(130);

    deploy_menu = view_controller->getContextMenu()->getDeployMenu();
    deploy_menu->setFixedWidth(width());
    deploy_menu->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(deploy_menu, &QMenu::aboutToHide, deploy_menu, &QMenu::show);
    connect(add_part_menu, &QMenu::aboutToHide, add_part_menu, &QMenu::show);
    //deploy_menu->setMinimumWidth(130);

    parts_dock = new QScrollArea();
    parts_dock->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    parts_dock->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    parts_dock->setWidgetResizable(true);
    parts_dock->setWidget(add_part_menu);
    parts_dock->setAlignment(Qt::AlignHCenter);
    
    deploy_dock = new QScrollArea();
    //deploy_dock->setMargins(0);
    deploy_dock->setContentsMargins(0,0,0,0);
    deploy_dock->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //deploy_dock->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    deploy_dock->setWidgetResizable(true);
    deploy_dock->setWidget(deploy_menu);
    deploy_dock->setAlignment(Qt::AlignHCenter);
    

    //deploy_menu->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    stack_widget->addWidget(parts_dock);
    stack_widget->addWidget(deploy_dock);
    connect(parts_action, &QAction::triggered, [=](){dockActionTriggered(parts_action);});
    connect(deploy_action, &QAction::triggered, [=](){dockActionTriggered(deploy_action);});

    parts_action->setCheckable(true);
    deploy_action->setCheckable(true);

    installEventFilter(this);
}

void DockTabWidget::refreshSize(){
    add_part_menu->setFixedWidth(parts_dock->viewport()->size().width());
    deploy_menu->setFixedWidth(deploy_dock->viewport()->size().width());
}

bool DockTabWidget::eventFilter(QObject *object, QEvent *event)
{
    if (object && event->type() == QEvent::Resize) {
        refreshSize();
    }
    return false;
}