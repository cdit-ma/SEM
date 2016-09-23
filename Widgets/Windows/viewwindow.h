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
    QMenu *createPopupMenu();
    void addDockWidget(Qt::DockWidgetArea area, QDockWidget *widget, Qt::Orientation orientation);

public slots:
    void resetDockWidgets();
private slots:
    void themeChanged();
private:
    QAction* resetDockedWidgetsAction;
};

#endif // VIEWWINDOW_H
