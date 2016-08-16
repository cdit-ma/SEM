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
#include "../../GUI/searchsuggestcompletion.h"
#include <QCompleter>
#include <QStringListModel>

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
    void updateSearchSuggestions(QStringList list);

    void toolbarChanged(Qt::DockWidgetArea area);
    void toolbarTopLevelChanged(bool a);

public slots:
    void setModelTitle(QString modelTitle);
    void settingChanged(SETTING_KEY setting, QVariant value);

signals:
    void requestSuggestions();
    void preloadImages();

private:
    void initializeApplication();
    void connectNodeView(NodeViewNew* nodeView);

    void setupTools();
    void setupInnerWindow();
    void setupMenuBar();
    void setupToolBar();
    void setupSearchBar();
    void setupDataTable();
    void setupMinimap();
    void setupMainDockWidgetToggles();

    MedeaWindowNew* innerWindow;

    QMenuBar* menuBar;
    QToolBar* applicationToolbar;
    TableWidget* tableWidget;
    NodeViewMinimap* minimap;

    QToolBar* searchToolbar;
    QLineEdit* searchBar;
    QToolButton* searchButton;
    QCompleter* searchCompleter;
    QStringListModel* searchCompleterModel;

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
    QToolButton* qosBrowserButton;
    QToolButton* restoreAspectsButton;
    QToolButton* restoreToolsButton;

protected:
    void resizeEvent(QResizeEvent *);
};

#endif // MEDEAMAINWINDOW_H
