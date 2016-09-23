#include "viewmanagerwidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include "../../View/theme.h"

ViewManagerWidget::ViewManagerWidget(MedeaWindowManager *manager) : QWidget(0)
{
    //setContentsMargins(5,5,2,5);
    //setContentsMargins(0,0,0,0);
    setContentsMargins(0,3,0,0);

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
    Theme* theme = Theme::theme();
    setStyleSheet(theme->getWidgetStyleSheet("ViewManagerWidget"));
    //scrollArea->setStyleSheet("QScrollArea{ background: rgba(0,0,0,0); padding: 2px; border: 1px solid " + theme->getDisabledBackgroundColorHex() + ";}");
    scrollArea->setStyleSheet("QScrollArea{ background: rgba(0,0,0,0); padding: 2px; border: 0px; }");
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

void ViewManagerWidget::windowDestructed(int ID)
{
    WindowItem* item = windows.value(ID, 0);
    if(item){
        windows.remove(ID);
        item->deleteLater();
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

void ViewManagerWidget::dockWidgetDestructed(int ID)
{
    DockWindowItem* item = dockWidgets.value(ID, 0);
    if(item){
        dockWidgets.remove(ID);
        item->deleteLater();
    }
}

void ViewManagerWidget::setupLayout()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    windowArea = new QWidget(this);
    windowArea->setStyleSheet("background: rgba(0,0,0,0);");

    scrollLayout = new QVBoxLayout(windowArea);
    scrollLayout->setContentsMargins(0,0,0,0);
    scrollLayout->setSpacing(5);
    layout->setContentsMargins(0,0,0,0);

    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);

    scrollArea->setWidget(windowArea);
    layout->addWidget(scrollArea, 1);
    scrollLayout->addStretch(1);
}



WindowItem::WindowItem(ViewManagerWidget *manager, MedeaWindowNew *window)
{
    this->manager = manager;
    this->window = window;
    setupLayout();


    setContentsMargins(0,0,0,0);

    connect(window, &MedeaWindowNew::dockWidgetAdded, this, &WindowItem::dockWidgetAdded);
    connect(window, &MedeaWindowNew::windowTitleChanged, this, &WindowItem::titleChanged);

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


void WindowItem::dockWidgetAdded(MedeaDockWidget *widget)
{
    if(widget){
        dockContainerLayout->addWidget(manager->getDockWindowItem(widget->getID()));
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
    connect(closeAction, &QAction::triggered, window, &MedeaWindowNew::tryClose);

    closeAction->setEnabled(window != MedeaWindowManager::manager()->getCentralWindow());
    windowToolbar->addAction(closeAction);

    windowToolbar->setIconSize(QSize(16,16));
    windowToolbar->setContentsMargins(0,0,0,0);
    layout->addWidget(windowToolbar);
    layout->addWidget(dockContainer);
    setLayout(layout);

    titleChanged();
}


DockWindowItem::DockWindowItem(ViewManagerWidget *manager, MedeaDockWidget *dockWidget)
{
    this->manager = manager;
    this->dockWidget = dockWidget;

    setFocusPolicy(Qt::ClickFocus);
    setProperty("ID", dockWidget->getID());

    setContentsMargins(0,0,0,0);
    setIconSize(QSize(16,16));
    setupLayout();

    connect(Theme::theme(), &Theme::theme_Changed, this, &DockWindowItem::themeChanged);
    connect(dockWidget, &MedeaDockWidget::dockSetActive, this, &DockWindowItem::themeChanged);
    connect(dockWidget, &MedeaDockWidget::titleChanged, this, &DockWindowItem::titleChanged);
    connect(dockWidget, &MedeaDockWidget::iconChanged, this, &DockWindowItem::updateIcon);
    themeChanged();
}

void DockWindowItem::updateIcon()
{
    if(iconLabel && dockWidget){
        QPair<QString, QString> icon = dockWidget->getIcon();
        iconLabel->setPixmap(Theme::theme()->getImage(icon.first, icon.second, QSize(16,16)));
    }

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

    QWidget* labelWidget = new QWidget(this);
    QHBoxLayout* labelWidgetLayout = new QHBoxLayout(labelWidget);
    labelWidgetLayout->setMargin(0);
    labelWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);
    labelWidgetLayout->addWidget(label);
    labelWidgetLayout->addStretch();


    iconAction = addWidget(iconLabel);
    labelAction = addWidget(labelWidget);

    if(titleBar){
        addActions(titleBar->getToolActions());
    }

    titleChanged();
    updateIcon();
}
