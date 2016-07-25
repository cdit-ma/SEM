#ifndef MEDEAMAINWINDOW_H
#define MEDEAMAINWINDOW_H

#include <QTableView>

#include "medeawindownew.h"
#include "../../View/nodeviewnew.h"
#include "../../View/nodeviewminimap.h"
#include "../../Controller/viewcontroller.h"
#include "tablewidget.h"

class MedeaMainWindow : public MedeaWindowNew
{
    Q_OBJECT
public:
    MedeaMainWindow(QWidget *parent=0);

    void setViewController(ViewController* vc);

private slots:
    void themeChanged();
    void activeViewDockWidgetChanged(MedeaViewDockWidget* widget, MedeaViewDockWidget* prevDock);

    void spawnSubView();

    void updateDataTableSize();

private:
    void connectNodeView(NodeViewNew* nodeView);
    void setupTools();
    void setupInnerWindow();
    void setupDataTable();
    void setupMinimap();

private:
    MedeaWindowNew* innerWindow;
    TableWidget* tableWidget;

    NodeViewNew* nodeView_Interfaces;
    NodeViewNew* nodeView_Behaviour;
    NodeViewNew* nodeView_Assemblies;
    NodeViewNew* nodeView_Hardware;
    ViewController* viewController;

    NodeViewMinimap* minimap;
};

#endif // MEDEAMAINWINDOW_H
