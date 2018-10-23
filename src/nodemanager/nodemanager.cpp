#include <iostream>
#include <vector>
#include <string>
#include <signal.h>
#include <boost/program_options.hpp>

#include "deploymentmanager.h"
#include "executionmanager.h"

#include "cmakevars.h"
#include <util/execution.hpp>
#include <comms/environmentrequester/environmentrequester.h>

std::string VERSION_NAME = "re_node_manager";

Execution execution;

void signal_handler(int sig)
{
    execution.Interrupt();
}

void GotValidVerbosity(int v){
    if(v < 0 || v > 10){
        throw std::invalid_argument("Verbosity " + std::to_string(v) + " invalid (Valid range 0 to 10)");
    }
}

void GotValidDuration(int d){
    if(!(d == -1 || d > 0)){
        throw std::invalid_argument("Duration " + std::to_string(d) + " invalid (Valid values -1 or > 0)");
    }
}

int main(int argc, char **argv){
    //Connect the SIGINT/SIGTERM signals to our handler.
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

    //Shared arguments
    std::string environment_manager_endpoint;
    std::string experiment_name;
    std::string ip_address;
    std::string container_id;
    int execution_duration = 60;
    int log_verbosity = 1;

    //Slave arguments
    std::string dll_path;

    boost::program_options::options_description options("Node manager options");
    
    //Add required arguments
    options.add_options()("address,a", boost::program_options::value<std::string>(&ip_address)->required(), "IP address for to identify this Node Manager.");
    options.add_options()("container-id,c", boost::program_options::value<std::string>(&container_id), "The name of the container for this Node Manager.");
    options.add_options()("experiment-name,n", boost::program_options::value<std::string>(&experiment_name)->required(), "Name of experiment.");
    options.add_options()("environment-manager,e", boost::program_options::value<std::string>(&environment_manager_endpoint)->required(), "Environment manager fully qualified endpoint ie. (tcp://192.168.111.230:20000).");
    
    //Add optional arguments
    options.add_options()("time,t", boost::program_options::value<int>(&execution_duration)->default_value(execution_duration)->notifier(&GotValidDuration), "Deployment Duration (In Seconds)");
    options.add_options()("log-verbosity,v", boost::program_options::value<int>(&log_verbosity)->default_value(log_verbosity)->notifier(&GotValidVerbosity), "Logging verbosity [0-10]");
    options.add_options()("library,l", boost::program_options::value<std::string>(&dll_path)->required(), "Model generated library path.");
    options.add_options()("help,h", "Display help");

    //Construct a variable_map
    boost::program_options::variables_map vm;

    try{
        //Parse Argument variables
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, options), vm);
        boost::program_options::notify(vm);
    }catch(const std::exception& e) {
        std::cerr << "Arg Error: " << e.what() << "\n" << std::endl;
        std::cout << options << std::endl;
        return 1;
    }

    //Display help
    if(vm.count("help")){
        std::cout << options << std::endl;
        return 0;
    }


    std::string master_publisher_endpoint;
    std::string master_registration_endpoint;
    std::string master_heartbeat_endpoint;
    bool is_master = false;
    bool is_slave = false;

    //Set the verbosity
    Print::Logger::get_logger().SetLogLevel(log_verbosity);

    try{
        auto reply = EnvironmentRequest::TryRegisterNodeManager(environment_manager_endpoint, experiment_name, ip_address);
        
        for(const auto& type : reply->types()){
            switch(type){
                case NodeManager::NodeManagerRegistrationReply::SLAVE:{
                    is_slave = true;
                    break;
                }
                case NodeManager::NodeManagerRegistrationReply::MASTER:{
                    is_master = true;
                    break;
                }
                default:
                    break;
            }
        }

        if(!is_master && !is_slave){
            return 0;
        }
        
        if(is_master){
            master_heartbeat_endpoint = reply->heartbeat_endpoint();
        }
        master_registration_endpoint = reply->master_registration_endpoint();
        master_publisher_endpoint = reply->master_publisher_endpoint();
    }catch(const std::exception& ex){
        std::cerr << "* Failed to Register with EnvironmentManager: " << ex.what() << std::endl;
        return 1;
    }

    std::cout << "-------[" + VERSION_NAME +" v" + RE_VERSION + "]-------" << std::endl;
    if(is_master){
        std::cout << "* Master:" << std::endl;
        std::cout << "** Endpoint: " << ip_address << std::endl;
        if(execution_duration == -1){
            std::cout << "** Duration: " << "Indefinite" << std::endl;
        }else{
            std::cout << "** Duration: " << execution_duration << std::endl;
        }
    }
    if(is_slave){
        std::cout << "* Slave:" << std::endl;
        std::cout << "** Endpoint: " << ip_address << std::endl;
        std::cout << "** Library Path: " << dll_path << std::endl;
    }
        

    std::unique_ptr<DeploymentManager> deployment_manager;
    std::unique_ptr<ExecutionManager> execution_manager;

    if(is_master){
        try{
            execution_manager = std::unique_ptr<ExecutionManager>(new ExecutionManager(execution, execution_duration, experiment_name, master_publisher_endpoint, master_registration_endpoint, master_heartbeat_endpoint));
        }catch(const std::exception& ex){
            std::cerr << "* Failed to construct ExecutionManager: " << ex.what() << std::endl;
            return 1;
        }
    }

    if(is_slave){
        try{
            deployment_manager = std::unique_ptr<DeploymentManager>(new DeploymentManager(execution, experiment_name, ip_address, container_id, master_publisher_endpoint, master_registration_endpoint, dll_path, is_master));
        }catch(const std::exception& ex){
            std::cerr << "* Failed to construct DeploymentManager: " << ex.what() << std::endl;
            return 1;
        }
    }
    

    execution.Start();
    return 0;
}