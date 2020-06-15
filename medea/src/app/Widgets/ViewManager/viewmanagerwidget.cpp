#include "viewmanagerwidget.h"
#include "windowitem.h"
#include "dockitem.h"

#include <QPushButton>

ViewManagerWidget::ViewManagerWidget(WindowManager *manager)
    : QWidget(nullptr)
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
    return dockItems.value(ID, nullptr);
}

void ViewManagerWidget::windowConstructed(BaseWindow *window)
{
    if (window->getType() == BaseWindow::VIEW_WINDOW) {
        auto item = new WindowItem(this, window);
        windowItems[window->getID()] = item;
        scrollLayout->insertWidget(scrollLayout->count() -1 , item);
    }
}

void ViewManagerWidget::windowDestructed(int ID)
{
    WindowItem* item = windowItems.value(ID, nullptr);
    if (item) {
        windowItems.remove(ID);
        item->deleteLater();
    }
}

void ViewManagerWidget::dockWidgetConstructed(BaseDockWidget *dockWidget)
{
    auto item = new DockItem(this, dockWidget);
    dockItems[dockWidget->getID()] = item;
}

void ViewManagerWidget::dockWidgetDestructed(int ID)
{
    DockItem* item = dockItems.value(ID, nullptr);
    if (item) {
        dockItems.remove(ID);
        item->deleteLater();
    }
}

void ViewManagerWidget::setupLayout()
{
    windowArea = new QWidget(this);
    windowArea->setStyleSheet("background: rgba(0,0,0,0);");

    scrollLayout = new QVBoxLayout(windowArea);
    scrollLayout->setContentsMargins(0,0,0,0);
    scrollLayout->setSpacing(5);
    scrollLayout->addStretch(1);

    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(windowArea);

    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(scrollArea, 1);
}