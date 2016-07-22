#ifndef MEDEAVIEWDOCKWIDGET_H
#define MEDEAVIEWDOCKWIDGET_H

#include "../../View/nodeviewnew.h"
#include "../../Controller/selectionhandler.h"
#include "medeadockwidget.h"
class NodeViewNew;
class MedeaViewDockWidget : public MedeaDockWidget
{
public:
    MedeaViewDockWidget(QString title, Qt::DockWidgetArea area = Qt::TopDockWidgetArea);

    SelectionHandler* getSelectionHandler();
    NodeViewNew* getNodeView();
    void setWidget(QWidget* widget);

private:
    NodeViewNew* nodeView;
};

#endif // MEDEAVIEWDOCKWIDGET_H
