#include "aggregationprotohandler.h"

#include <utility>

#include "databaseclient.h"
#include "experimenttracker.h"

AggregationProtoHandler::AggregationProtoHandler(std::shared_ptr<DatabaseClient> db_client, ExperimentTracker& exp_tracker) :
    database_(std::move(db_client)),
    experiment_tracker_(exp_tracker){};
