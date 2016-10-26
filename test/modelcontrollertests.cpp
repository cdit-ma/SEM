#include "modelcontrollertests.h"
#include <QDebug>
#include <QDateTime>

#include "../src/Controllers/modelcontroller.h"
#include "../src/Utils/filehandler.h"

void ModelControllerTests::initializeControllerTest()
{
    //Connect a way to setup/destroy controller
    ModelController* controller = new ModelController();
    //Blocked queues will halt this thread's event loop until the target function is complete.
    connect(this, SIGNAL(setupController()), controller, SLOT(setupController()), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(teardownController()), controller, SIGNAL(initiateTeardown()), Qt::BlockingQueuedConnection);

    //Setup the Controller (Will load work definitions and setup Base Model)
    emit setupController();

    QCOMPARE(controller->getProjectAsGraphML().size() > 0, true);

    //Teardown, after this the Controller is actually deleted, so don't reference controller again!
    emit teardownController();
}

void ModelControllerTests::loadHelloWorldTest()
{
    ModelController* controller = new ModelController();
    connect(this, SIGNAL(setupController()), controller, SLOT(setupController()), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(openProject(QString, QString)), controller, SLOT(openProject(QString,QString)), Qt::BlockingQueuedConnection);

    QString fileName = ":/Models/HelloWorld.graphml";
    QString fileData = FileHandler::readTextFile(fileName);
    emit setupController();
    emit openProject(fileName, fileData);
    QCOMPARE(controller->getProjectAsGraphML().size() > 0, true);

    //Teardown, after this the Controller is actually deleted, so don't reference controller again!
    emit teardownController();
}
