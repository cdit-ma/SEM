#include "executionmanager.h"
#include <iostream>
#include <map>
#include "controlmessage.pb.h"

ExecutionManager::ExecutionManager(ZMQMaster* zmq, std::string graphml_path){
    zmq_master_ = zmq;
    graphml_parser_ = new GraphmlParser(graphml_path);
    execution_loop();
}

void ExecutionManager::execution_loop(){
    
    auto component_instances = graphml_parser_->find_nodes("ComponentInstance");
    auto deployment_edges = graphml_parser_->find_edges("Edge_Deployment");
    auto hardware_nodes = graphml_parser_->find_nodes("HardwareNode");

    //std::set<std::string> deployed_hardware;

    std::map<std::string, NodeManager::ControlMessage*> deployment_map;

    //Get the Deployed nodes.
    for(auto h : hardware_nodes){
        
        for(auto e : deployment_edges){
            auto target = graphml_parser_->get_attribute(e, "target");
            if(h == target && deployment_map.count(h) == 0){
                
                //Allocate our Control Message Objects
                deployment_map[h] = new NodeManager::ControlMessage();

                //Get Hardware Information
                std::string node_name = graphml_parser_->get_data_value(h, "label");
                deployment_map[h]->mutable_node()->set_name(node_name);
                break;
            }
        }
    }


    for(auto c : component_instances){
        std::string label = graphml_parser_->get_data_value(c, "label");
        std::string hardware_id;

        for(auto e : deployment_edges){
            std::string source = graphml_parser_->get_attribute(e, "source");
            if(source == c){
                hardware_id = graphml_parser_->get_attribute(e, "target");
                break;
            }
        }
        std::string node_name = graphml_parser_->get_data_value(hardware_id, label);
                
        if(deployment_map.count(hardware_id)){
            auto ele = deployment_map[hardware_id]->mutable_node();

            NodeManager::Component* component = ele->add_components();
            component->set_id(c);
            component->set_name(label);
        }
    }




    for(auto a: deployment_map){
        std::cout << a.second->DebugString() << std::endl;
    }

}