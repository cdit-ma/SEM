#include "viewwindow.h"
#include "../DockWidgets/viewdockwidget.h"
#include "../../Controllers/WindowManager/windowmanager.h"

#include <QStringBuilder>
#include <QMenu>

ViewWindow::ViewWindow(BaseWindow* parent_window)
    : BaseWindow(parent_window, BaseWindow::VIEW_WINDOW)
{
    setAcceptDrops(true);
    setDockNestingEnabled(true);
    setContextMenuPolicy(Qt::CustomContextMenu);

    //Setup Tab positions
    setTabPosition(Qt::RightDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::TopDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::BottomDockWidgetArea, QTabWidget::West);
}

void ViewWindow::addDockWidget(Qt::DockWidgetArea area, QDockWidget *widget, Qt::Orientation orientation)
{
    // INSPECT: The comment below said to only allow View Dock Widgets
    //  Does that mean that the widget below should be casted to a ViewDockWidget instead?
    auto default_dock_widget = qobject_cast<DefaultDockWidget*>(widget);

    //Only allow View Dock Widgets.
    BaseWindow::addDockWidget(area, widget, orientation);
}