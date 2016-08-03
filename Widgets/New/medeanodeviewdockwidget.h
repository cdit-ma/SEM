#ifndef MEDEANODEVIEWDOCKWIDGET_H
#define MEDEANODEVIEWDOCKWIDGET_H

#include "medeaviewdockwidget.h"
#include "../../View/nodeviewnew.h"
#include "../../Controller/selectionhandler.h"

class MedeaNodeViewDockWidget : public MedeaViewDockWidget
{
    Q_OBJECT
public:
    MedeaNodeViewDockWidget(QString title, Qt::DockWidgetArea area = Qt::TopDockWidgetArea);

    SelectionHandler* getSelectionHandler();
    NodeViewNew* getNodeView();
    void setWidget(QWidget* widget);

private:
    NodeViewNew* nodeView;
};

#endif // MEDEANODEVIEWDOCKWIDGET_H
