#include "defaultdockwidget.h"
#include "../../theme.h"
#include <QDebug>

DefaultDockWidget::DefaultDockWidget(QString title, QWidget* parent, Qt::DockWidgetArea area, DefaultDockType type):BaseDockWidget(BaseDockType::DOCK, parent)
{
    setTitle(title);
    setDockWidgetArea(area);

    this->type = type;
    setCloseVisible(true);
    setFocusEnabled(true);
    setMaximizeVisible(true);
    setPopOutVisible(true);
    setHideVisible(true);

    setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
    themeChanged();
}

DefaultDockType DefaultDockWidget::getDefaultDockType(){
    return type;
}

void DefaultDockWidget::themeChanged()
{
    setStyleSheet(Theme::theme()->getViewDockWidgetStyleSheet(isActive()));
    BaseDockWidget::themeChanged();
}

void DefaultDockWidget::setWidget(QWidget *widget)
{
    if (widget) {
        BaseDockWidget::setWidget(widget);
    }
}
