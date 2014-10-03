#include "projectwindow.h"
#include <QLayout>
#include <QDebug>
ProjectWindow::ProjectWindow(QWidget *parent) :
    QMdiSubWindow(parent)
{

    view = new NodeView(parent);
    controller = new NewController(view);

    layout()->addWidget(view);


    showMaximized();
    setWindowTitle("New Project");

    setAttribute(Qt::WA_DeleteOnClose, true);
}

ProjectWindow::~ProjectWindow()
{
    delete view;
    delete controller;
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
