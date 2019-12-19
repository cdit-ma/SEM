// REVIEW (Mitch): This file is unused
#include <iostream>
#include <vector>
#include <string>
#include <boost/program_options.hpp>

#include "deploymentmanager.h"
#include "executionmanager.h"
#include <util/execution.hpp>

#include "zmq/registrant.h"
#include "zmq/registrar.h"

int main(int argc, char **argv){
    //Get the library path from the argument variables
    std::string graphml_path;

    boost::program_options::options_description options("Node manager options");
    options.add_options()("deployment,d", boost::program_options::value<std::string>(&graphml_path), "Deployment graphml file path");

    //Construct a variable_map
    boost::program_options::variables_map vm;

    try{
        //Parse Argument variables
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, options), vm);
        boost::program_options::notify(vm);
    }catch(boost::program_options::error& e) {
        std::cerr << "Arg Error: " << e.what() << std::endl << std::endl;
        std::cout << options << std::endl;
        return 1;
    }

    ProtobufModelParser parser(graphml_path, "experiment_id");

    std::cout << parser.GetDeploymentJSON() << std::endl;


    // ExecutionManager em("", graphml_path, 0, 0, "experiment_id", "endpoint");
    // for(auto message : em.getNodeStartupMessage()){
    //     std::cout << message->DebugString() << std::endl;
    // }
    return 0;
}
