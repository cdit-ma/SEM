#include "viewmanagerwidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include "../../View/theme.h"

ViewManagerWidget::ViewManagerWidget(MedeaWindowManager *manager) : QWidget(0)
{
    setMinimumHeight(250);
    setupLayout();
    connect(manager, &MedeaWindowManager::windowConstructed, this, &ViewManagerWidget::windowConstructed);
    connect(manager, &MedeaWindowManager::viewDockWidgetConstructed, this, &ViewManagerWidget::dockWidgetConstructed);

    connect(Theme::theme(), &Theme::theme_Changed, this, &ViewManagerWidget::themeChanged);
}

void ViewManagerWidget::themeChanged()
{
    setStyleSheet(Theme::theme()->getToolBarStyleSheet());
}

DockWindowItem *ViewManagerWidget::getDockWindowItem(int ID)
{
    return dockWidgets.value(ID, 0);
}

WindowItem *ViewManagerWidget::getWindowItem(int ID)
{
    return windows.value(ID, 0);
}

void ViewManagerWidget::windowConstructed(MedeaWindowNew *window)
{
    WindowItem* item = new WindowItem(this, window);

    if(item){
        int ID = window->getID();
        windows[ID] = item;
    }

    scrollLayout->addWidget(item);
}

void ViewManagerWidget::dockWidgetConstructed(MedeaDockWidget *dockWidget)
{
    DockWindowItem* item = new DockWindowItem(this, dockWidget);

    if(item){
        int ID = dockWidget->getID();
        dockWidgets[ID] = item;
    }
}

void ViewManagerWidget::setupLayout()
{
    QVBoxLayout* layout = new QVBoxLayout(this);


    windowArea = new QWidget(this);
    scrollLayout = new QVBoxLayout();
    windowArea->setLayout(scrollLayout);





    scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);

    scrollArea->setWidget(windowArea);
    layout->addWidget(scrollArea, 1);
    setLayout(layout);
}



WindowItem::WindowItem(ViewManagerWidget *manager, MedeaWindowNew *window)
{
    this->manager = manager;
    this->window = window;
    setupLayout();


    connect(window, &MedeaWindowNew::dockWidgetAdded, this, &WindowItem::dockWidgetAdded);
}



void WindowItem::dockWidgetAdded(MedeaDockWidget *widget)
{
    if(widget){
        dockContainerLayout->addWidget(manager->getDockWindowItem(widget->getID()));
    }
}

void WindowItem::setupLayout()
{
    QVBoxLayout* layout = new QVBoxLayout();
    dockContainerLayout = new QVBoxLayout();
    windowToolbar = new QToolBar(this);

    windowToolbar->addWidget(new QLabel("TEST", this));
    dockContainer = new QWidget(this);
    dockContainer->setLayout(dockContainerLayout);

    layout->addWidget(windowToolbar);
    layout->addWidget(dockContainer);
    setLayout(layout);
}


DockWindowItem::DockWindowItem(ViewManagerWidget *manager, MedeaDockWidget *dockWidget)
{
    this->manager = manager;
    this->dockWidget = dockWidget;
    setContentsMargins(0,0,0,0);
    setupLayout();
}

void DockWindowItem::setupLayout()
{
    DockTitleBarWidget * titleBar = dockWidget->getTitleBar();
    if(titleBar){
        //addWidget(new QLabel("DOCK: " + titleBar->getTitle(), this));
        addActions(titleBar->actions());
    }
    setIconSize(QSize(16,16));
}
