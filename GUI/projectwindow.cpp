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

void ProjectWindow::closeEvent(QCloseEvent *)
{
    delete this;
}
