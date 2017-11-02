#ifndef DOCKWIDGET_DEFAULTDOCKWIDGET_H
#define DOCKWIDGET_DEFAULTDOCKWIDGET_H

#include "basedockwidget.h"

enum class DefaultDockType{NONE, VIEW};

class DefaultDockWidget : public BaseDockWidget
{
    Q_OBJECT
    
    friend class WindowManager;
public:
protected:
    DefaultDockWidget(QString title, QWidget* parent = 0, Qt::DockWidgetArea area = Qt::TopDockWidgetArea, DefaultDockType type = DefaultDockType::NONE);
    ~DefaultDockWidget();
public:
    DefaultDockType getDefaultDockType();
    void themeChanged();
    void setWidget(QWidget *widget);
private:
    DefaultDockType type;

};

#endif // DOCKWIDGET_DEFAULTDOCKWIDGET_H
