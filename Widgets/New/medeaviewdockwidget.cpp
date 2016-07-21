#include "medeaviewdockwidget.h"

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

NodeViewNew *MedeaViewDockWidget::getNodeView()
{
    return nodeView;
}

void MedeaViewDockWidget::setWidget(QWidget *widget)
{
    NodeViewNew* view = qobject_cast<NodeViewNew*>(widget);
    if(view){
        nodeView = view;
        setWidget(view);
    }
}

