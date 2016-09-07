#ifndef MEDEAMAINWINDOW_H
#define MEDEAMAINWINDOW_H

#include <QTableView>
#include <QPushButton>
#include <QLineEdit>
#include <QProgressBar>
#include <QCompleter>
#include <QStringListModel>

#include "../../Plugins/CUTS/cutsmanager.h"
#include "medeawindownew.h"
#include "../../View/nodeviewnew.h"
#include "../../View/nodeviewminimap.h"
#include "../../Controller/viewcontroller.h"
#include "../../GUI/appsettings.h"
#include "qosbrowser.h"
#include "tablewidget.h"

#include "../../Plugins/Jenkins/jenkinsmanager.h"
#include "../../plugins/XMI/xmiimporter.h"

#include "../../GUI/popupwidget.h"
#include "../../View/Dock/docktabwidget.h"
#include "../../Widgets/New/searchdialog.h"


class MedeaMainWindow : public MedeaWindowNew
{
    Q_OBJECT
public:
    MedeaMainWindow(ViewController* vc, QWidget *parent=0);
    ~MedeaMainWindow();

    void setViewController(ViewController* vc);

signals:
    void requestSuggestions();
    void preloadImages();
    void jenkins_validated(bool);

public slots:
    void setModelTitle(QString modelTitle);
    void settingChanged(SETTING_KEY setting, QVariant value);

    void showNotification(QString title, QString message);
    void showProgressBar(bool show, QString description = "");
    void updateProgressBar(int value);

    void resetToolDockWidgets();

private slots:
    void themeChanged();
    void activeViewDockWidgetChanged(MedeaViewDockWidget* widget, MedeaViewDockWidget* prevDock);

    void popupSearch();
    void updateSearchSuggestions(QStringList list);

    void toolbarChanged(Qt::DockWidgetArea area);
    void toolbarTopLevelChanged(bool a);

    void hideWelcomeScreen(QAction *action);
    void recentProjectsUpdated();

private:
    void initializeApplication();
    void connectNodeView(NodeViewNew* nodeView);
    void toggleWelcomeScreen(bool on);

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
    void setupMinimap();
    void setupWindowManager();
    void setupMainDockWidgetToggles();

    void setupJenkinsManager();
    void setupCUTSManager();
    void setupXMIImporter();

    void moveWidget(QWidget* widget, Qt::Alignment alignment = Qt::AlignCenter);

    MedeaWindowNew* innerWindow;

    JenkinsManager* jenkinsManager;
    CUTSManager* cutsManager;
    XMIImporter* xmiImporter;

    QMenuBar* menuBar;
    QToolBar* applicationToolbar;
    DockTabWidget* dockTabWidget;
    TableWidget* tableWidget;
    NodeViewMinimap* minimap;

    PopupWidget* searchPopup;
    QToolBar* searchToolbar;
    QLineEdit* searchBar;
    QToolButton* searchButton;
    QCompleter* searchCompleter;
    QStringListModel* searchCompleterModel;
    SearchDialog* searchDialog;

    PopupWidget* progressPopup;
    QProgressBar* progressBar;
    QLabel* progressLabel;

    PopupWidget* notificationPopup;
    QLabel* notificationLabel;
    QTimer* notificationTimer;

    ViewController* viewController;
    NodeViewNew* nodeView_Interfaces;
    NodeViewNew* nodeView_Behaviour;
    NodeViewNew* nodeView_Assemblies;
    NodeViewNew* nodeView_Hardware;
    QOSBrowser* qosBrowser;

    QToolButton* interfaceButton;
    QToolButton* behaviourButton;
    QToolButton* assemblyButton;
    QToolButton* hardwareButton;
    QToolButton* restoreAspectsButton;
    QToolButton* restoreToolsButton;
    QAction* restoreToolsAction;

    QVBoxLayout* holderLayout;
    QToolBar* recentProjectsToolbar;
    QWidget* welcomeWidget;
    bool welcomeScreenOn;

protected:
    void resizeEvent(QResizeEvent *);

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // MEDEAMAINWINDOW_H
