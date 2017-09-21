#include "invisiblewindow.h"

#include "../../Controllers/WindowManager/windowmanager.h"
#include "../../theme.h"

InvisibleWindow::InvisibleWindow(BaseWindow* parent_window):BaseWindow(0, BaseWindow::INVISIBLE_WINDOW)
{
    this->parent_window = parent_window;
    setAcceptDrops(true);
    setDockNestingEnabled(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &BaseWindow::dockWidgetVisibilityChanged, this, &InvisibleWindow::updateVisibility);
}

void InvisibleWindow::updateVisibility(){
    bool visible = false;
    for(auto dock_widget : getDockWidgets()){
        if(dock_widget->isVisibleTo(this)){
            visible = true; 
        }
    }
    auto parent_widget = parentWidget();
    if(parent_widget){
        parent_widget->setVisible(visible);
    }
    
}


QMenu* InvisibleWindow::createPopupMenu(){
    if(parent_window){
        return parent_window->createPopupMenu();
    }else{
        return new QMenu(this);
    }
}