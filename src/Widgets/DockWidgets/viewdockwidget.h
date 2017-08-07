#ifndef VIEWDOCKWIDGET_H
#define VIEWDOCKWIDGET_H

#include "basedockwidget.h"
class ViewDockWidget : public BaseDockWidget
{
    friend class WindowManager;
    Q_OBJECT
public:
    enum VIEWDOCKWIDGET_TYPE {MVDW_NONE, MVDW_NODEVIEW};
protected:
    ViewDockWidget(QString title, Qt::DockWidgetArea area = Qt::TopDockWidgetArea, VIEWDOCKWIDGET_TYPE type = MVDW_NONE);
    ~ViewDockWidget();
public:
    bool isNodeViewDock();
    void themeChanged();

    void setWidget(QWidget *widget);

private:
    VIEWDOCKWIDGET_TYPE type;

};

#endif // VIEWDOCKWIDGET_H
