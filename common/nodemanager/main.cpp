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
    std::string lib_path;
    std::string host_name;
    std::string port;
    bool is_server = false;
    if(argc >= 4){
        host_name = argv[1];
        lib_path = argv[2];
        port = argv[3];
        if(argc == 5){
            is_server = true;
        }
    }

    if(lib_path.empty()){
        std::cerr << "DLL Error: No DLL path provided" << std::endl;
        exit(1);
    }

    DeploymentManager* manager = new DeploymentManager(lib_path);


    ZMQMaster* m = 0;
    ZMQSlave* s = 0;

    std::string my_ip = "tcp://192.168.111.187";
    if(is_server){
        std::cout << "Is Server" << std::endl;
        std::vector<std::string> slaves;
        slaves.push_back("tcp://192.168.111.187:7001");
        slaves.push_back("tcp://192.168.111.187:7002");
        //slaves.push_back("tcp://192.168.111.84:7001");
        //slaves.push_back("tcp://192.168.111.84:7002");
        m = new ZMQMaster(host_name, my_ip + ":" + port, slaves);
    }else{
        s = new ZMQSlave(manager, host_name, my_ip + ":" + port);
    }

    //Construct an instance of the Deployment
    NodeContainer* instance = manager->get_deployment();

    if(!instance){
        std::cerr << "Cannot construct Deployment" << std::endl;

        //exit(1);
    }else{
        //Start deployment instance
        //instance->startup();
    }
    
    //Wait for a period of time before trying to send
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    
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
                instance->activate_all();
            }else{
                instance->activate(name);
            }
        }else if(command == "passivate"){
            std::string name;
            std::cout << "Enter Component Name or *: ";
            std::getline(std::cin, name);
            if(name == "*"){
                instance->passivate_all();
            }else{
                instance->passivate(name);
            }
        }else if(command == "quit"){
            running = false;
        }else if(command == "terminate"){
            if(m){
                delete m;
            }else if(s){
                delete s;
            }
            
            running = false;
        }else if(command == "send" && m){
            std::string host;
            std::string action;
            std::cout << "Enter Component Name or *: ";
            std::getline(std::cin, host);
            std::cout << "Enter Action : ";
            std::getline(std::cin, action);

            NodeManager::ControlMessage_Type* t = new NodeManager::ControlMessage_Type();
            bool success = NodeManager::ControlMessage_Type_Parse(action, t);

            NodeManager::ControlMessage* cm = new NodeManager::ControlMessage();
            //cm->set_type(NodeManager::ControlMessage::STARTUP);
            cm->set_type(*t);
            cm->mutable_node()->set_name(host);
            m->send_action(host, cm);
        }
    }
  
    if(instance){
        //Teardown deployment instance
        instance->passivate_all();
        instance->teardown();
    }

    //Free Memory
    delete manager;
    
    return 0;
}