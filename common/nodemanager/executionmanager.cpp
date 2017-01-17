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


    //get component instances
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
        std::string node_name = graphml_parser_->get_data_value(hardware_id, "label");
        std::string node_ip = graphml_parser_->get_data_value(hardware_id, "ip_address");
                
        if(deployment_map.count(hardware_id)){
            auto ele = deployment_map[hardware_id]->mutable_node();

            NodeManager::Component* component = ele->add_components();
            component->set_id(c);
            component->set_name(label);
            NodeManager::Attribute* ip_address = component->add_attributes();
            ip_address->set_name("ip_address");
            ip_address->set_type(NodeManager::Attribute::STRING);
            ip_address->set_s(node_ip);
            auto attribute_instances = graphml_parser_->find_nodes("AttributeInstance", c);

            //get component attribute instances
            for(auto attri : attribute_instances){
                std::cout << "Attribute ID: " << attri << std::endl;
                NodeManager::Attribute* a = component->add_attributes();
                a->set_name(graphml_parser_->get_data_value(attri, "label"));

                std::string type_string = graphml_parser_->get_data_value(attri, "type");
                std::string value_string = graphml_parser_->get_data_value(attri, "value");

                if(type_string == "DoubleNumber" || type_string == "Float"){
                    a->set_type(NodeManager::Attribute::DOUBLE);
                    a->set_d(std::stod(value_string));
                } else if(type_string.find("String")!=std::string::npos){
                    a->set_type(NodeManager::Attribute::STRING);
                    a->set_s(value_string);
                } else {
                    a->set_type(NodeManager::Attribute::INTEGER);
                    a->set_i(std::stoi(value_string));
                }

            }
        }
    }




    for(auto a: deployment_map){
        std::cout << a.second->DebugString() << std::endl;
    }

}