#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "basewindow.h"
#include "../../Controllers/ViewController/viewcontroller.h"

#include <QMenu>
#include <QToolButton>

class Theme;
class WelcomeScreenWidget;

class MainWindow : public BaseWindow
{
    friend class WindowManager;
    Q_OBJECT
    
protected:
    explicit MainWindow(ViewController* vc, QWidget *parent = nullptr);

    void closeEvent(QCloseEvent *event) override;

public:
    QMenu* createPopupMenu() override;
    
signals:
    void welcomeScreenToggled(bool);
    
private slots:
    void themeChanged();
    void resetToolDockWidgets();
    
private:
    void settingChanged(SETTINGS key, const QVariant& value);
    void ShowWorkerAspect(bool show);
    bool isWelcomeScreenVisible();
    void setModelTitle(QString model_title = "");

    void setDockWidgetIcon(BaseDockWidget* dockwidget, const QString& icon_path, const QString& icon_alias, Theme* theme = nullptr);
    void setupDockIcons();
    
    void swapCentralWidget(QWidget* widget);
    void setViewController(ViewController* view_controller);
    void initializeApplication();
    void toggleWelcomeScreen(bool on);
    void toggleDocks(bool on);
    
    void saveWindowState();
    void restoreWindowState();

    void setupTools();
    void setupInnerWindow();
    void setupMenuBar();
    void setupToolBar();

    void setupMenuCornerWidget();
    void setupDockablePanels();

    void resetToolWidgets();

    ViewController* view_controller = nullptr;
    ActionController* action_controller = nullptr;

    BaseWindow* innerWindow = nullptr;
    BaseWindow* rightWindow = nullptr;

    BaseDockWidget* dockwidget_Search = nullptr;
    BaseDockWidget* dockwidget_Qos = nullptr;
    BaseDockWidget* dockwidget_Notification = nullptr;

    BaseDockWidget* dockWidget_Trigger = nullptr;
    BaseDockWidget* dockwidget_Charts = nullptr;
    BaseDockWidget* dockwidget_Dataflow = nullptr;

    //Right hand tools
    BaseDockWidget* dockwidget_Table = nullptr;
    BaseDockWidget* dockwidget_ViewManager = nullptr;
    BaseDockWidget* dockwidget_Minimap = nullptr;

    //Main windows dock widgets
    BaseDockWidget* dockwidget_Center = nullptr;
    BaseDockWidget* dockwidget_Right = nullptr;
    BaseDockWidget* dockwidget_Dock = nullptr;

    ViewDockWidget* dockwidget_workers = nullptr;
    
    QMenuBar* menu_bar = nullptr;
    QToolBar* applicationToolbar = nullptr;

    QToolButton* restore_toolbutton = nullptr;
    QAction* reset_action = nullptr;

    WelcomeScreenWidget* welcomeScreen = nullptr;
};

#endif // MAINWINDOW_H