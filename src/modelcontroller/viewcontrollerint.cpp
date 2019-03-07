#include "viewcontrollerint.h"
#include "modelcontroller.h"

void ViewControllerInterface::ConnectModelController(ModelController* model_controller){
    if(model_controller){
        connect(model_controller, &ModelController::NodeConstructed, this, &ViewControllerInterface::NodeConstructed, Qt::QueuedConnection);
        connect(model_controller, &ModelController::EdgeConstructed, this, &ViewControllerInterface::EdgeConstructed, Qt::QueuedConnection);
        connect(model_controller, &ModelController::EntityDestructed, this, &ViewControllerInterface::EntityDestructed, Qt::QueuedConnection);
        
        connect(model_controller, &ModelController::DataChanged, this, &ViewControllerInterface::DataChanged, Qt::QueuedConnection);
        connect(model_controller, &ModelController::DataRemoved, this, &ViewControllerInterface::DataRemoved, Qt::QueuedConnection);
        
        //Notifier Signals
        connect(model_controller, &ModelController::ModelReady, this, &ViewControllerInterface::ModelReady, Qt::QueuedConnection);

        connect(model_controller, &ModelController::NodeEdgeKindsChanged, this, &ViewControllerInterface::NodeEdgeKindsChanged, Qt::QueuedConnection);
        connect(model_controller, &ModelController::NodeTypesChanged, this, &ViewControllerInterface::NodeTypesChanged, Qt::QueuedConnection);

        connect(model_controller, &ModelController::ProjectModified, this, &ViewControllerInterface::ProjectModified, Qt::QueuedConnection);
        connect(model_controller, &ModelController::ProjectFileChanged, this, &ViewControllerInterface::ProjectFileChanged, Qt::QueuedConnection);
        connect(model_controller, &ModelController::ActionFinished, this, &ViewControllerInterface::ActionFinished, Qt::QueuedConnection);
        connect(model_controller, &ModelController::Notification, this, &ViewControllerInterface::AddNotification, Qt::QueuedConnection);

        connect(model_controller, &ModelController::ShowProgress, this, &ViewControllerInterface::ShowProgress, Qt::QueuedConnection);
        connect(model_controller, &ModelController::ProgressChanged, this, &ViewControllerInterface::ProgressUpdated, Qt::QueuedConnection);

        connect(model_controller, &ModelController::UndoRedoUpdated, this, &ViewControllerInterface::UndoRedoUpdated, Qt::QueuedConnection);
        model_controller->ConnectViewController(this);
    }
};