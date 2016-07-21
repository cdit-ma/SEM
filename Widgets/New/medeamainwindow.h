#ifndef MEDEAMAINWINDOW_H
#define MEDEAMAINWINDOW_H

#include <QTableView>

#include "medeawindownew.h"
#include "../../View/nodeviewnew.h"
#include "../../View/nodeviewminimap.h"

class MedeaMainWindow : public MedeaWindowNew
{
    Q_OBJECT
public:
    MedeaMainWindow(QWidget *parent=0);

    void setViewController(ViewController* vc);

private slots:
    void themeChanged();
    void activeDockWidgetChanged(MedeaDockWidget* widget);
    void dataTableModelChanged(AttributeTableModel* model);

private:
    void setupInnerWindow();
    void setupDataTable();
    void setupMinimap();

private:
    MedeaWindowNew* innerWindow;

    NodeViewNew* nodeView_Interfaces;
    NodeViewNew* nodeView_Behaviour;
    NodeViewNew* nodeView_Assemblies;
    NodeViewNew* nodeView_Hardware;
    ViewController* viewController;

    NodeViewMinimap* minimap;
    QTableView* tableView;
    bool resizeModeSet;
};

#endif // MEDEAMAINWINDOW_H
