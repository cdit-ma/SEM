#include "docktabwidget.h"
#include "../../theme.h"
#include "../../Views/ContextToolbar/contextmenu.h"

#include <QToolBar>
#include <QVBoxLayout>
#include <QToolButton>

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

    auto theme = Theme::theme();
    connect(theme, &Theme::theme_Changed, this, &DockTabWidget::themeChanged);
    themeChanged();
    
    connect(vc->getSelectionController(), &SelectionController::selectionChanged, this, &DockTabWidget::refreshSize);
    
    dockActionTriggered(parts_action);
    refreshSize();
}


/**
 * @brief DockTabWidget::themeChanged
 */
void DockTabWidget::themeChanged()
{
    Theme* theme = Theme::theme();

    add_part_menu->setStyleSheet(theme->getMenuStyleSheet(32) + " QMenu{background:transparent;font-size:10pt;} QLabel{color:" + theme->getTextColorHex(Theme::CR_DISABLED) + ";}");// QMenu::item{padding: 4px 8px 4px " + QString::number(MENU_ICON_SIZE + 8)  + "px; }"
    deploy_menu->setStyleSheet(theme->getMenuStyleSheet(32) + " QMenu{background:transparent;font-size:10pt;} QLabel{color:" + theme->getTextColorHex(Theme::CR_DISABLED) + ";}");// QMenu::item{padding: 4px 8px 4px " + QString::number(MENU_ICON_SIZE + 8)  + "px; }"

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

        emit current_menu->aboutToShow();
        current_menu->show();
    }
}

/**
 * @brief DockTabWidget::setupDocks
 */
void DockTabWidget::setupDocks()
{
    add_part_menu = view_controller->getContextMenu()->getAddMenu();
    add_part_menu->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    add_part_menu->setFixedWidth(width());

    deploy_menu = view_controller->getContextMenu()->getDeployMenu();
    deploy_menu->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    deploy_menu->setFixedWidth(width());

    //Always make these menus visible
    connect(deploy_menu, &QMenu::aboutToHide, deploy_menu, &QMenu::show);
    connect(add_part_menu, &QMenu::aboutToHide, add_part_menu, &QMenu::show);

    parts_dock = new QScrollArea();
    parts_dock->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    parts_dock->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    parts_dock->setWidgetResizable(true);
    parts_dock->setWidget(add_part_menu);
    parts_dock->setAlignment(Qt::AlignHCenter);
    
    deploy_dock = new QScrollArea();
    deploy_dock->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    deploy_dock->setWidgetResizable(true);
    deploy_dock->setWidget(deploy_menu);
    deploy_dock->setAlignment(Qt::AlignHCenter);
    

    stack_widget->addWidget(parts_dock);
    stack_widget->addWidget(deploy_dock);

    connect(parts_action, &QAction::triggered, [=](){dockActionTriggered(parts_action);});
    connect(deploy_action, &QAction::triggered, [=](){dockActionTriggered(deploy_action);});

    parts_action->setCheckable(true);
    deploy_action->setCheckable(true);

    installEventFilter(this);
}

void DockTabWidget::refreshSize(){
    add_part_menu->setFixedWidth(parts_dock->viewport()->size().width() - 8);
    deploy_menu->setFixedWidth(deploy_dock->viewport()->size().width() - 8);
}

bool DockTabWidget::eventFilter(QObject *object, QEvent *event)
{
    if (object && event->type() == QEvent::Resize) {
        refreshSize();
    }
    return false;
}