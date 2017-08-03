#ifndef VIEWWINDOW_H
#define VIEWWINDOW_H

#include "basewindow.h"


class ViewWindow : public BaseWindow
{
    Q_OBJECT
protected:
    ViewWindow();
    ~ViewWindow();
public:
    void addDockWidget(Qt::DockWidgetArea area, QDockWidget *widget, Qt::Orientation orientation);
};

#endif // VIEWWINDOW_H
