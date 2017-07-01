#include "modelcontrollertests.h"
#include "utils.h"
#include <functional>
#include "../ModelController/modelcontroller.h"

void ModelControllerTests::test_controller()
{
    //Connect a way to setup/destroy controller
    ModelController* mc = new ModelController();

    //Blocked queues will halt this thread's event loop until the target function is complete.
    connect(this, &ModelControllerTests::setup_controller, mc, &ModelController::SetupController, Qt::BlockingQueuedConnection);
    connect(this, &ModelControllerTests::teardown_controller, mc, &ModelController::InitiateTeardown, Qt::BlockingQueuedConnection);
    
    //Setup the Controller (Will load work definitions and setup Base Model)
    emit setup_controller();

    QCOMPARE(mc->getProjectAsGraphML().size() > 0, true);

    //Teardown, after this the Controller is actually deleted, so don't reference controller again!
    emit teardown_controller();
}

void ModelControllerTests::load_helloworld()
{
    //Define variables for use in function
    QEventLoop wait_loop;
    bool action_success = false;
    QString action_result;
    

    ModelController* mc = new ModelController();
    connect(this, &ModelControllerTests::setup_controller, mc, &ModelController::SetupController, Qt::BlockingQueuedConnection);
    connect(this, &ModelControllerTests::teardown_controller, mc, &ModelController::InitiateTeardown, Qt::BlockingQueuedConnection);
    

    QString file_name = ":/Models/HelloWorld.graphml";
    QString file_data = Utils::readTextFile(file_name);
    
    //Connect the finish signal to set action_success, action_result and terminate the wait loop
    connect(mc, &ModelController::ActionFinished, &wait_loop, &QEventLoop::quit);

    //Setup Controller
    emit setup_controller(file_name);
    
    //Load Model
    emit open_project(file_name, file_data);
    //The loop will be unlocked once the project has either loaded or failed
    wait_loop.exec();

    //Test the compare
    QCOMPARE(action_success, true);
    
    //Setup Controller
    //emit teardown_controller();
    delete mc;
}