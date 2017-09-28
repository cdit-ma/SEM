#include "docktabwidget.h"
#include "../../theme.h"
#include "../../Views/ContextMenu/contextmenu.h"

#include <QToolBar>
#include <QVBoxLayout>
#include <QTimer>
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
    setMinimumSize(130,130);

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

    add_part_menu->setStyleSheet(theme->getMenuStyleSheet(32) + " QMenu#TOP_LEVEL{background:transparent;} QLabel{color:" + theme->getTextColorHex(Theme::CR_DISABLED) + ";}");// QMenu::item{padding: 4px 8px 4px " + QString::number(MENU_ICON_SIZE + 8)  + "px; }"
    deploy_menu->setStyleSheet(theme->getMenuStyleSheet(32) + " QMenu#TOP_LEVEL{background:transparent;} QLabel{color:" + theme->getTextColorHex(Theme::CR_DISABLED) + ";}");// QMenu::item{padding: 4px 8px 4px " + QString::number(MENU_ICON_SIZE + 8)  + "px; }"

    QIcon partIcon;
    partIcon.addPixmap(theme->getImage("Icons", "plus", QSize(), theme->getMenuIconColor()));
    partIcon.addPixmap(theme->getImage("Icons", "plus", QSize(), theme->getMenuIconColor(Theme::CR_SELECTED)), QIcon::Active);

    QIcon hardwareIcon;
    hardwareIcon.addPixmap(theme->getImage("Icons", "screen", QSize(), theme->getMenuIconColor()));
    hardwareIcon.addPixmap(theme->getImage("Icons", "screen", QSize(), theme->getMenuIconColor(Theme::CR_SELECTED)), QIcon::Active);

    parts_action->setIcon(partIcon);
    deploy_action->setIcon(hardwareIcon);

    setStyleSheet(
        theme->getToolBarStyleSheet() +
        "QScrollArea {"
        "border: 1px solid " + theme->getDisabledBackgroundColorHex() + ";"
        "background: rgba(0,0,0,0);"
        "}"
        "QToolButton {"
        "border-radius: " + theme->getCornerRadius() + ";"
        "border-bottom-left-radius: 0px;"
        "border-bottom-right-radius: 0px;"
        "}"
        "QToolButton::checked:!hover {"
        "background:" + theme->getActiveWidgetBorderColorHex() + ";"
        "border-color:" + theme->getDisabledBackgroundColorHex() + ";"
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
        deploy_action->setCheckable(true);
        deploy_action->setChecked(false);
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
    //add_part_menu->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    add_part_menu->setFixedWidth(width());

    deploy_menu = view_controller->getContextMenu()->getDeployMenu();
    deploy_menu->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    //deploy_menu->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
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
    //parts_dock->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    parts_dock->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    parts_dock->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    parts_dock->setWidgetResizable(true);
    parts_dock->setWidget(add_part_menu);
    //parts_dock->setAlignment(Qt::AlignHCenter);
    
    deploy_dock = new QScrollArea();
    deploy_dock->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    //deploy_dock->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    deploy_dock->setWidgetResizable(true);
    deploy_dock->setWidget(deploy_menu);
    //deploy_dock->setAlignment(Qt::AlignHCenter);
    

    stack_widget->addWidget(parts_dock);
    stack_widget->addWidget(deploy_dock);

    connect(parts_action, &QAction::triggered, [=](){dockActionTriggered(parts_action);});
    connect(deploy_action, &QAction::triggered, [=](){dockActionTriggered(deploy_action);});

    parts_action->setCheckable(true);
    deploy_action->setCheckable(true);

    installEventFilter(this);
}

void DockTabWidget::refreshSize()
{
    auto current_menu = stack_widget->currentWidget() == parts_dock ? add_part_menu : deploy_menu;
    auto current_dock =stack_widget->currentWidget() == parts_dock ? parts_dock : deploy_dock;

    auto dock_width = current_dock->viewport()->width();
    
    auto width = current_menu->width();
    auto required_width = current_menu->sizeHint().width();
    
    if(required_width > width){
        //Add the margin in
        setMinimumWidth(required_width + 2);   
    }

    current_menu->setFixedWidth(dock_width);
}

bool DockTabWidget::eventFilter(QObject *object, QEvent *event)
{
    if (object && event->type() == QEvent::Resize) {
        refreshSize();
    }
    return false;
}
