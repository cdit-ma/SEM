#ifndef MEDEAVIEWDOCKWIDGET_H
#define MEDEAVIEWDOCKWIDGET_H

#include "medeadockwidget.h"
class MedeaViewDockWidget : public MedeaDockWidget
{
    Q_OBJECT
public:
    enum VIEWDOCKWIDGET_TYPE {MVDW_NONE, MVDW_NODEVIEW};
    MedeaViewDockWidget(QString title, Qt::DockWidgetArea area = Qt::TopDockWidgetArea, VIEWDOCKWIDGET_TYPE type = MVDW_NONE);
    bool isNodeViewDock();
    void themeChanged();

private:
    VIEWDOCKWIDGET_TYPE type;
};

#endif // MEDEAVIEWDOCKWIDGET_H
