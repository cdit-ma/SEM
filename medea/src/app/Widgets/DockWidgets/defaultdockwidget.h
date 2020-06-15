#ifndef DOCKWIDGET_DEFAULTDOCKWIDGET_H
#define DOCKWIDGET_DEFAULTDOCKWIDGET_H

#include "basedockwidget.h"

enum class DefaultDockType{NONE, VIEW};

class DefaultDockWidget : public BaseDockWidget
{
    Q_OBJECT
    friend class WindowManager;

protected:
    explicit DefaultDockWidget(const QString& title, QWidget* parent = nullptr, Qt::DockWidgetArea area = Qt::TopDockWidgetArea, DefaultDockType type = DefaultDockType::NONE);

public:
    DefaultDockType getDefaultDockType();

    void themeChanged() override;

private:
    DefaultDockType type;
};

#endif // DOCKWIDGET_DEFAULTDOCKWIDGET_H