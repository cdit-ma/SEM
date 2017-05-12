#include "nodeviewdockwidget.h"
#include <QDebug>
NodeViewDockWidget::NodeViewDockWidget(QString title, Qt::DockWidgetArea area):ViewDockWidget(title, area, MVDW_NODEVIEW)
{
    setCloseVisible(true);
    setFocusEnabled(true);
    setMaximizeVisible(true);
    setPopOutVisible(true);

    setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
    nodeView = 0;
}

NodeViewDockWidget::~NodeViewDockWidget()
{
}

SelectionHandler *NodeViewDockWidget::getSelectionHandler()
{
    if(nodeView){
        return nodeView->getSelectionHandler();
    }
    return 0;
}

NodeView *NodeViewDockWidget::getNodeView()
{
    return nodeView;
}

void NodeViewDockWidget::setWidget(QWidget *widget)
{
    NodeView* view = qobject_cast<NodeView*>(widget);
    if(view){
        nodeView = view;
        connect(nodeView, SIGNAL(destroyed(QObject*)), this, SLOT(destruct()));
        ViewDockWidget::setWidget(widget);
    }else{
        qCritical() << "Can't adopt!";
    }
}
