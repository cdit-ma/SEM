#include "docktabwidget.h"
#include "../../theme.h"
#include "../../Views/ContextMenu/contextmenu.h"

#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QTimer>
#include <QScrollBar>

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
    setMinimumSize(130,130);

    auto theme = Theme::theme();
    connect(theme, &Theme::theme_Changed, this, &DockTabWidget::themeChanged);
    themeChanged();
    
    auto action_controller = vc->getActionController();
    addAction(action_controller->dock_addPart);
    addAction(action_controller->dock_deploy);
    connect(action_controller->dock_addPart, &QAction::triggered, parts_action, &QAction::triggered);
    connect(action_controller->dock_deploy, &QAction::triggered, deploy_action, &QAction::triggered);

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
    auto icon_size = theme->getLargeIconSize();
    auto menu_style = new CustomMenuStyle(icon_size.width());

    toolbar->setIconSize(icon_size);

    add_part_menu->setStyle(menu_style);
    add_part_menu->setStyleSheet(theme->getMenuStyleSheet(icon_size.width()) + " QMenu#TOP_LEVEL{background:transparent;} QLabel{color:" + theme->getTextColorHex(ColorRole::DISABLED) + ";} QMenu::item{background:transparent;}");

    deploy_menu->setStyle(menu_style);
    deploy_menu->setStyleSheet(theme->getMenuStyleSheet(icon_size.width()) + " QMenu#TOP_LEVEL{background:transparent;} QLabel{color:" + theme->getTextColorHex(ColorRole::DISABLED) + ";} QMenu::item{background:transparent;}");

    QIcon partIcon;
    partIcon.addPixmap(theme->getImage("Icons", "circlePlusDark", QSize(), theme->getMenuIconColor()));
    partIcon.addPixmap(theme->getImage("Icons", "circlePlusTwoTone", QSize(), theme->getMenuIconColor(ColorRole::SELECTED)), QIcon::Active);
    partIcon.addPixmap(theme->getImage("Icons", "circlePlusDark", QSize(), theme->getMenuIconColor()), QIcon::Normal, QIcon::On);
    partIcon.addPixmap(theme->getImage("Icons", "circlePlusTwoTone", QSize(), theme->getMenuIconColor(ColorRole::SELECTED)), QIcon::Active, QIcon::On);
    parts_action->setIcon(partIcon);

    QIcon hardwareIcon;
    hardwareIcon.addPixmap(theme->getImage("Icons", "screen", QSize(), theme->getMenuIconColor()));
    hardwareIcon.addPixmap(theme->getImage("Icons", "screenTwoTone", QSize(), theme->getMenuIconColor(ColorRole::SELECTED)), QIcon::Active);
    hardwareIcon.addPixmap(theme->getImage("Icons", "screen", QSize(), theme->getMenuIconColor()), QIcon::Normal, QIcon::On);
    hardwareIcon.addPixmap(theme->getImage("Icons", "screenTwoTone", QSize(), theme->getMenuIconColor(ColorRole::SELECTED)), QIcon::Active, QIcon::On);
    deploy_action->setIcon(hardwareIcon);

    auto top_button_style =  "QToolButton {"
                             "border-radius: " + theme->getCornerRadius() + ";"
                             "border-bottom-left-radius: 0px;"
                             "border-bottom-right-radius: 0px;"
                             "}"
                             "QToolButton::checked:!hover {"
                             "background:" + theme->getActiveWidgetBorderColorHex() + ";"
                             "border-color:" + theme->getDisabledBackgroundColorHex() + ";}";

    toolbar->widgetForAction(parts_action)->setStyleSheet(top_button_style);
    toolbar->widgetForAction(deploy_action)->setStyleSheet(top_button_style);

    parts_dock->setStyleSheet(theme->getScrollAreaStyleSheet());
    parts_dock->verticalScrollBar()->setStyleSheet(theme->getScrollBarStyleSheet());
    parts_dock->horizontalScrollBar()->setStyleSheet(theme->getScrollBarStyleSheet());

    deploy_dock->setStyleSheet(theme->getScrollAreaStyleSheet());
    deploy_dock->verticalScrollBar()->setStyleSheet(theme->getScrollBarStyleSheet());
    deploy_dock->horizontalScrollBar()->setStyleSheet(theme->getScrollBarStyleSheet());
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
        deploy_action->setCheckable(true);
        deploy_action->setChecked(false);
    }

    // setup toggle icons for the parts and hardware dock
    Theme::theme()->setIconToggledImage("ToggleIcons", "PartsDock", "Icons", "plus", "Icons", "plus");
    Theme::theme()->setIconToggledImage("ToggleIcons", "HardwareDock", "Icons", "screen", "Icons", "screen");

    auto layout = new QVBoxLayout(this);
    
    layout->setMargin(0);
    layout->setSpacing(2);
    layout->addWidget(toolbar);
    layout->addWidget(stack_widget, 1);
}


/**
 * @brief DockTabWidget::dockActionTriggered
 * @param action
 */
void DockTabWidget::dockActionTriggered(QAction* action)
{
    auto current_menu = action == parts_action ? add_part_menu : deploy_menu;
    auto current_dock = action == parts_action ? parts_dock : deploy_dock;
    auto other = action == parts_action ? deploy_action : parts_action;
    if (action && other) {
        action->setChecked(true);
        other->setChecked(false);
        stack_widget->setCurrentWidget(current_dock);
        current_menu->show();
        refreshSize();
        emit current_menu->aboutToShow();
    }
}


/**
 * @brief DockTabWidget::setupDocks
 */
void DockTabWidget::setupDocks()
{
    add_part_menu = view_controller->getContextMenu()->getAddMenu();
    add_part_menu->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    add_part_menu->setFixedWidth(width());

    deploy_menu = view_controller->getContextMenu()->getDeployMenu();
    deploy_menu->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    deploy_menu->setFixedWidth(width());

    add_part_menu->setObjectName("TOP_LEVEL");
    deploy_menu->setObjectName("TOP_LEVEL");
    
    //Deselect the currently highlight item
    connect(deploy_menu, &QMenu::aboutToHide, [=](){deploy_menu->setActiveAction(0);});
    connect(add_part_menu, &QMenu::aboutToHide, [=](){add_part_menu->setActiveAction(0);});
    
    //Always make these menus visible
    connect(deploy_menu, &QMenu::aboutToHide, deploy_menu, &QMenu::show);
    connect(add_part_menu, &QMenu::aboutToHide, add_part_menu, &QMenu::show);

    parts_dock = new QScrollArea();
    parts_dock->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    parts_dock->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    parts_dock->setWidgetResizable(true);
    parts_dock->setWidget(add_part_menu);
    
    deploy_dock = new QScrollArea();
    deploy_dock->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    deploy_dock->setWidgetResizable(true);
    deploy_dock->setWidget(deploy_menu);

    stack_widget->addWidget(parts_dock);
    stack_widget->addWidget(deploy_dock);

    connect(parts_action, &QAction::triggered, [=](){dockActionTriggered(parts_action);});
    connect(deploy_action, &QAction::triggered, [=](){dockActionTriggered(deploy_action);});

    parts_action->setCheckable(true);
    deploy_action->setCheckable(true);

    installEventFilter(this);
}


/**
 * @brief DockTabWidget::refreshSize
 */
void DockTabWidget::refreshSize()
{
    auto current_menu = stack_widget->currentWidget() == parts_dock ? add_part_menu : deploy_menu;
    auto current_dock = stack_widget->currentWidget() == parts_dock ? parts_dock : deploy_dock;

    auto dock_width = current_dock->viewport()->width();
    
    auto width = current_menu->width();
    auto required_width = current_menu->sizeHint().width();
    
    if (required_width > width){
        //Add the margin in
        setMinimumWidth(required_width + 2);
    }

    current_menu->setFixedWidth(dock_width);
}


/**
 * @brief DockTabWidget::eventFilter
 * @param object
 * @param event
 * @return
 */
bool DockTabWidget::eventFilter(QObject* object, QEvent* event)
{
    if (object && event->type() == QEvent::Resize) {
        refreshSize();
    }
    return false;
}
