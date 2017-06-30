#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTableView>
#include <QPushButton>
#include <QLineEdit>
#include <QProgressBar>
#include <QCompleter>
#include <QStringListModel>
#include <QTimer>

#include <QtCharts/QChartView>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
QT_CHARTS_USE_NAMESPACE

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
    ~MainWindow();

public:
signals:
    void requestSuggestions();
    void preloadImages();
    void jenkins_validated(bool);
    void recentProjectsUpdated();

public slots:
    void setModelTitle(QString modelTitle="");

    void popupNotification(QString iconPath, QString iconName, QString description);
    void toggleNotificationPanel();
    void ensureNotificationPanelVisible();

    void showProgressBar(bool show, QString description = "");
    void updateProgressBar(int value);

    void updateMenuBarSize();

    void resetToolDockWidgets();

private slots:
    void updateGraph(QString name, double value);
    void themeChanged();
    void activeViewDockWidgetChanged(ViewDockWidget* widget, ViewDockWidget* prevDock);

    void popupSearch();
    void updateSearchSuggestions(QStringList list);
    void searchEntered();

    void toolbarChanged(Qt::DockWidgetArea area);
    void toolbarTopLevelChanged(bool a);

private:
    void setViewController(ViewController* vc);
    void initializeApplication();
    void toggleWelcomeScreen(bool on);
    void saveSettings();

    void setupTools();
    void setupInnerWindow();
    void setupWelcomeScreen();
    void setupMenuBar();
    void setupToolBar();
    void setupSearchBar();
    void setupProgressBar();
    void setupNotificationBar();
    void setupDock();
    void setupDataTable();
    void setupViewManager();
    void setupMinimap();
    void setupMenuCornerWidget();
    void setupDockablePanels();

    void setupJenkinsManager();
    void setupXMIImporter();

    void resizeToolWidgets();
    void moveWidget(QWidget* widget, QWidget* parentWidget = 0, Qt::Alignment alignment = Qt::AlignCenter);

    ViewController* viewController;

    BaseWindow* innerWindow;
    BaseDockWidget *jenkinsDockWidget;
    BaseDockWidget* searchDockWidget;
    BaseDockWidget* notificationDockWidget;

    //CUTSManager* cutsManager;
    //XMIImporter* xmiImporter;

    QMenuBar* menuBar;
    QToolBar* applicationToolbar;
    DockTabWidget* dockTabWidget;
    DataTableWidget* tableWidget;
    NodeViewMinimap* minimap;
    ViewManagerWidget* viewManager;

    PopupWidget* searchPopup;
    QToolBar* searchToolbar;
    QLineEdit* searchBar;
    QToolButton* searchButton;
    QCompleter* searchCompleter;
    QStringListModel* searchCompleterModel;
    SearchDialog* searchPanel;

    PopupWidget* progressPopup;
    QProgressBar* progressBar;
    QLabel* progressLabel;

    PopupWidget* notificationPopup;
    QWidget* notificationWidget;
    QLabel* notificationIconLabel;
    QLabel* notificationLabel;
    QTimer* notificationTimer;
    NotificationToolbar* notificationToolbar;
    NotificationDialog* notificationPanel;

    QToolButton* restoreToolsButton;
    QAction* restoreToolsAction;

    WelcomeScreenWidget* welcomeScreen;
    QVBoxLayout* holderLayout;
    bool welcomeScreenOn;

    QHash<QString, QLineSeries*> series_hash;
    QChart* chart;
    QValueAxis *axisX;
    QValueAxis *axisY;

    double max_y_range = 0;

protected:
    void resizeEvent(QResizeEvent *);
    void closeEvent(QCloseEvent *event);

};

#endif // MAINWINDOW_H
