#include "projectwindow.h"
#include <QLayout>
#include <QDebug>

ProjectWindow::ProjectWindow(QWidget *parent) :
    QMdiSubWindow(parent)
{

    //thread = new QThread();
    view = new NodeView(0);
    layout()->addWidget(view);

    controller = new NewController(view);
    //controller->moveToThread(thread);

    connect(this, SIGNAL(updateFilters(QStringList)), controller, SLOT(view_FilterNodes(QStringList)));
    //connect(thread, SIGNAL(started()), controller, SLOT(setupControllerOnThread()));
   // thread->start();


    //controller->moveToThread(thread);
    //thread->start();




    showMaximized();
    setWindowTitle("New Project");

    setAttribute(Qt::WA_DeleteOnClose, true);

}

ProjectWindow::~ProjectWindow()
{
    delete controller;
    delete view;
}

NodeView *ProjectWindow::getView()
{
    return view;
}

NewController *ProjectWindow::getController()
{
    return controller;
}

void ProjectWindow::appendFilterString(QString filter)
{
    if(!filters.contains(filter)){
        filters << filter;
        emit updateFilters(filters);
    }
}

void ProjectWindow::removeFilterString(QString filter)
{
    int position = filters.indexOf(filter);
    filters.removeAt(position);
    if(position != -1){
        emit updateFilters(filters);
    }
}

void ProjectWindow::clearFilters()
{
    filters.clear();
    emit updateFilters(filters);
}

void ProjectWindow::closeEvent(QCloseEvent *)
{
    delete this;
}
