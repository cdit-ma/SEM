#ifndef MEDEAWINDOWMANAGER_H
#define MEDEAWINDOWMANAGER_H

#include <QObject>
#include <QAction>
#include <QMenu>
#include <QToolButton>

class MedeaDockWidget;
class MedeaWindowNew;
class MedeaWindowManager:public QObject
{
    Q_OBJECT

protected:
    MedeaWindowManager();
    ~MedeaWindowManager();

public:
    MedeaDockWidget* getActiveDockWidget();
    void setActiveDockWidget(MedeaDockWidget* dockWidget = 0);

//private helper functions.
private:
    MedeaWindowNew* _constructMainWindow(QString title);
    MedeaWindowNew* _constructSubWindow(QString title);
    MedeaDockWidget* _constructDockWidget(QString title, Qt::DockWidgetArea initalArea);
    void _destructWindow(MedeaWindowNew* window);
    void _destructDockWidget(MedeaDockWidget* widget);



private:
    void addDockWidget(MedeaDockWidget* dockWidget);
    void removeDockWidget(MedeaDockWidget* dockWidget);

    void addWindow(MedeaWindowNew* window);
    void removeWindow(MedeaWindowNew* window);
private slots:
    void dockWidgetPopOut(bool popOut);
    void widgetButtonPressed();
private:
    void reparentDockWidget(MedeaDockWidget* dockWidget, MedeaWindowNew* window);

    void showPopOutDialog(MedeaDockWidget* dw);
    QToolButton* constructPopOutWindowButton(MedeaWindowNew* window=0);



    MedeaWindowNew* mainWindow;
    MedeaDockWidget* activeDockWidget;
    QHash<int, MedeaWindowNew*> windows;
    QHash<int, MedeaDockWidget*> dockWidgets;

public:
    static MedeaWindowManager* manager();
    static void teardown();

    static MedeaWindowNew* constructMainWindow(QString title="");
    static MedeaWindowNew* constructSubWindow(QString title="");
    static MedeaDockWidget* constructDockWidget(QString title="", Qt::DockWidgetArea initialArea = Qt::TopDockWidgetArea);

    static void destructWindow(MedeaWindowNew* window);
    static void destructDockWidget(MedeaDockWidget* widget);

private:
    static MedeaWindowManager* managerSingleton;
};

#endif // MEDEAWINDOWMANAGER_H
