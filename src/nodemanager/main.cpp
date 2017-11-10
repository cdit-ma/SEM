#include <iostream>
#include <vector>
#include <string>
#include <signal.h>
#include <boost/program_options.hpp>

#include "controlmessage/controlmessage.pb.h"

#include "deploymentmanager.h"
#include "executionmanager.h"

#include "cmakevars.h"
#include "execution.hpp"

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
    //Get the library path from the argument variables
    std::string dll_path;
    std::string graphml_path;
    std::string slave_endpoint;
    std::string master_endpoint;
    double execution_duration = 60.0;
    bool live_logging = false;

    boost::program_options::options_description options("Node manager options");
    options.add_options()("library,l", boost::program_options::value<std::string>(&dll_path), "Library path");
    options.add_options()("deployment,d", boost::program_options::value<std::string>(&graphml_path), "Deployment graphml file path");
    options.add_options()("time,t", boost::program_options::value<double>(&execution_duration)->default_value(execution_duration), "Deployment Duration (In Seconds)");
    options.add_options()("slave,s", boost::program_options::value<std::string>(&slave_endpoint), "Slave endpoint, including port");
    options.add_options()("master,m", boost::program_options::value<std::string>(&master_endpoint), "Master endpoint, including port");
    options.add_options()("live-logging,L", boost::program_options::value<bool>(&live_logging), "Master endpoint, including port");
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

    //display help
    if(vm.count("help")){
		std::cout << options << std::endl;
		return 0;
	}

    bool valid_args = true;

    bool is_master = !master_endpoint.empty() || !graphml_path.empty();
    bool is_slave = !slave_endpoint.empty() || !dll_path.empty();

    if(is_master){
        if(master_endpoint.empty()){
            std::cerr << "Arg Error: re_node_manager[Master] requires a valid endpoint." << std::endl;
            valid_args = false;
        }
        if(graphml_path.empty()){
            std::cerr << "Arg Error: re_node_manager[Master] requires a graphml deployment to execute." << std::endl;
            valid_args = false;
        }
    }
    if(is_slave){
        if(slave_endpoint.empty()){
            std::cerr << "Arg Error: re_node_manager[Slave] requires a valid endpoint." << std::endl;
            valid_args = false;
        }
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
        std::cout << "** Endpoint: " << master_endpoint << std::endl;    
        std::cout << "** Deployment Graphml: " << graphml_path << std::endl;    
        std::cout << "** Duration: " << execution_duration << std::endl;    
    }
    if(is_slave){
        std::cout << "* Slave:" << std::endl;    
        std::cout << "** Endpoint: " << slave_endpoint << std::endl;    
        std::cout << "** Library Path: " << dll_path << std::endl;    
    }

    zmq::Registrar* master = 0;
    zmq::Registrant* slave = 0;
    NodeContainer* node_container = 0;
    ExecutionManager* execution_manager = 0;
    DeploymentManager* deployment_manager = 0;

    if(is_slave){
        //Construct a Deployment Manager to handle the Deployment
        deployment_manager = new DeploymentManager(dll_path, exe);
        //Get the NodeContainer from the DLL
        node_container = deployment_manager->get_deployment();
        slave = new zmq::Registrant(deployment_manager, slave_endpoint);
    }

    if(is_master){
        execution_manager = new ExecutionManager(master_endpoint, graphml_path, execution_duration, exe);
        master = new zmq::Registrar(execution_manager, master_endpoint);
    }

    //Use execution class to wait for interrupt
    exe->Start();

    if(is_slave){
        delete slave;
    }


    if(is_master){
        delete master;
        delete execution_manager;
    }

    delete exe;
   
    return 0;
}
