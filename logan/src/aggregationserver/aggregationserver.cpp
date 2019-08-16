
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

#include <utility>

#include <signal.h>

#include <boost/program_options.hpp>

#include <google/protobuf/util/json_util.h>
#include <google/protobuf/util/time_util.h>
#include <random>

#include <zmq/protowriter/protowriter.h>

#include "utils.h"

#include "aggregationserver.h"
#include "databaseclient.h"
#include "experimenttracker.h"
#include "nodemanagerprotohandler.h"

#include <sqlstrings.h>

using google::protobuf::util::TimeUtil;

namespace progopt = boost::program_options;

Execution execution;

void signal_handler(int signal_value) { execution.Interrupt(); }

const int success_return_val = 0;
const int error_return_val = 1;

int main(int argc, char** argv)
{
    // Handle the SIGINT/SIGTERM signal
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Variables to store the input parameters
    std::string database_ip;
    std::string password;
    std::string environment_manager_endpoint;
    bool init_db;

    // Parse command line options
    progopt::options_description desc("Aggregation Server Options");
    desc.add_options()("ip-address,i",
                       progopt::value<std::string>(&database_ip)->multitoken()->required(),
                       "address of the postgres database (192.168.1.1)");
    desc.add_options()("password,p", progopt::value<std::string>(&password)->default_value(""),
                       "the password for the database");
    desc.add_options()("environment-manager,e",
                       progopt::value<std::string>(&environment_manager_endpoint)->required(),
                       "Environment manager fully qualified endpoint ie. "
                       "(tcp://192.168.111.230:20000).");
    desc.add_options()("init", progopt::bool_switch(&init_db));
    desc.add_options()("help,h", "Display help");

    // Construct a variable_map
    progopt::variables_map vm;

    try {
        // Parse Argument variables
        progopt::store(progopt::parse_command_line(argc, argv, desc), vm);
        progopt::notify(vm);
    } catch(progopt::error& e) {
        std::cerr << "Arg Error: " << e.what() << std::endl << std::endl;
        std::cout << desc << std::endl;
        return 1;
    }

    std::unique_ptr<AggregationServer> aggServer = std::unique_ptr<AggregationServer>(
        new AggregationServer(database_ip, password, environment_manager_endpoint));

    std::cout << "Started AggregationServer without throwing any exceptions" << std::endl;

    if(init_db) {
        std::cout << "Initialising database" << std::endl;
        aggServer->InitialiseDatabase();
    }

    execution.Start();

    // std::this_thread::sleep_for(std::chrono::milliseconds(200));

    std::cout << "Shutting down" << std::endl;

    return 0;
}

AggregationServer::AggregationServer(const std::string& database_ip,
                                     const std::string& password,
                                     const std::string& environment_endpoint)
{
    std::stringstream conn_string_stream;
    conn_string_stream << "dbname = postgres user = postgres ";
    conn_string_stream << "password = " << password << " hostaddr = " << database_ip
                       << " port = 5432";

    database_client = std::make_shared<DatabaseClient>(conn_string_stream.str());
    experiment_tracker = std::unique_ptr<ExperimentTracker>(new ExperimentTracker(database_client));

    nodemanager_protohandler = std::unique_ptr<AggregationProtoHandler>(
        new NodeManagerProtoHandler(database_client, *experiment_tracker));

    try {
        NodeManager::AggregationServerRegistrationRequest registration_request;
        env_requester = std::unique_ptr<zmq::ProtoRequester>(
            new zmq::ProtoRequester(environment_endpoint));
        auto reply = env_requester->SendRequest<NodeManager::AggregationServerRegistrationRequest,
                                                NodeManager::AggregationServerRegistrationReply>(
            "AggregationServerRegistration", registration_request, 3000);
        std::string publisher_endpoint = reply.get()->publisher_endpoint();

        nodemanager_protohandler->BindCallbacks(receiver);

        receiver.Connect(publisher_endpoint);
        receiver.Filter("");
    } catch(const std::exception& e) {
        throw std::runtime_error(std::string("Failed to start protoreceiver: ") + e.what());
    }
}

void AggregationServer::InitialiseDatabase()
{
    // TODO: Handle exception thrown by this call
    database_client->InitSchemaFrom(re::logan::sqlstrings::init_db_schema());
}
