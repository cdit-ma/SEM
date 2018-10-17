#include "aggregationproxy.h"
#include "../../Controllers/NotificationManager/notificationmanager.h"
#include "../../Controllers/NotificationManager/notificationobject.h"

#include <iostream>
#include <QFutureWatcher>
#include <QtConcurrent>

AggregationProxy::AggregationProxy() :
    requester_("tcp://192.168.111.249:12345")
{
}


void AggregationProxy::RequestRunningExperiments()
{
    auto notification = NotificationManager::manager()->AddNotification("Requesting Port Lifecycle", "Icons", "buildingPillared", Notification::Severity::RUNNING, Notification::Type::APPLICATION, Notification::Category::NONE);

    try {
        // setup request
        AggServer::PortLifecycleRequest request_type;
        /*
        request_type.add_time_interval();
        request_type.add_port_paths("poop");
        request_type.add_component_instance_paths("");
        request_type.add_component_names("");
        */

        /*
        auto futureResult = getPortLifecycle(request_type);
        QFutureWatcher* watcher = new QFutureWatcher(this);
        watcher->setFuture(futureResult);
        //QFutureWatcher<std::unique_ptr<AggServer::PortLifecycleResponse> > watcher;
        connect(watcher, &QFutureWatcher::finished, [=]()
        {
            qInfo("AggregationProxy::RequestRunningExperiments - Watched future is finished.");
            emit resultsReady();

            auto results = futureResult.results();
            for (auto result : results) {
                qInfo() << QString(result.DebugString().c_str());
            }
            notification->setSeverity(Notification::Severity::SUCCESS);
        });*/

        //QFuture<void> future = requester_.GetPortLifecycle(request_type);
        //watcher->setFuture(future);;


        auto result = requester_.GetPortLifecycle(request_type);
        qCritical() << QString(result->DebugString().c_str());
        notification->setSeverity(Notification::Severity::SUCCESS);

    } catch (const std::exception& ex) {
        notification->setSeverity(Notification::Severity::ERROR);
        notification->setDescription(ex.what());
    }
}

/*
QFuture<std::unique_ptr<AggServer::PortLifecycleResponse> > AggregationProxy::getPortLifecycle(const AggServer::PortLifecycleRequest request)
{
    auto getLifecycleWorker = [=](const AggServer::PortLifecycleRequest request){
        return requester_.GetPortLifecycle(request);
    };
    return QtConcurrent::run(getLifecycleWorker, request);
}
*/
