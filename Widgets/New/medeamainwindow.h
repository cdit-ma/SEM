#ifndef MEDEAMAINWINDOW_H
#define MEDEAMAINWINDOW_H

#include <QTableView>
#include <QPushButton>
#include <QLineEdit>

#include "medeawindownew.h"
#include "../../View/nodeviewnew.h"
#include "../../View/nodeviewminimap.h"
#include "../../Controller/viewcontroller.h"
#include "../../GUI/appsettings.h"
#include "qosbrowser.h"
#include "tablewidget.h"

class MedeaMainWindow : public MedeaWindowNew
{
    Q_OBJECT
public:
    MedeaMainWindow(ViewController* vc, QWidget *parent=0);
    ~MedeaMainWindow();

    void setViewController(ViewController* vc);

private slots:
    void themeChanged();
    void activeViewDockWidgetChanged(MedeaViewDockWidget* widget, MedeaViewDockWidget* prevDock);

    void spawnSubView();

    void popupSearch();

    void toolbarChanged(Qt::DockWidgetArea area);
    void toolbarTopLevelChanged(bool a);

public slots:
    void setModelTitle(QString modelTitle);
    void settingChanged(SETTING_KEY setting, QVariant value);

signals:
    void preloadImages();

private:
    void initializeApplication();
    void connectNodeView(NodeViewNew* nodeView);

    void setupTools();
    void setupInnerWindow();
    void setupMenuAndTitle();
    void setupMenuBar();
    void setupToolBar();
    void setupSearchBar();
    void setupPopupSearchBar();
    void setupDataTable();
    void setupMinimap();
    void setupMainDockWidgetToggles();

private:
    MedeaWindowNew* innerWindow;

    QMenuBar* menuBar;
    QPushButton* menuButton;
    QPushButton* projectTitleButton;
    QToolButton* middlewareButton;
    QToolButton* closeProjectButton;

    QLineEdit* searchBar;
    QToolButton* searchButton;
    QToolButton* searchOptionsButton;

    QToolBar* searchToolbar;
    QLineEdit* popupSearchBar;
    QToolButton* popupSearchButton;

    QToolBar* floatingToolbar;
    TableWidget* tableWidget;
    NodeViewMinimap* minimap;

    NodeViewNew* nodeView_Interfaces;
    NodeViewNew* nodeView_Behaviour;
    NodeViewNew* nodeView_Assemblies;
    NodeViewNew* nodeView_Hardware;
    QOSBrowser* qosBrowser;
    ViewController* viewController;

    QToolButton* interfaceButton;
    QToolButton* behaviourButton;
    QToolButton* assemblyButton;
    QToolButton* hardwareButton;
    QToolButton* qosBrowserButton;
    QToolButton* restoreAspectsButton;
    QToolButton* restoreToolsButton;

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *);
};

#endif // MEDEAMAINWINDOW_H
