#include "defaultdockwidget.h"
#include "../../theme.h"
#include <QDebug>

DefaultDockWidget::DefaultDockWidget(QString title, Qt::DockWidgetArea area, DefaultDockType type):BaseDockWidget(BaseDockType::DOCK)
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

DefaultDockWidget::~DefaultDockWidget()
{
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
        widget->setMinimumSize(400, 300);
        BaseDockWidget::setWidget(widget);
    }
}
