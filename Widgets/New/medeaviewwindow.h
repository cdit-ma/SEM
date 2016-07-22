#ifndef MEDEAVIEWWINDOW_H
#define MEDEAVIEWWINDOW_H

#include "medeawindownew.h"
#include "medeawindowmanager.h"
class MedeaViewWindow : public MedeaWindowNew
{
    Q_OBJECT
public:
    MedeaViewWindow();
    void addDockWidget(Qt::DockWidgetArea area, QDockWidget *widget, Qt::Orientation orientation);

private slots:
    void resetDockWidgets();
    void showContextMenu(const QPoint &point);
    QMenu *createPopupMenu();


private:
    QAction* resetDockedWidgetsAction;
};

#endif // MEDEAVIEWWINDOW_H
