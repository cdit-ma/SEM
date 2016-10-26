#include "windowitem.h"
#include "dockitem.h"
#include "../../theme.h"

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
    windowToolbar->setStyleSheet(Theme::theme()->getDockTitleBarStyleSheet(false, "#WINDOW_TOOLBAR") +
                                 //"#WINDOW_TOOLBAR{ border: 1px solid " + Theme::theme()->getAltBackgroundColorHex() + "; background: rgba(0,0,0,0); }"
                                 "#WINDOW_TOOLBAR{ border: 1px solid " + Theme::theme()->getAltBackgroundColorHex() + "; background:" + Theme::theme()->getDisabledBackgroundColorHex() + "; }"
                                 "#WINDOW_TOOLBAR QToolButton::!hover{ background: rgba(0,0,0,0); }");

    closeAction->setIcon(Theme::theme()->getIcon("Actions", "Close"));

}

void WindowItem::titleChanged(QString)
{
    label->setText(window->windowTitle());
}


void WindowItem::dockWidgetAdded(BaseDockWidget *widget)
{
    if(widget){
        dockContainerLayout->addWidget(manager->getDockItem(widget->getID()));
    }
}

void WindowItem::setupLayout()
{
    QVBoxLayout* layout = new QVBoxLayout();
    //layout->setContentsMargins(5,5,5,5);
    layout->setSpacing(5);
    layout->setMargin(0);

    dockContainerLayout = new QVBoxLayout();
    dockContainerLayout->setContentsMargins(10,0,0,0);
    dockContainerLayout->setSpacing(5);
    windowToolbar = new QToolBar(this);
    windowToolbar->setObjectName("WINDOW_TOOLBAR");

    label = new QLabel(this);

    QWidget* labelWidget = new QWidget(this);
    QHBoxLayout* labelWidgetLayout = new QHBoxLayout(labelWidget);
    labelWidgetLayout->setMargin(0);
    labelWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);
    labelWidgetLayout->addWidget(label);
    labelWidgetLayout->addStretch();




    windowToolbar->addWidget(labelWidget);

    dockContainer = new QWidget(this);
    dockContainer->setLayout(dockContainerLayout);


    closeAction = 0;
    closeAction = new QAction(this);
    connect(closeAction, &QAction::triggered, window, &BaseWindow::tryClose);

    closeAction->setEnabled(window != WindowManager::manager()->getCentralWindow());
    windowToolbar->addAction(closeAction);

    windowToolbar->setIconSize(QSize(16,16));
    windowToolbar->setContentsMargins(0,0,0,0);
    layout->addWidget(windowToolbar);
    layout->addWidget(dockContainer);
    setLayout(layout);

    titleChanged();
}
