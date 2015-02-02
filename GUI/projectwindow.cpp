#include "projectwindow.h"
#include <QLayout>
#include <QDebug>
#include <QTreeView>
#include <QMessageBox>
#include "nodetableview.h"
ProjectWindow::ProjectWindow(QWidget *parent):QMdiSubWindow(parent)
{
    //thread = new QThread();

    view = new NodeView(0);

    //this->tableView = new NodeTableView();
   // view2 = new NodeView(0);

    layout()->addWidget(view);


    controller = new NewController();
    thread = new QThread();
    thread->start();

    controller->moveToThread(thread);

    //controller->connectView(tableView);
    controller->connectView(view);

    controller->initializeModel();
    //controller->connectView(view2);


    //treeView->setModel(tableView->getModel());


    //controller->moveToThread(thread);

    connect(controller, SIGNAL(view_DialogMessage(MESSAGE_TYPE,QString)), this, SLOT(view_DialogWarning(MESSAGE_TYPE,QString)));
    connect(controller, SIGNAL(view_UpdateProjectName(QString)), this, SLOT(updateWindowTitle(QString)));
    connect(this, SIGNAL(updateFilters(QStringList)), controller, SLOT(view_FilterNodes(QStringList)));




    connect(view, SIGNAL(customContextMenuRequested(QPoint)), view, SLOT(showContextMenu(QPoint)));
    //connect(view2, SIGNAL(customContextMenuRequested(QPoint)), view, SLOT(showContextMenu(QPoint)));



   //controller->moveToThread(thread);
    //appendAspectString("Definitions");
    foreach(QString aspect, controller->getViewAspects()){
        appendAspectString(aspect);
    }

    showMaximized();

    //setAttribute(Qt::WA_DeleteOnClose, true);


}
ProjectWindow::~ProjectWindow()
{

    //qCritical() << "Deleting Project Window";
    delete controller;
    delete view;
    //delete view2;
}

NodeView *ProjectWindow::getView()
{
    return view;
}

NewController *ProjectWindow::getController()
{
    return controller;
}

void ProjectWindow::treeViewItemSelected(QModelIndex index)
{
    //tableView->view_TreeViewItemSelected(index);
}

void ProjectWindow::treeViewItemCentered(QModelIndex index)
{
    //tableView->view_TreeViewItemSelected(index);
}

void ProjectWindow::selectedProject()
{
    emit updateFilterButtons(appliedFilterButtons);
    emit updateAspectButtons(appliedAspectButtons);
    //emit setTableView(tableView->getModel());
    controller->view_ClearSelection();
}

void ProjectWindow::appendFilterString(QString filter)
{
    if(!appliedFilters.contains(filter)){
        FilterButton* filterButton = new FilterButton(filter, this);
        connect(filterButton, SIGNAL(removeFilter(QString)), this, SLOT(removeFilterString(QString)));
        appliedFilterButtons.append(filterButton);
        appliedFilters << filter;


        emit updateFilters(appliedFilters);
        emit updateFilterButtons(appliedFilterButtons);
    }
}

void ProjectWindow::removeFilterString(QString filter)
{
    int index = appliedFilters.indexOf(filter);

    if(index != -1){
        appliedFilters.removeAt(index);
        appliedFilterButtons.removeAt(index);
        emit updateFilters(appliedFilters);
        emit updateFilterButtons(appliedFilterButtons);
    }

}

void ProjectWindow::clearFilters()
{
    for(int i = 0; i < appliedFilterButtons.size(); i++){
        appliedFilterButtons.removeAt(i);
    }
    appliedFilters.clear();
    appliedFilterButtons.clear();

    emit updateFilters(appliedFilters);
    emit updateFilterButtons(appliedFilterButtons);
}

void ProjectWindow::appendAspectString(QString aspect)
{

    if(!visibleAspects.contains(aspect)){
        FilterButton* aspectButton = new FilterButton(aspect, this);
        connect(aspectButton, SIGNAL(removeFilter(QString)), this, SLOT(removeAspectString(QString)));
        appliedAspectButtons.append(aspectButton);

        visibleAspects << aspect;

        view->setViewAspects(visibleAspects);
        emit updateAspectButtons(appliedAspectButtons);
    }

}

void ProjectWindow::removeAspectString(QString aspect)
{
    int index = visibleAspects.indexOf(aspect);

    if(index != -1){
        visibleAspects.removeAt(index);
        appliedAspectButtons.removeAt(index);

        view->setViewAspects(visibleAspects);
        emit updateAspectButtons(appliedAspectButtons);
    }
}

void ProjectWindow::clearAspects()
{
    for(int i = 0; i < appliedAspectButtons.size(); i++){
        appliedAspectButtons.removeAt(i);
    }
    visibleAspects.clear();
    appliedAspectButtons.clear();

    view->setViewAspects(visibleAspects);
    emit updateAspectButtons(appliedAspectButtons);
}

void ProjectWindow::updateWindowTitle(QString title)
{
    setWindowTitle(title);
}

void ProjectWindow::setVisibleAspects(QStringList aspects)
{
    visibleAspects = aspects;
    view->setViewAspects(aspects);

}

void ProjectWindow::view_DialogWarning(MESSAGE_TYPE type, QString output)
{
    if(type == CRITICAL){
        QMessageBox::critical(this, "Error", output, QMessageBox::Ok);
    }else if(type == WARNING){
        QMessageBox::warning(this, "Warning", output, QMessageBox::Ok);
    }else{
        QMessageBox::information(this, "Message", output, QMessageBox::Ok);
    }
}

void ProjectWindow::closeEvent(QCloseEvent *)
{
    delete this;
}
