#include <iostream>
#include <vector>
#include <string>
#include <boost/program_options.hpp>

#include "../controlmessage/controlmessage.pb.h"

#include "deploymentmanager.h"
#include "executionmanager.h"

#include "zmq/registrant.h"
#include "zmq/registrar.h"

std::string VERSION_NAME = "re_node_manager";
std::string VERSION_NUMBER = "1.0";

int main(int argc, char **argv){

    //Get the library path from the argument variables
    std::string dll_path;
    std::string graphml_path;
    std::string slave_endpoint;
    std::string master_endpoint;
    double execution_duration = 60.0;

    boost::program_options::options_description options("Node manager options");
    options.add_options()("library,l", boost::program_options::value<std::string>(&dll_path), "Library path");
    options.add_options()("deployment,d", boost::program_options::value<std::string>(&graphml_path), "Deployment graphml file path");
    options.add_options()("time,t", boost::program_options::value<double>(&execution_duration)->default_value(execution_duration), "Deployment Duration (In Seconds)");
    options.add_options()("slave,s", boost::program_options::value<std::string>(&slave_endpoint), "Slave endpoint, including port");
    options.add_options()("master,m", boost::program_options::value<std::string>(&master_endpoint), "Master endpoint, including port");
    options.add_options()("help,h", "Display help");

    boost::program_options::variables_map options_map;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, options), options_map);
	boost::program_options::notify(options_map);

    //display help
    if(options_map.count("help")){
		std::cout << options << std::endl;
		return 0;
	}

    //If we have graphml input, we are a server
    bool is_server = false;
    if(!graphml_path.empty()){
        is_server = true;
        if(master_endpoint.empty()){
            std::cerr << "Endpoint Error: Deployment graphml specified but no master endpoint given." << std::endl;
            std::cout << options << std::endl;
            return 1;
        }
    }else{
        if(slave_endpoint.empty()){
            std::cerr << "Endpoint Error: No slave endpoint found" << std::endl;
            std::cout << options << std::endl;
            return 1;
        }
    }

    zmq::Registrar* master = 0;
    zmq::Registrant* slave = 0;
    
    NodeContainer* node_container = 0;
    ExecutionManager* execution_manager = 0;
    DeploymentManager* deployment_manager = 0;

    if(!dll_path.empty()){
        //Construct a Deployment Manager to handle the Deployment
        deployment_manager = new DeploymentManager(dll_path);
        //Get the NodeContainer from the DLL
        node_container = deployment_manager->get_deployment();
    }else{
        std::cerr << "DLL Error: No library path given." << std::endl;
        std::cout << options << std::endl;
    }

    std::cout << "-------[" + VERSION_NAME +" v" + VERSION_NUMBER + "]-------" << std::endl;
    std::cout << "* Library path: " << dll_path << std::endl << std::endl;
    std::cout << "* Constructed Deployment Manager" << std::endl;

    //Start the Master/Slave
    if(is_server){
        execution_manager = new ExecutionManager(master_endpoint, graphml_path, execution_duration);
        std::cout << "* Started ExecutionManager" << std::endl;
        master = new zmq::Registrar(execution_manager, master_endpoint);
        std::cout << "* Started Registrar" << std::endl;        
    }
    
    if(deployment_manager){
        slave = new zmq::Registrant(deployment_manager, slave_endpoint);
        std::cout << "* Started Registrant" << std::endl;        
    }
	std::cout << "---------------------------------" << std::endl;
    
    bool running = true;

    while(running){
        std::cout << "Enter Instruction: ";
        std::string command;
        std::getline(std::cin, command);
        
        if(command == "ACTIVATE"){
            execution_manager->ActivateExecution();
        }else if(command == "TERMINATE"){
            execution_manager->TerminateExecution();
        }else if(command == "activate"){
            std::string name;
            std::cout << "Enter Component Name or *: ";
            std::getline(std::cin, name);
            if(name == "*"){
                node_container->ActivateAll();
            }else{
                node_container->Activate(name);
            }
        }else if(command == "passivate"){
            std::string name;
            std::cout << "Enter Component Name or *: ";
            std::getline(std::cin, name);
            if(name == "*"){
                node_container->PassivateAll();
            }else{
                node_container->Passivate(name);
            }
        }else if(command == "quit" || command == "terminate"){
            running = false;
        }else if(command == "send" && master){
            std::string host;
            std::string action;
            std::cout << "Enter Node Name or *: ";
            std::getline(std::cin, host);

            std::cout << "Enter Action: ";
            std::getline(std::cin, action);
            
            NodeManager::ControlMessage::Type t;
            bool success = NodeManager::ControlMessage::Type_Parse(action, &t);

            if(success){
                NodeManager::ControlMessage* cm = new NodeManager::ControlMessage();
                cm->mutable_node()->set_name(host);
                cm->set_type(t);

                if(t == NodeManager::ControlMessage::SET_ATTRIBUTE){
                    std::string attribute_component;
                    std::string attribute_name;
                    std::string attribute_value;
                    
                    std::cout << "Set Component Name: ";
                    std::getline(std::cin, attribute_component);
                    std::cout << "Set Attribute Name: ";
                    std::getline(std::cin, attribute_name);
                    std::cout << "Set Attribute Value: ";
                    std::getline(std::cin, attribute_value);

                    auto component = cm->mutable_node()->add_components();
                    component->set_name(attribute_component);
                    auto attr = component->add_attributes();
                    attr->set_name(attribute_name);
                    attr->set_type(NodeManager::Attribute::STRING);
                    attr->add_s(attribute_value);
                }

                execution_manager->PushMessage(host, cm);
            }
        }
    }

    if(master){
        delete master;
    }
    if(slave){
        delete slave;
    }
  
    if(node_container){
        //Teardown deployment instance
        node_container->PassivateAll();
        node_container->Teardown();
    }

    //Free Memory
    delete deployment_manager;
    
    std::cout << "PASSIVATING LOGGER" << std::endl;
    //ModelLogger::shutdown_logger();
    
    return 0;
}
