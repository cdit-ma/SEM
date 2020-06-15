#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <QObject>
#include <QSignalMapper>
#include <QAction>
#include <QSet>

#include "../../Widgets/Charts/Timeline/chartdialog.h"
#include "../../Widgets/Dataflow/dataflowdialog.h"

//Forward Class Declarations
class BaseDockWidget;
class DefaultDockWidget;
class ViewDockWidget;
class ToolDockWidget;
class InvisibleDockWidget;
class DockReparenterPopup;

class BaseWindow;
class ViewController;
class ViewManagerWidget;
class ViewItem;

class WindowManager : public QObject
{
    friend class BaseWindow;
    friend class BaseDockWidget;
    Q_OBJECT

public:
    static bool Sort(const BaseDockWidget *a, const BaseDockWidget *b);
    
    //Public Static functions.
    static WindowManager* manager();
    static void teardown();
    static void ShowDockWidget(BaseDockWidget* widget);
    
    static void MoveWidget(QWidget* widget, QWidget* parent_widget = nullptr, Qt::Alignment = Qt::AlignCenter);
    static bool isViewDockWidget(BaseDockWidget* base_dock_widget);
    
    BaseWindow* getMainWindow();
    BaseWindow* getCentralWindow();
    ViewManagerWidget* getViewManagerGUI();
    BaseWindow* getActiveWindow();
    
    void destructDockWidget(BaseDockWidget* widget);
    
    //Factory constructor Functions
    BaseWindow* constructMainWindow(ViewController* vc);
    BaseWindow* constructSubWindow(const QString& title, BaseWindow* parent_window);
    BaseWindow* constructCentralWindow(const QString& title, BaseWindow* parent_window);
    BaseWindow* constructInvisibleWindow(const QString& title, BaseWindow* parent_window);
    
    ViewDockWidget* constructViewDockWidget(const QString& title, QWidget* parent, Qt::DockWidgetArea area = Qt::TopDockWidgetArea);
    DefaultDockWidget* constructDockWidget(const QString& title, QWidget* parent, Qt::DockWidgetArea area = Qt::TopDockWidgetArea);
    ToolDockWidget* constructToolDockWidget(const QString& title, QWidget* parent);
    InvisibleDockWidget* constructInvisibleDockWidget(const QString& title, QWidget* parent);
    
    DefaultDockWidget* constructChartDockWidget(const QString& title, ChartDialog* dialog, QWidget* parent, Qt::DockWidgetArea area = Qt::TopDockWidgetArea);
    DefaultDockWidget* constructPulseDockWidget(const QString& title, DataflowDialog* dialog, QWidget* parent, Qt::DockWidgetArea area = Qt::TopDockWidgetArea);
    
    void constructInnerDockWidget(ViewController* vc, BaseDockWidget* parentDockWidget, const QString& title = "");
    
    bool reparentDockWidget(BaseDockWidget* dockWidget);
    bool reparentDockWidget(BaseDockWidget* dockWidget, BaseWindow* window);
    
    //Getters
    ViewDockWidget* getActiveViewDockWidget();
    
    void setActiveViewDockWidget(ViewDockWidget *view = nullptr);
    
    BaseWindow* getWindow(int ID);
    QList<BaseWindow*> getWindows();
    QList<ViewDockWidget*> getViewDockWidgets();
    ViewDockWidget* getViewDockWidget(ViewItem* item);

signals:
    void windowConstructed(BaseWindow* window);
    void windowDestructed(int ID);
    
    void dockWidgetConstructed(BaseDockWidget* widget);
    void dockWidgetDestructed(int ID);
    
    void activeViewDockWidgetChanged(ViewDockWidget* widget, ViewDockWidget* prevWidget = nullptr);

private slots:
    void dockWidget_Close(int ID);
    void dockWidget_PopOut(int ID);
    void activeDockWidgetVisibilityChanged();

protected:
    WindowManager();
    ~WindowManager() final;
    
    DockReparenterPopup* getDockPopup();
    void destructWindow(BaseWindow* window);

private:
    void focusChanged(QWidget* prev, QWidget* now);
    void addWindow(BaseWindow *window);
    void removeWindow(BaseWindow *window);
    void addDockWidget(BaseDockWidget* dockWidget);
    void removeDockWidget(BaseDockWidget* dockWidget);
    
    BaseWindow* mainWindow = nullptr;
    BaseWindow* centralWindow = nullptr;
    ViewDockWidget* activeViewDockWidget = nullptr;
    
    ViewManagerWidget* viewManagerWidget = nullptr;
    DockReparenterPopup* dock_popup = nullptr;
    
    QHash<int, BaseWindow*> windows;
    QHash<int, BaseDockWidget*> dockWidgets;
    
    QSet<int> window_ids;
    QSet<int> view_dock_ids;
	
	static WindowManager* managerSingleton;
    
    Q_INVOKABLE void MoveWidgetEvent(QWidget* widget, QWidget* parent_widget, Qt::Alignment alignment);
};

#endif // WINDOWMANAGER_H
