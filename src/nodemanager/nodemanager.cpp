#include <iostream>
#include <vector>
#include <string>
#include <signal.h>
#include <boost/program_options.hpp>

#include "deploymentmanager.h"
#include "executionmanager.h"

#include "cmakevars.h"
#include <re_common/util/execution.hpp>

#include "zmq/registrant.h"
#include "zmq/registrar.h"

std::string VERSION_NAME = "re_node_manager";

Execution* exe = 0;

void signal_handler(int sig)
{
    exe->Interrupt();
}

int main(int argc, char **argv){
    //Connect the SIGINT/SIGTERM signals to our handler.
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

    exe = new Execution();

    //Shared arguments
    std::string environment_manager_endpoint;
    std::string experiment_id;
    bool live_logging = false;
    std::string address;

    //Master arguments
    std::string graphml_path;
    double execution_duration = 60.0;

    //Slave arguments
    std::string dll_path;

    boost::program_options::options_description options("Node manager options");
    //Add shared arguments
    options.add_options()("address,a", boost::program_options::value<std::string>(&address), "Node manager ip address.");
    options.add_options()("experiment-id,n", boost::program_options::value<std::string>(&experiment_id), "ID of experiment to start.");
    options.add_options()("environment-manager,e", boost::program_options::value<std::string>(&environment_manager_endpoint), "Environment manager fully qualified endpoint ie. (tcp://192.168.111.230:20000).");
    options.add_options()("live-logging,L", boost::program_options::value<bool>(&live_logging), "Live model logging toggle.");
    
    //Add master arguments
    options.add_options()("deployment,d", boost::program_options::value<std::string>(&graphml_path), "Deployment graphml file path.");
    options.add_options()("time,t", boost::program_options::value<double>(&execution_duration)->default_value(execution_duration), "Deployment Duration (In Seconds)");

    //Add slave arguments
    options.add_options()("library,l", boost::program_options::value<std::string>(&dll_path), "Model generated library path.");

    //Add other options
    options.add_options()("help,h", "Display help");

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

    //Display help
    if(vm.count("help")){
        std::cout << options << std::endl;
        return 0;
    }

    bool valid_args = true;

    //Check shared arguments
    if(environment_manager_endpoint.empty()){
        std::cerr << "Arg Error: re_node_manager requires a valid environment manager." << std::endl;
        valid_args = false;
    }
    if(experiment_id.empty()){
        std::cerr << "Arg Error: re_node_manager requires a valid experiment-id." << std::endl;
        valid_args = false;
    }
    if(address.empty()){
        std::cerr << "Arg Error: re_node_manager requires a valid ip address." << std::endl;
        valid_args = false;
    }

    bool is_master = !graphml_path.empty();
    if(is_master){
        if(graphml_path.empty()){
            std::cerr << "Arg Error: re_node_manager[Master] requires a graphml deployment to execute." << std::endl;
            valid_args = false;
        }
    }

    bool is_slave = !dll_path.empty();
    if(is_slave){
        if(dll_path.empty()){
            std::cerr << "Arg Error: re_node_manager[Slave] requires a library path to execute." << std::endl;
            valid_args = false;
        }
    }

    if(!is_master && !is_slave){
        std::cerr << "Arg Error: re_node_manager needs to be run in slave or master mode." << std::endl;
        valid_args = false;
    }

    if(!valid_args){
        std::cout << options << std::endl;
        return 1;
    }

    std::cout << "-------[" + VERSION_NAME +" v" + RE_VERSION + "]-------" << std::endl;
    if(is_master){
        std::cout << "* Master:" << std::endl;
        std::cout << "** Endpoint: " << address << std::endl;
        std::cout << "** Deployment Graphml: " << graphml_path << std::endl;
        if(execution_duration == -1){
            std::cout << "** Duration: " << "Indefinite" << std::endl;
        }else{
            std::cout << "** Duration: " << execution_duration << std::endl;
        }
    }
    if(is_slave){
        std::cout << "* Slave:" << std::endl;
        std::cout << "** Endpoint: " << address << std::endl;
        std::cout << "** Library Path: " << dll_path << std::endl;
    }


    std::unique_ptr<zmq::Registrar> master;
    std::unique_ptr<zmq::Registrant> slave;
    bool success = true;

    if(success && is_master){
        auto execution_manager = new ExecutionManager(address,
                                                    graphml_path,
                                                    execution_duration,
                                                    exe,
                                                    experiment_id,
                                                    environment_manager_endpoint);

        master = std::unique_ptr<zmq::Registrar>(new zmq::Registrar(execution_manager, address));

        success = execution_manager->IsValid();
    }

    if(success && is_slave){
        //Construct a Deployment Manager to handle the Deployment
        auto deployment_manager = new DeploymentManager(is_master, dll_path, exe, experiment_id, address, environment_manager_endpoint);
        slave = std::unique_ptr<zmq::Registrant>(new zmq::Registrant(deployment_manager));
    }

    if(success){
        //Use execution class to wait for interrupt
        exe->Start();
    }

    delete exe;
    return success ? 0 : 1;
}