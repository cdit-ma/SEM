#ifndef MEDEAWINDOWMANAGER_H
#define MEDEAWINDOWMANAGER_H

#include <QObject>
#include <QToolButton>

//Forward Class Declarations
class MedeaDockWidget;
class MedeaToolDockWidget;
class MedeaViewDockWidget;
class MedeaNodeViewDockWidget;
class MedeaWindowNew;
class ViewController;

class MedeaWindowManager : public QObject
{
    Q_OBJECT
public:
    //Public Static functions.
    static MedeaWindowManager* manager();
    static void teardown();

    static MedeaWindowNew* getMainWindow();

    //Factory constructor Functions
    static MedeaWindowNew* constructMainWindow(ViewController* vc);
    static MedeaWindowNew* constructSubWindow(QString title="");
    static MedeaWindowNew* constructCentralWindow(QString title="");
    static MedeaNodeViewDockWidget* constructNodeViewDockWidget(QString title="", Qt::DockWidgetArea area = Qt::TopDockWidgetArea);
    static MedeaViewDockWidget* constructViewDockWidget(QString title="", Qt::DockWidgetArea area = Qt::TopDockWidgetArea);
    static MedeaToolDockWidget* constructToolDockWidget(QString title="");

    //Factory destructor functions
    static void destructWindow(MedeaWindowNew* window);
    static void destructDockWidget(MedeaDockWidget* widget);
protected:
    MedeaWindowManager();
    ~MedeaWindowManager();
signals:
    void activeViewDockWidgetChanged(MedeaViewDockWidget* widget, MedeaViewDockWidget* prevWidget = 0);
public:
    MedeaWindowNew* getActiveWindow();
    MedeaViewDockWidget* getActiveViewDockWidget();
    void setActiveDockWidget(MedeaDockWidget *dockWidget = 0);
    QList<MedeaViewDockWidget*> getViewDockWidgets();
    QList<MedeaNodeViewDockWidget*> getNodeViewDockWidgets();
private slots:
    void dockWidget_Close();
    void dockWidget_PopOut();
    void dockWidget_Maximize(bool maximize);

    void reparentDockWidget_ButtonPressed();

private:
    //Helper functions
    MedeaWindowNew* _constructMainWindow(ViewController *vc);
    MedeaWindowNew* _constructSubWindow(QString title);
    MedeaWindowNew* _constructCentralWindow(QString title);
    MedeaToolDockWidget* _constructToolDockWidget(QString title);
    MedeaViewDockWidget* _constructViewDockWidget(QString title, Qt::DockWidgetArea area);
    MedeaNodeViewDockWidget* _constructNodeViewDockWidget(QString title, Qt::DockWidgetArea area);

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
    MedeaWindowNew* centralWindow;
    MedeaViewDockWidget* activeViewDockWidget;
    QHash<int, MedeaWindowNew*> windows;
    QHash<int, MedeaDockWidget*> dockWidgets;
    QList<int> viewDockIDs;

private:
    static MedeaWindowManager* managerSingleton;
};

#endif // MEDEAWINDOWMANAGER_H
