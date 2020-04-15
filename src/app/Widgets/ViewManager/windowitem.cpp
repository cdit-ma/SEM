#include "windowitem.h"
#include "dockitem.h"
#include "../../theme.h"
#include "../../Widgets/DockWidgets/defaultdockwidget.h"

WindowItem::WindowItem(ViewManagerWidget *manager, BaseWindow *window)
{
    this->manager = manager;
    this->window = window;
    setupLayout();


    setContentsMargins(0,0,0,0);

    connect(window, &BaseWindow::dockWidgetAdded, this, &WindowItem::dockWidgetAdded);
    connect(window, &BaseWindow::windowTitleChanged, this, &WindowItem::titleChanged);

    connect(Theme::theme(), &Theme::theme_Changed, this, &WindowItem::themeChanged);

    themeChanged();
}

WindowItem::~WindowItem()
{
    //Unset all items.
}

void WindowItem::themeChanged()
{
    auto theme = Theme::theme();

    windowToolbar->setIconSize(theme->getIconSize());
    windowToolbar->setStyleSheet(theme->getDockTitleBarStyleSheet(false, "#WINDOW_TOOLBAR") +
                                 "#WINDOW_TOOLBAR{ border: 1px solid " + Theme::theme()->getAltBackgroundColorHex() + "; background:" + Theme::theme()->getDisabledBackgroundColorHex() + "; }"
                                 "#WINDOW_TOOLBAR QToolButton::!hover{ background: rgba(0,0,0,0); }");

    closeAction->setIcon(theme->getIcon("Icons", "cross"));

    label->setStyleSheet(theme->getLabelStyleSheet());
}

void WindowItem::titleChanged(QString)
{
    label->setText(window->windowTitle());
}

void WindowItem::dockWidgetAdded(BaseDockWidget *widget)
{
    if(widget){
        if(widget->getBaseDockType() == BaseDockType::DOCK){
            if(WindowManager::manager()->isViewDockWidget(widget)){
                viewContainerLayout->addWidget(manager->getDockItem(widget->getID()));
            }else{
                toolContainerLayout->addWidget(manager->getDockItem(widget->getID()));
            }
        }
    }
}

void WindowItem::setupLayout()
{
    QVBoxLayout* layout = new QVBoxLayout();
    //layout->setContentsMargins(5,5,5,5);
    layout->setSpacing(5);
    layout->setMargin(0);

    viewContainerLayout = new QVBoxLayout();
    viewContainerLayout->setContentsMargins(5,0,5,0);
    viewContainerLayout->setSpacing(5);
    toolContainerLayout = new QVBoxLayout();
    toolContainerLayout->setContentsMargins(5,0,5,0);
    toolContainerLayout->setSpacing(5);
    windowToolbar = new QToolBar(this);
    windowToolbar->setObjectName("WINDOW_TOOLBAR");

    label = new QLabel(this);

    QWidget* labelWidget = new QWidget(this);
    QHBoxLayout* labelWidgetLayout = new QHBoxLayout(labelWidget);
    labelWidgetLayout->setMargin(0);
    labelWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);
    labelWidgetLayout->addWidget(label);
    labelWidgetLayout->addStretch();

    dockContainer = new QWidget(this);
    QVBoxLayout* combineLayout = new QVBoxLayout();
    combineLayout->setContentsMargins(0,0,0,0);
    combineLayout->setSpacing(10);
    combineLayout->addLayout(viewContainerLayout);
    combineLayout->addLayout(toolContainerLayout);
    dockContainer->setLayout(combineLayout);

    closeAction = new QAction(this);
    closeAction->setEnabled(window != WindowManager::manager()->getCentralWindow());
    connect(closeAction, &QAction::triggered, window, &BaseWindow::tryClose);

    windowToolbar->addWidget(labelWidget);
    windowToolbar->addAction(closeAction);
    windowToolbar->setContentsMargins(0,0,0,0);

    layout->addWidget(windowToolbar);
    layout->addWidget(dockContainer);
    setLayout(layout);

    titleChanged();
}
