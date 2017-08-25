#include "viewdockwidget.h"
#include <QDebug>
ViewDockWidget::ViewDockWidget(QString title, Qt::DockWidgetArea area):DefaultDockWidget(title, area, DefaultDockType::VIEW)
{
    nodeView = 0;
}

ViewDockWidget::~ViewDockWidget()
{
}

SelectionHandler *ViewDockWidget::getSelectionHandler()
{
    if(nodeView){
        return nodeView->getSelectionHandler();
    }
    return 0;
}

NodeView *ViewDockWidget::getNodeView()
{
    return nodeView;
}

void ViewDockWidget::setWidget(QWidget *widget)
{
    NodeView* view = qobject_cast<NodeView*>(widget);
    if(view){
        nodeView = view;
        connect(nodeView, SIGNAL(destroyed(QObject*)), this, SLOT(destruct()));
        DefaultDockWidget::setWidget(widget);
    }else{
        qCritical() << "Can't adopt!";
    }
}
