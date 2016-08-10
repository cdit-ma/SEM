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

public slots:
    void resetDockWidgets();

private slots:
    void themeChanged();


private:
    QAction* resetDockedWidgetsAction;

    // QMainWindow interface
public:
    QMenu *createPopupMenu();
};

#endif // MEDEAVIEWWINDOW_H
