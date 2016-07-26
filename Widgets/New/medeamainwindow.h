#ifndef MEDEAMAINWINDOW_H
#define MEDEAMAINWINDOW_H

#include <QTableView>
#include <QPushButton>
#include <QLineEdit>

#include "medeawindownew.h"
#include "../../View/nodeviewnew.h"
#include "../../View/nodeviewminimap.h"
#include "../../Controller/viewcontroller.h"
#include "tablewidget.h"

class MedeaMainWindow : public MedeaWindowNew
{
    Q_OBJECT
public:
    MedeaMainWindow(ViewController* vc, QWidget *parent=0);

    void setViewController(ViewController* vc);

private slots:
    void themeChanged();
    void activeViewDockWidgetChanged(MedeaViewDockWidget* widget, MedeaViewDockWidget* prevDock);

    void spawnSubView();

private:
    void connectNodeView(NodeViewNew* nodeView);
    void setupTools();
    void setupInnerWindow();
    void setupMenuAndTitle();
    void setupToolBar();
    void setupSearchBar();
    void setupDataTable();
    void setupMinimap();

private:
    MedeaWindowNew* innerWindow;

    QPushButton* menuButton;
    QPushButton* projectTitleButton;
    QToolButton* middlewareButton;
    QToolButton* closeProjectButton;

    QLineEdit* searchBar;
    QToolButton* searchButton;
    QToolButton* searchOptionsButton;

    QToolBar* floatingToolbar;
    TableWidget* tableWidget;
    NodeViewMinimap* minimap;

    NodeViewNew* nodeView_Interfaces;
    NodeViewNew* nodeView_Behaviour;
    NodeViewNew* nodeView_Assemblies;
    NodeViewNew* nodeView_Hardware;
    ViewController* viewController;

};

#endif // MEDEAMAINWINDOW_H
