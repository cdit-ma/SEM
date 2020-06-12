#include "viewdockwidget.h"
#include "../../Controllers/WindowManager/windowmanager.h"

ViewDockWidget::ViewDockWidget(const QString& title, QWidget* parent, Qt::DockWidgetArea area)
    : DefaultDockWidget(title, parent, area, DefaultDockType::VIEW) {}

SelectionHandler* ViewDockWidget::getSelectionHandler()
{
    if (nodeView) {
        return &nodeView->getSelectionHandler();
    }
    return nullptr;
}

NodeView* ViewDockWidget::getNodeView()
{
    return nodeView;
}

/**
 * @brief ViewDockWidget::setWidget
 * @param widget
 * @throws std::invalid_argument
 */
void ViewDockWidget::setWidget(QWidget *widget)
{
    auto view = qobject_cast<NodeView*>(widget);
    if (view) {
        nodeView = view;
        connect(nodeView, SIGNAL(destroyed(QObject*)), this, SLOT(destruct()));
        DefaultDockWidget::setWidget(widget);
    } else {
        throw std::invalid_argument("Invalid widget: ViewDockWidget requires a NodeView as its widget");
    }
}