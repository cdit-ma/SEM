#include "viewmanagerwidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include "../../View/theme.h"

ViewManagerWidget::ViewManagerWidget(MedeaWindowManager *manager) : QWidget(0)
{
    setMinimumHeight(200);
    setContentsMargins(0,0,0,0);

    setupLayout();
    connect(manager, &MedeaWindowManager::windowConstructed, this, &ViewManagerWidget::windowConstructed);
    connect(manager, &MedeaWindowManager::windowDestructed, this, &ViewManagerWidget::windowDestructed);

    connect(manager, &MedeaWindowManager::viewDockWidgetConstructed, this, &ViewManagerWidget::dockWidgetConstructed);
    connect(manager, &MedeaWindowManager::viewDockWidgetDestructed, this, &ViewManagerWidget::dockWidgetDestructed);

    connect(Theme::theme(), &Theme::theme_Changed, this, &ViewManagerWidget::themeChanged);
    themeChanged();
}

void ViewManagerWidget::themeChanged()
{
    //setStyleSheet("QScrollArea{background: red;}");
    //windowArea->setStyleSheet(Theme::theme()->getWidgetStyleSheet());
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
    if(window->getType() == MedeaWindowNew::VIEW_WINDOW){
        WindowItem* item = new WindowItem(this, window);

        if(item){
            int ID = window->getID();
            windows[ID] = item;
        }
        scrollLayout->insertWidget(scrollLayout->count() -1 , item);
    }
}

void ViewManagerWidget::windowDestructed(MedeaWindowNew *window)
{
    if(window){
        int ID = window->getID();
        WindowItem* item = windows.value(ID, 0);

        if(item){
            item->deleteLater();
        }
    }

}

void ViewManagerWidget::dockWidgetConstructed(MedeaDockWidget *dockWidget)
{
    DockWindowItem* item = new DockWindowItem(this, dockWidget);

    if(item){
        int ID = dockWidget->getID();
        dockWidgets[ID] = item;
    }
}

void ViewManagerWidget::dockWidgetDestructed(MedeaDockWidget *dockWidget)
{
    if(dockWidget){
        int ID = dockWidget->getID();
        DockWindowItem* item = dockWidgets.value(ID, 0);

        if(item){
            item->deleteLater();
        }
    }
}

void ViewManagerWidget::setupLayout()
{
    QVBoxLayout* layout = new QVBoxLayout(this);


    windowArea = new QWidget(this);

    scrollLayout = new QVBoxLayout();
    windowArea->setLayout(scrollLayout);
    scrollLayout->setContentsMargins(0,0,0,0);
    //windowArea->setContentsMargins(0,0,0,0);
    layout->setContentsMargins(0,0,0,0);



    scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);

    scrollArea->setWidget(windowArea);
    layout->addWidget(scrollArea, 1);
    scrollLayout->addStretch(1);
    setLayout(layout);
}



WindowItem::WindowItem(ViewManagerWidget *manager, MedeaWindowNew *window)
{
    this->manager = manager;
    this->window = window;
    setupLayout();


    setContentsMargins(0,0,0,0);

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

    windowToolbar->addWidget(new QLabel(window->windowTitle(), this));
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

    setFocusPolicy(Qt::ClickFocus);
    setFocusProxy(dockWidget);

    setContentsMargins(0,0,0,0);
    setupLayout();

    connect(Theme::theme(), &Theme::theme_Changed, this, &DockWindowItem::themeChanged);
    connect(dockWidget, &MedeaDockWidget::dockSetActive, this, &DockWindowItem::themeChanged);
    connect(dockWidget, &MedeaDockWidget::titleChanged, this, &DockWindowItem::titleChanged);
    themeChanged();
}

void DockWindowItem::themeChanged()
{
    setStyleSheet(Theme::theme()->getDockTitleBarStyleSheet(dockWidget->isActive(), "DockWindowItem"));
}

void DockWindowItem::titleChanged()
{
    label->setText(dockWidget->getTitle());
}

void DockWindowItem::setupLayout()
{
    DockTitleBarWidget * titleBar = dockWidget->getTitleBar();

    iconLabel = new QLabel(this);
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setStyleSheet("margin-right: 2px;");

    label = new QLabel(this);
    label->setText(titleBar->getTitle());
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    iconAction = addWidget(iconLabel);
    iconAction->setVisible(false);

    labelAction = addWidget(label);

    if(titleBar){
        addActions(titleBar->getToolActions());
    }
    setIconSize(QSize(16,16));
}
