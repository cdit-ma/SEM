#ifndef MEDEAWINDOWMANAGER_H
#define MEDEAWINDOWMANAGER_H

#include <QObject>
#include <QToolButton>

//Forward Class Declarations
class MedeaDockWidget;
class MedeaToolDockWidget;
class MedeaViewDockWidget;
class MedeaWindowNew;

class MedeaWindowManager : public QObject
{
    Q_OBJECT
public:
    //Public Static functions.
    static MedeaWindowManager* manager();
    static void teardown();

    //Factory constructor Functions
    static MedeaWindowNew* constructMainWindow(QString title="");
    static MedeaWindowNew* constructSubWindow(QString title="");
    static MedeaViewDockWidget* constructViewDockWidget(QString title="", Qt::DockWidgetArea area = Qt::TopDockWidgetArea);
    static MedeaToolDockWidget* constructToolDockWidget(QString title="");

    //Factory destructor functions
    static void destructWindow(MedeaWindowNew* window);
    static void destructDockWidget(MedeaDockWidget* widget);
protected:
    MedeaWindowManager();
    ~MedeaWindowManager();
signals:
    void activeViewDockWidgetChanged(MedeaViewDockWidget* widget);
public:
    MedeaViewDockWidget* getActiveViewDockWidget();
    void setActiveDockWidget(MedeaDockWidget* dockWidget = 0);
private slots:
    void dockWidget_Close();
    void dockWidget_PopOut();
    void dockWidget_Maximize(bool maximize);

    void reparentDockWidget_ButtonPressed();

private:
    //Helper functions
    MedeaWindowNew* _constructMainWindow(QString title);
    MedeaWindowNew* _constructSubWindow(QString title);
    MedeaToolDockWidget* _constructToolDockWidget(QString title);
    MedeaViewDockWidget* _constructViewDockWidget(QString title, Qt::DockWidgetArea area);

    void _destructWindow(MedeaWindowNew* window);
    void _destructDockWidget(MedeaDockWidget* widget);
    void _reparentDockWidget(MedeaDockWidget* dockWidget, MedeaWindowNew* window);
private:
    void addWindow(MedeaWindowNew *window);
    void removeWindow(MedeaWindowNew *window);
    void addDockWidget(MedeaDockWidget* dockWidget);
    void removeDockWidget(MedeaDockWidget* dockWidget);
    void destructWindowIfEmpty(MedeaWindowNew* window);

    void showPopOutDialog(MedeaDockWidget* dw);
    QToolButton* constructPopOutWindowButton(QDialog *parent, MedeaWindowNew* window=0);

    MedeaWindowNew* mainWindow;
    MedeaViewDockWidget* activeViewDockWidget;
    QHash<int, MedeaWindowNew*> windows;
    QHash<int, MedeaDockWidget*> dockWidgets;

private:
    static MedeaWindowManager* managerSingleton;
};

#endif // MEDEAWINDOWMANAGER_H
