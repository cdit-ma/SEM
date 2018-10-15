#include "aggregationproxy.h"
#include <iostream>
#include "../../Controllers/NotificationManager/notificationmanager.h"
#include "../../Controllers/NotificationManager/notificationobject.h"

AggregationProxy::AggregationProxy():
controller_("tcp://192.168.111.187:20000")
{
}


void AggregationProxy::RequestRunningExperiments(){
    auto notification = NotificationManager::manager()->AddNotification("Requesting Running Experiments", "Icons", "buildingPillared", Notification::Severity::RUNNING, Notification::Type::APPLICATION, Notification::Category::NONE);
    


    try{
        auto results = controller_.ListExperiments();
        std::cerr << "* Got Experiments" << std::endl;
        for(auto result : results){
            NotificationManager::manager()->AddNotification(QString("Got Experiment: ") + QString(result.c_str()), "Icons", "circleBoltDark", Notification::Severity::INFO, Notification::Type::APPLICATION, Notification::Category::NONE);
        }
        notification->setSeverity(Notification::Severity::SUCCESS);
    }catch(const std::exception& ex){
        notification->setSeverity(Notification::Severity::ERROR);
        notification->setDescription(ex.what());
    }
    
}