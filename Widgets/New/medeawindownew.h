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
    bool hasDockWidgets();
    QList<MedeaDockWidget*> getDockWidgets();
    int getID();

    void addDockWidget(MedeaDockWidget *widget);

    void addDockWidget(Qt::DockWidgetArea area, QDockWidget *widget);
    void addDockWidget(Qt::DockWidgetArea area, QDockWidget* widget, Qt::Orientation orientation);
    void removeDockWidget(QDockWidget* widget);

    void setDockWidgetMaximized(MedeaDockWidget *dockwidget, bool maximized);

    bool isMainWindow();
private slots:
    void resetDockWidgets();
    void showContextMenu(const QPoint &point);

private:
    QAction* resetDockedWidgetsAction;

    bool _isMainWindow;

    QList<MedeaDockWidget*> currentDockWidgets;
    QList<MedeaDockWidget*> ownedDockWidgets;

protected:
    void closeEvent(QCloseEvent *);
public:
    QMenu *createPopupMenu();
private:
    int ID;
    static int _WindowID;
};

#endif


