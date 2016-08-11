#include "medeanodeviewdockwidget.h"
#include <QDebug>
MedeaNodeViewDockWidget::MedeaNodeViewDockWidget(QString title, Qt::DockWidgetArea area):MedeaViewDockWidget(title, area, MVDW_NODEVIEW)
{
    setCloseVisible(true);
    setFocusEnabled(true);
    setMaximizeVisible(true);
    setPopOutVisible(true);

    setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
    nodeView = 0;
}

SelectionHandler *MedeaNodeViewDockWidget::getSelectionHandler()
{
    if(nodeView){
        return nodeView->getSelectionHandler();
    }
    return 0;
}

NodeViewNew *MedeaNodeViewDockWidget::getNodeView()
{
    return nodeView;
}

void MedeaNodeViewDockWidget::setWidget(QWidget *widget)
{
    NodeViewNew* view = qobject_cast<NodeViewNew*>(widget);
    if(view){
        nodeView = view;
        connect(nodeView, SIGNAL(destroyed(QObject*)), this, SLOT(destruct()));
        MedeaViewDockWidget::setWidget(widget);
    }else{
        qCritical() << "Can't adopt!";
    }
}
