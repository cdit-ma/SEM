#include <iostream>
#include <vector>
#include <string>
#include "deploymentmanager.h"

#include "zmqmaster.h"
#include "zmqslave.h"

#include "controlmessage.pb.h"
#include <google/protobuf/message_lite.h>

int main(int argc, char **argv)
{
    //Get the library path from the argument variables
    std::string dll_path;
    std::string tcp_endpoint;
    std::string graphml_path;
    
    bool is_server = false;
    if(argc >= 2){
        tcp_endpoint = argv[1];
        dll_path = argv[2];
        if(argc == 4){
            graphml_path = argv[3];
            is_server = true;
        }
    }

    if(dll_path.empty()){
        std::cerr << "DLL Error: No DLL path provided" << std::endl;
        exit(1);
    }

    ZMQMaster* master = 0;
    ZMQSlave* slave = 0;
    NodeContainer* node_container = 0;
    DeploymentManager* deployment_manager = 0;

    if(!dll_path.empty()){
        //Construct a Deployment Manager to handle the Deployment
        deployment_manager = new DeploymentManager(dll_path);
        //Get the NodeContainer from the DLL
        node_container = deployment_manager->get_deployment();

        if(!node_container){
            std::cerr << "Cannot construct Deployment" << std::endl;
        }
    }

    //Start the Master/Slave
    if(is_server){
        std::cout << "Starting MASTER on " << tcp_endpoint << std::endl;
        master = new ZMQMaster(tcp_endpoint, graphml_path);
    }else{
        if(deployment_manager){
            std::cout << "Starting SLAVE on " << tcp_endpoint << std::endl;
            slave = new ZMQSlave(deployment_manager, tcp_endpoint);
        }
    }
    
    bool running = true;

    while(running){
        std::cout << "Enter Instruction: ";
        std::string command;
        std::getline(std::cin, command);
        
        if(command == "activate"){
            std::string name;
            std::cout << "Enter Component Name or *: ";
            std::getline(std::cin, name);
            if(name == "*"){
                node_container->activate_all();
            }else{
                node_container->activate(name);
            }
        }else if(command == "passivate"){
            std::string name;
            std::cout << "Enter Component Name or *: ";
            std::getline(std::cin, name);
            if(name == "*"){
                node_container->passivate_all();
            }else{
                node_container->passivate(name);
            }
        }else if(command == "startup"){
            node_container->startup();
        }else if(command == "quit" || command == "terminate"){
            running = false;
        }else if(command == "send" && master){
            std::string host;
            std::string action;
            std::cout << "Enter Node Name or *: ";
            std::getline(std::cin, host);

            std::cout << "Enter Action: ";
            std::getline(std::cin, action);

            NodeManager::ControlMessage_Type t;
            bool success = NodeManager::ControlMessage_Type_Parse(action, &t);

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

                master->send_action(host, cm);
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
        node_container->passivate_all();
        node_container->teardown();
    }

    //Free Memory
    delete deployment_manager;
    
    return 0;
}
