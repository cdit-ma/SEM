#include "aggregationbroker.h"

AggServer::AggregationBroker::AggregationBroker(const std::string& replier_ip,
        const std::string& database_ip,
        const std::string& password) {

    std::stringstream conn_string_stream;
    conn_string_stream << "dbname = postgres user = postgres ";
    conn_string_stream << "password = " << password << " hostaddr = " << database_ip << " port = 5432";
    
    
    replier = std::unique_ptr<AggServer::AggregationReplier>(
        new AggServer::AggregationReplier(std::make_shared<DatabaseClient>(conn_string_stream.str()))
    );

    replier->Bind(replier_ip);
    replier->Start();
}