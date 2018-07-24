#include "viewmanagerwidget.h"

#include <QVBoxLayout>
#include <QPushButton>

#include "windowitem.h"
#include "dockitem.h"

#include "../../theme.h"

ViewManagerWidget::ViewManagerWidget(WindowManager *manager) : QWidget(0)
{
    setContentsMargins(0,3,0,0);
    setMinimumSize(130,130);

    setupLayout();
    connect(manager, &WindowManager::windowConstructed, this, &ViewManagerWidget::windowConstructed);
    connect(manager, &WindowManager::windowDestructed, this, &ViewManagerWidget::windowDestructed);

    connect(manager, &WindowManager::dockWidgetConstructed, this, &ViewManagerWidget::dockWidgetConstructed);
    connect(manager, &WindowManager::dockWidgetDestructed, this, &ViewManagerWidget::dockWidgetDestructed);

    connect(Theme::theme(), &Theme::theme_Changed, this, &ViewManagerWidget::themeChanged);
    themeChanged();
}

void ViewManagerWidget::themeChanged()
{
    Theme* theme = Theme::theme();
    setStyleSheet(theme->getWidgetStyleSheet("ViewManagerWidget"));
    scrollArea->setStyleSheet("QScrollArea{ background: rgba(0,0,0,0); padding: 2px; border: 0px; }");
}

DockItem *ViewManagerWidget::getDockItem(int ID)
{
    return dockItems.value(ID, 0);
}

void ViewManagerWidget::windowConstructed(BaseWindow *window)
{
    if(window->getType() == BaseWindow::VIEW_WINDOW){
        WindowItem* item = new WindowItem(this, window);

        if(item){
            int ID = window->getID();
            windowItems[ID] = item;
        }
        scrollLayout->insertWidget(scrollLayout->count() -1 , item);
    }
}

void ViewManagerWidget::windowDestructed(int ID)
{
    WindowItem* item = windowItems.value(ID, 0);
    if(item){
        windowItems.remove(ID);
        item->deleteLater();
    }
}

void ViewManagerWidget::dockWidgetConstructed(BaseDockWidget *dockWidget)
{
    DockItem* item = new DockItem(this, dockWidget);

    if(item){
        int ID = dockWidget->getID();
        dockItems[ID] = item;
    }
}

void ViewManagerWidget::dockWidgetDestructed(int ID)
{
    DockItem* item = dockItems.value(ID, 0);
    if(item){
        dockItems.remove(ID);
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


