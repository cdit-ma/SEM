#ifndef AGGREGATIONBROKER_H
#define AGGREGATIONBROKER_H

#include <string>
#include <memory>

#include "../databaseclient.h"
#include "aggregationreplier.h"

namespace AggServer {

class AggregationBroker {
public:
    AggregationBroker(const std::string& receiver_ip,
        const std::string& database_ip,
        const std::string& password);

        
private:
    
    std::unique_ptr<AggregationReplier> replier;
    std::shared_ptr<DatabaseClient> database_client;
};

}

#endif //AGGREGATIONBROKER_H
