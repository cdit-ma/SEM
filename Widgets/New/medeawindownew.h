#ifndef MEDEAWINDOWNEW_H
#define MEDEAWINDOWNEW_H

#include <QMainWindow>
#include "medeawindowmanager.h"
#include "medeadockwidget.h"
class MedeaWindowNew : public QMainWindow{
Q_OBJECT

public:
    MedeaWindowNew(QWidget* parent = 0);

    void addMedeaDockWidget(MedeaDockWidget* widget, Qt::DockWidgetArea area=Qt::TopDockWidgetArea);
    void removeMedeaDockWidget(MedeaDockWidget *dockwidget);

private slots:
    void dockWidget_Closed();
    void dockWidget_Maximized(bool maximized);
    void dockWidget_PopOut(bool popOut);

private:
    QAction* resetDockedWidgetsAction;

    QList<MedeaDockWidget*> childrenDockWidgets;

public:
    QMenu *createPopupMenu();
};

#endif


