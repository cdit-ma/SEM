#ifndef MEDEAMAINWINDOW_H
#define MEDEAMAINWINDOW_H

#include <QTableView>
#include <QPushButton>
#include <QLineEdit>
#include <QProgressBar>

#include "../../Plugins/CUTS/cutsmanager.h"
#include "medeawindownew.h"
#include "../../View/nodeviewnew.h"
#include "../../View/nodeviewminimap.h"
#include "../../Controller/viewcontroller.h"
#include "../../GUI/appsettings.h"
#include "qosbrowser.h"
#include "tablewidget.h"
#include <QCompleter>
#include <QStringListModel>

#include "../../Plugins/Jenkins/jenkinsmanager.h"

#include "../../GUI/popupwidget.h"
#include "../../View/Dock/docktabwidget.h"


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
    void setupDock();
    void setupDataTable();
    void setupMinimap();
    void setupMainDockWidgetToggles();

    void setupJenkinsManager();
    void setupCUTSManager();

    MedeaWindowNew* innerWindow;
    QMainWindow* centralWindow;

    JenkinsManager* jenkinsManager;
    CUTSManager* cutsManager;

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

    PopupWidget* progressPopup;
    QProgressBar* progressBar;
    QLabel* progressLabel;

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

    QWidget* welcomeWidget;
    QToolBar* leftWelcomeToolbar;
    QToolBar* bottomWelcomeToolbar;
    QToolBar* recentProjectsToolbar;
    QVBoxLayout* holderLayout;
    bool welcomeScreenOn;

protected:
    void resizeEvent(QResizeEvent *);
};

#endif // MEDEAMAINWINDOW_H
