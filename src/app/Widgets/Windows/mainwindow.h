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
    MainWindow(ViewController* vc, QWidget *parent=0);
public:
    QMenu *createPopupMenu();
signals:
    void welcomeScreenToggled(bool);
private slots:
    void themeChanged();
    void resetToolDockWidgets();
private:
    bool isWelcomeScreenVisible();
    void setModelTitle(QString model_title="");

    void setDockWidgetIcon(BaseDockWidget* dockwidget, QString icon_path, QString icon_alias, Theme* theme = 0);
    void setupDockIcons();
    
    void setupRightToolPanel();
    
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

    ViewController* view_controller = 0;
    ActionController* action_controller = 0;

    BaseWindow* innerWindow = 0;
    BaseWindow* rightWindow = 0;
    BaseWindow* leftWindow = 0;

    //For the innew window
    //BaseDockWidget* dockwidget_Jenkins = 0;
    BaseDockWidget* dockwidget_Search = 0;
    BaseDockWidget* dockwidget_Qos = 0;
    BaseDockWidget* dockwidget_Notification = 0;

    BaseDockWidget* dockwidget_Charts = 0;


    //Right hand tools
    BaseDockWidget* dockwidget_Table = 0;
    BaseDockWidget* dockwidget_ViewManager = 0;
    BaseDockWidget* dockwidget_Minimap = 0;

    //Main windows dock widgets
    BaseDockWidget* dockwidget_Center = 0;
    BaseDockWidget* dockwidget_Right = 0;
    BaseDockWidget* dockwidget_Dock = 0;
    
    QMenuBar* menu_bar = 0;
    QToolBar* applicationToolbar = 0;

    QToolButton* restore_toolbutton = 0;
    QAction* reset_action = 0;

    WelcomeScreenWidget* welcomeScreen = 0;
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
