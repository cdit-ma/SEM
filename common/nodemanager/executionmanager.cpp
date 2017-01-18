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

        //find deployment location of component
        for(auto e : deployment_edges){
            std::string source = graphml_parser_->get_attribute(e, "source");
            if(source == c){
                hardware_id = graphml_parser_->get_attribute(e, "target");
                break;
            }
        }
        std::string node_name = graphml_parser_->get_data_value(hardware_id, "label");

        //create control message for node
        if(deployment_map.count(hardware_id)){
            auto ele = deployment_map[hardware_id]->mutable_node();

            NodeManager::Component* component = ele->add_components();
            component->set_id(c);
            component->set_name(label);

            //get component attribute instances
            auto attribute_instances = graphml_parser_->find_nodes("AttributeInstance", c);
            for(auto attri : attribute_instances){
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

            //get component ports
            auto port_instances = graphml_parser_->find_nodes("OutEventPortInstance", c);
            auto in_port_instances = graphml_parser_->find_nodes("InEventPortInstance", c);

            //Combine into one list to reduce code duplication            
            port_instances.insert(port_instances.end(), in_port_instances.begin(), in_port_instances.end());

            for(auto port: port_instances){
                NodeManager::EventPort* p = component->add_ports();
                p->set_name(graphml_parser_->get_data_value(port, "label"));
                std::string port_kind = graphml_parser_->get_data_value(port, "kind");

                if(port_kind == "OutEventPortInstance"){
                    p->set_type(NodeManager::EventPort::OUT);
                } else if(port_kind == "InEventPortInstance"){
                    p->set_type(NodeManager::EventPort::IN);                    
                }

                //parse middleware
                std::string port_middleware = graphml_parser_->get_data_value(port, "middleware");
                if(port_middleware == "RTI_DDS"){
                    p->set_middleware(NodeManager::EventPort::RTI_DDS);
                }else if(port_middleware == "QPID"){
                    p->set_middleware(NodeManager::EventPort::QPID);
                }else if(port_middleware == "ZMQ"){
                    p->set_middleware(NodeManager::EventPort::ZMQ);
                }else if(port_middleware == "OSPL_DDS"){
                    p->set_middleware(NodeManager::EventPort::OSPL_DDS);
                }else{
                    p->set_middleware(NodeManager::EventPort::UNKNOWN);
                }

                //Set port ip addr
                NodeManager::Attribute* ip_address = p->add_attributes();
                std::string node_ip = graphml_parser_->get_data_value(hardware_id, "ip_address");
                ip_address->set_name("ip_address");
                ip_address->set_type(NodeManager::Attribute::STRING);
                ip_address->set_s(node_ip);

                //Set port port number
                NodeManager::Attribute* port_port = p->add_attributes();
                std::string port_port_s = graphml_parser_->get_data_value(hardware_id, "ip_address");
                port_port->set_name("port_number");
                port_port->set_type(NodeManager::Attribute::STRING);
                
                //TODO: actually set unique port number.
                port_port->set_s("60000");

                //Set port port number
                NodeManager::Attribute* topic_attr = p->add_attributes();
                topic_attr->set_name("topic");
                topic_attr->set_type(NodeManager::Attribute::STRING);
                
                //TODO: actually set unique port number.
                topic_attr->set_s("a");

            }
        }
    }

    for(auto a: deployment_map){
        std::cout << a.second->DebugString() << std::endl;
    }
}