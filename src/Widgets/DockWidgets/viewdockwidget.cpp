#include "viewdockwidget.h"
#include "../../Controllers/WindowManager/windowmanager.h"
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

        nodeView->installEventFilter(this);
        DefaultDockWidget::setWidget(widget);
    }else{
        qCritical() << "Can't adopt!";
    }
}


bool ViewDockWidget::eventFilter(QObject *o, QEvent *e){
    auto type = e->type();
    if(type == QEvent::FocusIn || type == QEvent::FocusIn){
        if(type == QEvent::FocusIn){
            WindowManager::manager()->setActiveViewDockWidget(this);
        }else{
            WindowManager::manager()->setActiveViewDockWidget(0);
        }
        return true;
    }
    return false;
}