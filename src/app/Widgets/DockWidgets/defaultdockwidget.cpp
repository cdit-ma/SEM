#include "defaultdockwidget.h"
#include "../../theme.h"

DefaultDockWidget::DefaultDockWidget(const QString& title, QWidget* parent, Qt::DockWidgetArea area, DefaultDockType type)
    : BaseDockWidget(BaseDockType::DOCK, parent)
{
    setTitle(title);
    setDockWidgetArea(area);

    this->type = type;

    setCloseVisible(true);
    setMaximizeVisible(true);
    setPopOutVisible(true);
    setHideVisible(true);

    setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
}

DefaultDockType DefaultDockWidget::getDefaultDockType()
{
    return type;
}

void DefaultDockWidget::themeChanged()
{
    qDebug() << "DefaultDockWidget::themeChanged";
    setStyleSheet(Theme::theme()->getViewDockWidgetStyleSheet(isActive()));
    BaseDockWidget::themeChanged();
}