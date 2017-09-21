#include "invisibledockwidget.h"
#include "../ViewManager/viewmanagerwidget.h"
#include "../ViewManager/windowitem.h"
#include "../../theme.h"

InvisibleDockWidget::InvisibleDockWidget(QString title) : BaseDockWidget(BaseDockType::INVISIBLE)
{
    removeTitleBar();
    setTitleBarWidget(new QWidget(this));
    setTitle(title);
}

InvisibleDockWidget::~InvisibleDockWidget()
{
}

void InvisibleDockWidget::themeChanged()
{
}

void InvisibleDockWidget::connectChildDockWidget(BaseDockWidget* dockWidget)
{
    if (dockWidget) {
        connect(dockWidget, &BaseDockWidget::visibilityChanged, this, &InvisibleDockWidget::childDockWidgetVisibilityChanged);
    }
}

void InvisibleDockWidget::childDockWidgetVisibilityChanged(bool visible)
{
    return;
    if (visible) {
        if (!isVisible()) {
            setVisible(true);
        }
    } else {
        if (window) {
            auto children = window->getDockWidgets();
            bool hideParentDockWidget = true;
            for (auto child : children) {
                if (child->isVisible()) {
                    hideParentDockWidget = false;
                    break;
                }
            }
            setVisible(!hideParentDockWidget);
        }
    }
}

void InvisibleDockWidget::setWidget(QWidget* widget)
{
    auto baseWindow = qobject_cast<BaseWindow*>(widget);
    if (baseWindow) {
        window = baseWindow;
        for (auto childDockWidget : window->getDockWidgets()) {
            connectChildDockWidget(childDockWidget);
        }
        connect(window, &BaseWindow::dockWidgetAdded, this, &InvisibleDockWidget::connectChildDockWidget);

        // hide the invisible dock's window in the view manager so that it can't be closed which in turn deletes the invisible dock
        auto windowItem = WindowManager::manager()->getViewManagerGUI()->getWindowItem(window->getID());
        if (windowItem) {
            windowItem->setVisible(false);
        }
    }
    BaseDockWidget::setWidget(widget);
}
