#include "aggregationrequester.h"
#include <iostream>
#include <boost/program_options.hpp>
#include <google/protobuf/util/json_util.h>

int main(int argc, char** argv){
    std::string aggregation_broker_endpoint;


    boost::program_options::options_description desc("Aggregation Requester Options");
    desc.add_options()("aggregation-broker,a", boost::program_options::value<std::string>(&aggregation_broker_endpoint)->required(), "TCP endpoint of Aggregation Broker to connect to.");
    desc.add_options()("help,h", "Display help");

    //Construct a variable_map
    boost::program_options::variables_map vm;

    try{
        //Parse Argument variables
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
        boost::program_options::notify(vm);
    }catch(const boost::program_options::error& e) {
        std::cerr << "Arg Error: " << e.what() << std::endl << std::endl;
        std::cout << desc << std::endl;
        return 1;
    }
    if(vm.count("help")){
        std::cout << desc << std::endl;
        return 0;
    }

    AggServer::Requester requester(aggregation_broker_endpoint);

    try{
        AggServer::PortLifecycleRequest request;
        //TODO: FILL REQUEST
        
        auto result = requester.GetPortLifecycle(request);
        
        google::protobuf::util::JsonOptions options;
        options.add_whitespace = true;

        if(result){
            std::string output;
            google::protobuf::util::MessageToJsonString(*result, &output, options);
            std::cout << output << std::endl;
        }
    }catch(const std::exception& ex){
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}