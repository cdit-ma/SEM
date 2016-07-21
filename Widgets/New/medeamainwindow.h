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
    void activeDockWidgetChanged(MedeaDockWidget* widget);
private:
    void setupInnerWindow();
    void setupTools();


private:

    MedeaWindowNew* innerWindow;

    NodeViewNew* nodeView_Interfaces;
    NodeViewNew* nodeView_Behaviour;
    NodeViewNew* nodeView_Assemblies;
    NodeViewNew* nodeView_Hardware;
    ViewController* viewController;

    NodeViewMinimap* minimap;
    QTableView* tableView;
};

#endif // MEDEAMAINWINDOW_H
