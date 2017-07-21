#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <QObject>
#include <QSignalMapper>
#include <QAction>

//Forward Class Declarations
class BaseDockWidget;
class ToolDockWidget;
class ViewDockWidget;
class NodeViewDockWidget;
class BaseWindow;
class ViewController;
class ViewManagerWidget;
class ViewItem;

class WindowManager : public QObject
{
    Q_OBJECT
public:
    //Public Static functions.
    static WindowManager* manager();
    static void teardown();

    static BaseWindow* getMainWindow();

    //Factory constructor Functions
    static BaseWindow* constructMainWindow(ViewController* vc);
    static BaseWindow* constructSubWindow(QString title="");
    static BaseWindow* constructCentralWindow(QString title="");
    static NodeViewDockWidget* constructNodeViewDockWidget(QString title="", Qt::DockWidgetArea area = Qt::TopDockWidgetArea);
    static ViewDockWidget* constructViewDockWidget(QString title="", Qt::DockWidgetArea area = Qt::TopDockWidgetArea);
    static ToolDockWidget* constructToolDockWidget(QString title="");

    //Factory destructor functions
    static void destructWindow(BaseWindow* window);
    static void destructDockWidget(BaseDockWidget* widget);
protected:
    WindowManager();
    ~WindowManager();
signals:
    void windowConstructed(BaseWindow* window);
    void windowDestructed(int ID);
    void viewDockWidgetConstructed(BaseDockWidget* widget);
    void viewDockWidgetDestructed(int ID);

    void activeViewDockWidgetChanged(ViewDockWidget* widget, ViewDockWidget* prevWidget = 0);
public:
    BaseWindow* getCentralWindow();
    ViewManagerWidget* getViewManagerGUI();
    BaseWindow* getActiveWindow();
    ViewDockWidget* getActiveViewDockWidget();
    void setActiveDockWidget(BaseDockWidget *dockWidget = 0);
    void setActiveDockWidget(int ID);
    QList<ViewDockWidget*> getViewDockWidgets();
    QList<NodeViewDockWidget*> getNodeViewDockWidgets();
    NodeViewDockWidget* getNodeViewDockWidget(ViewItem* item);

    void currentDockWidget(BaseDockWidget *dockWidget);
private slots:
    void focusChanged(QWidget *old, QWidget *now);
    void dockWidget_Close(int ID);
    void dockWidget_PopOut(int ID);

    void reparentDockWidgetAction(int windowID);

    void activeDockWidgetVisibilityChanged();

private:
    //Helper functions
    BaseWindow* _constructMainWindow(ViewController *vc);
    BaseWindow* _constructSubWindow(QString title="");
    BaseWindow* _constructCentralWindow(QString title);
    ToolDockWidget* _constructToolDockWidget(QString title);
    ViewDockWidget* _constructViewDockWidget(QString title, Qt::DockWidgetArea area);
    NodeViewDockWidget* _constructNodeViewDockWidget(QString title, Qt::DockWidgetArea area);

    void _destructWindow(BaseWindow* window);
    void _destructDockWidget(BaseDockWidget* widget);
    void _reparentDockWidget(BaseDockWidget* dockWidget, BaseWindow* window);
private:
    void addWindow(BaseWindow *window);
    void removeWindow(BaseWindow *window);
    void addDockWidget(BaseDockWidget* dockWidget);
    void removeDockWidget(BaseDockWidget* dockWidget);
    void destructWindowIfEmpty(BaseWindow* window);

    void showPopOutDialog(BaseDockWidget* dw);
    QAction* constructPopOutWindowAction(QSignalMapper* mapper, BaseWindow* window=0);

    BaseWindow* mainWindow;
    BaseWindow* centralWindow;
    ViewDockWidget* activeViewDockWidget;
    BaseDockWidget* currentDockWidget_ = 0;



    ViewManagerWidget* viewManagerWidget;
    QHash<int, BaseWindow*> windows;
    QHash<int, BaseDockWidget*> dockWidgets;
    QList<int> windowIDs;
    QList<int> viewDockIDs;

private:
    static WindowManager* managerSingleton;
};

#endif // WINDOWMANAGER_H
