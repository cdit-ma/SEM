#include "windowitem.h"
#include "dockitem.h"

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

void WindowItem::titleChanged(const QString&)
{
    label->setText(window->windowTitle());
}

void WindowItem::dockWidgetAdded(BaseDockWidget* widget)
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
    auto layout = new QVBoxLayout(this);
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
    windowToolbar->setContentsMargins(0,0,0,0);

    label = new QLabel(this);

    auto labelWidget = new QWidget(this);
    auto labelWidgetLayout = new QHBoxLayout(labelWidget);
    labelWidgetLayout->setMargin(0);
    labelWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);
    labelWidgetLayout->addWidget(label);
    labelWidgetLayout->addStretch();
    windowToolbar->addWidget(labelWidget);

    dockContainer = new QWidget(this);
    auto combineLayout = new QVBoxLayout(dockContainer);
    combineLayout->setContentsMargins(0,0,0,0);
    combineLayout->setSpacing(10);
    combineLayout->addLayout(viewContainerLayout);
    combineLayout->addLayout(toolContainerLayout);

    closeAction = new QAction(this);
    closeAction->setEnabled(window != WindowManager::manager()->getCentralWindow());
    connect(closeAction, &QAction::triggered, window, &BaseWindow::tryClose);
    windowToolbar->addAction(closeAction);

    layout->addWidget(windowToolbar);
    layout->addWidget(dockContainer);
    titleChanged();
}