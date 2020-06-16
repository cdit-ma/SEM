#ifndef VIEWWINDOW_H
#define VIEWWINDOW_H

#include "basewindow.h"

class ViewWindow : public BaseWindow
{
    Q_OBJECT
protected:
    explicit ViewWindow(BaseWindow* parent_window);

public:
    void addDockWidget(Qt::DockWidgetArea area, QDockWidget *widget, Qt::Orientation orientation) override;
};

#endif // VIEWWINDOW_H