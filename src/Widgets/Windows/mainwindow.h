#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTableView>
#include <QPushButton>
#include <QLineEdit>
#include <QProgressBar>
#include <QCompleter>
#include <QStringListModel>
#include <QTimer>

#include "basewindow.h"
#include "welcomescreenwidget.h"

#include "../../Views/Dock/docktabwidget.h"
#include "../../Views/Search/searchdialog.h"
#include "../../Views/Table/datatablewidget.h"
#include "../../Views/QOSBrowser/qosbrowser.h"

#include "../../Views/NodeView/nodeview.h"
#include "../../Views/NodeView/nodeviewminimap.h"

#include "../../Controllers/ViewController/viewcontroller.h"
#include "../../Controllers/NotificationManager/notificationmanager.h"

#include "../../Controllers/JenkinsManager/jenkinsmanager.h"

#include "../../Widgets/Dialogs/popupwidget.h"
#include "../../Views/Notification/notificationtoolbar.h"
#include "../../Views/Notification/notificationdialog.h"



class MainWindow : public BaseWindow
{
    friend class WindowManager;
    Q_OBJECT
protected:
    MainWindow(ViewController* vc, QWidget *parent=0);
public:

signals:
    void welcomeScreenToggled(bool);
private slots:
    void themeChanged();
    void resetToolDockWidgets();
private:
    Q_INVOKABLE void updateMenuBar();
    void setModelTitle(QString model_title="");

    void setDockWidgetIcon(BaseDockWidget* dockwidget, QString icon_path, QString icon_alias, Theme* theme = 0);
    void setupDockIcons();
    
    void setupRightToolPanel();
    
    void swapCentralWidget(QWidget* widget);
    void setViewController(ViewController* view_controller);
    void initializeApplication();
    void toggleWelcomeScreen(bool on);
    
    void saveWindowState();
    void restoreWindowState(bool restore_geo = true);

    void setupTools();
    void setupInnerWindow();
    void setupWelcomeScreen();
    void setupMenuBar();
    void setupToolBar();
    void setupProgressBar();
    void setupMenuCornerWidget();
    void setupDockablePanels();

    void resizeToolWidgets();

    ViewController* view_controller = 0;
    ActionController* action_controller = 0;

    BaseWindow* innerWindow = 0;
    BaseWindow* rightWindow = 0;
    BaseWindow* leftWindow = 0;

    //For the innew window
    BaseDockWidget* dockwidget_Jenkins = 0;
    BaseDockWidget* dockwidget_Search = 0;
    BaseDockWidget* dockwidget_Qos = 0;
    BaseDockWidget* dockwidget_Notification = 0;

    //Right hand tools
    BaseDockWidget* dockwidget_Table = 0;
    BaseDockWidget* dockwidget_ViewManager = 0;
    BaseDockWidget* dockwidget_Minimap = 0;

    // Left hand tools
    BaseDockWidget* dockwidget_Dock = 0;

    //Main windows dock widgets
    BaseDockWidget* dockwidget_Center = 0;
    BaseDockWidget* dockwidget_Right = 0;
    BaseDockWidget* dockwidget_Left = 0;

    
    QMenuBar* menu_bar = 0;
    QToolBar* applicationToolbar = 0;

    QAction* restoreToolsAction = 0;



    WelcomeScreenWidget* welcomeScreen = 0;
protected:
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *object, QEvent *event);
};

#endif // MAINWINDOW_H
