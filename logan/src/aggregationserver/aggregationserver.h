#ifndef LOGAN_AGGREGATIONSERVER_H
#define LOGAN_AGGREGATIONSERVER_H

#include "string"

#include <util/execution.hpp>

#include "databaseclient.h"
#include "experimenttracker.h"

#include "aggregationprotohandler.h"

#include <zmq/protoreceiver/protoreceiver.h>
#include <zmq/protorequester/protorequester.hpp>

// REVIEW(Jackson): Namespace all the things

class AggregationServer {
public:
    AggregationServer(const std::string& database_ip,
                      const std::string& password,
                      const std::string& environment_endpoint);

    // REVIEW(Jackson): This shouldn't be exposed functionality, move inside of constructor
    void InitialiseDatabase();

private:
    zmq::ProtoReceiver receiver_;
    std::shared_ptr<DatabaseClient> database_client_;

    std::unique_ptr<zmq::ProtoRequester> env_requester_;
    std::unique_ptr<ExperimentTracker> experiment_tracker_;

    std::unique_ptr<AggregationProtoHandler> nodemanager_protohandler_;
};

#endif
