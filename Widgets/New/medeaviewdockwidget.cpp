#include "medeaviewdockwidget.h"
#include <QDebug>

MedeaViewDockWidget::MedeaViewDockWidget(QString title, Qt::DockWidgetArea area):MedeaDockWidget(MedeaDockWidget::MDW_VIEW)
{
    setTitle(title);
    setDockWidgetArea(area);

    setCloseVisible(true);
    setFocusEnabled(true);
    setMaximizeVisible(true);
    setPopOutVisible(true);

    setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
    nodeView = 0;
}

SelectionHandler *MedeaViewDockWidget::getSelectionHandler()
{
    if(nodeView){
        return nodeView->getSelectionHandler();
    }
    return 0;
}

NodeViewNew *MedeaViewDockWidget::getNodeView()
{
    return nodeView;
}

void MedeaViewDockWidget::setWidget(QWidget *widget)
{
    qCritical() << "Setting View!?";
    NodeViewNew* view = qobject_cast<NodeViewNew*>(widget);
    if(view){
        nodeView = view;
    }
    MedeaDockWidget::setWidget(widget);
}

