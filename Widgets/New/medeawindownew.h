#ifndef MEDEAWINDOWNEW_H
#define MEDEAWINDOWNEW_H

#include <QMainWindow>
#include "medeawindowmanager.h"
#include "medeadockwidget.h"
class MedeaWindowNew : public QMainWindow{
Q_OBJECT
friend class MedeaWindowManager;
protected:
    MedeaWindowNew(QWidget* parent = 0, bool mainWindow=false);
    ~MedeaWindowNew();
public:

    QList<MedeaDockWidget*> getDockWidgets();
    int getID();
    void addMedeaDockWidget(MedeaDockWidget* widget, Qt::DockWidgetArea area=Qt::TopDockWidgetArea);
    void removeMedeaDockWidget(MedeaDockWidget *dockwidget);
    bool isMainWindow();
private slots:
    void dockWidget_Closed();
    void dockWidget_Maximized(bool maximized);
    void resetDockWidgets();
    void showContextMenu(const QPoint &point);

private:
    QAction* resetDockedWidgetsAction;

    bool _isMainWindow;
    QList<MedeaDockWidget*> childrenDockWidgets;
protected:
    void closeEvent(QCloseEvent *);
public:
    QMenu *createPopupMenu();
private:
    int ID;
    static int _WindowID;
};

#endif


