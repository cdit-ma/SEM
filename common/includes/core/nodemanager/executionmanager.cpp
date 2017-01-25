#include "executionmanager.h"
#include <iostream>
#include <map>
#include "controlmessage.pb.h"
#include "zmqmaster.h"

void set_attr_string(NodeManager::Attribute* attr, std::string val){
    attr->add_s(val);
}

ExecutionManager::ExecutionManager(ZMQMaster* zmq, std::string graphml_path){
    zmq_master_ = zmq;
    graphml_parser_ = new GraphmlParser(graphml_path);
    if(scrape_document()){
        std::cout << "Got valid GraphML" << std::endl;
    }
}

std::vector<std::string> ExecutionManager::get_slave_endpoints(){
    std::vector<std::string> out;
    for(auto hardware : hardware_nodes_){
        if(hardware.second->component_ids.size() != 0){
            std::string ip = "tcp://" + hardware.second->ip_address + ":" + std::to_string(hardware.second->node_manager_port);
            out.push_back(ip);
        }
    }
    return out;
}

std::string ExecutionManager::get_host_name_from_address(std::string address){
    for(auto hardware : hardware_nodes_){
        if(address.find(hardware.second->ip_address) != std::string::npos){
            return hardware.second->name;
        }
    }
    return "";
}

ExecutionManager::HardwareNode* ExecutionManager::get_hardware_node(std::string id){
    ExecutionManager::HardwareNode* node = 0;

    if(hardware_nodes_.count(id)){
        node = hardware_nodes_[id];
    }
    return node;
}
ExecutionManager::ComponentInstance* ExecutionManager::get_component(std::string id){
    ExecutionManager::ComponentInstance* node = 0;

    if(component_instances_.count(id)){
        node = component_instances_[id];
    }
    return node;
}
ExecutionManager::EventPort* ExecutionManager::get_event_port(std::string id){
    ExecutionManager::EventPort* node = 0;

    if(event_ports_.count(id)){
        node = event_ports_[id];
    }
    return node;
}
ExecutionManager::Attribute* ExecutionManager::get_attribute(std::string id){
    ExecutionManager::Attribute* node = 0;

    if(attributes_.count(id)){
        node = attributes_[id];
    }
    return node;
}

bool ExecutionManager::scrape_document(){
    if(graphml_parser_){
        deployment_edge_ids_ = graphml_parser_->find_edges("Edge_Deployment");
        assembly_edge_ids_ = graphml_parser_->find_edges("Edge_Assembly");
        
        for(auto e_id: deployment_edge_ids_){
            auto source_id = get_attribute(e_id, "source");
            auto target_id = get_attribute(e_id, "target");
            //Component->Node
            deployed_instance_map_[source_id] = target_id;
        }

        //Parse Hardware Node
        for(auto n_id : graphml_parser_->find_nodes("HardwareNode")){
            auto node = new HardwareNode();
            //Set the Node information
            node->id = n_id;
            node->name = get_data_value(n_id, "label");
            node->ip_address = get_data_value(n_id, "ip_address");        

            for(auto e_id : deployment_edge_ids_){
                auto target_id = get_attribute(e_id, "target");

                if(n_id == target_id){
                    auto source_id = get_attribute(e_id, "source");
                    auto source_kind = get_data_value(source_id, "kind");
                    if(source_kind == "ComponentInstance"){
                        node->component_ids.push_back(source_id);
                    }
                }
            }

            if(hardware_nodes_.count(n_id) == 0){
                hardware_nodes_[n_id] = node;
            }else{
                std::cout << "Got Duplicate Nodes: " << n_id << std::endl;
            }
        }

        for(auto c_id : graphml_parser_->find_nodes("ComponentInstance")){
            auto component = new ComponentInstance();
            //Set the Node information
            component->id = c_id;
            component->name = get_data_value(c_id, "label");

            HardwareNode* node = 0;
            if(deployed_instance_map_.count(c_id)){
                //Get the Node ID
                component->node_id = deployed_instance_map_[c_id];
                node = get_hardware_node(component->node_id);
            }
            
            //Get Attributes
            for(auto a_id : graphml_parser_->find_nodes("AttributeInstance", c_id)){
                auto attribute = new Attribute();
                attribute->id = a_id;
                attribute->component_id = c_id;
                attribute->name = get_data_value(a_id, "label");

                if(attributes_.count(a_id) == 0){
                    attributes_[a_id] = attribute;
                }else{
                    std::cout << "Got Duplicate Attribute: " << a_id << std::endl;
                }
                //Add the Attribute to the Component
                component->attribute_ids.push_back(a_id);
            }

            //Get the Event ports
            auto port_ids = graphml_parser_->find_nodes("OutEventPortInstance", c_id);
            {
                auto in_port_ids = graphml_parser_->find_nodes("InEventPortInstance", c_id);
                //Combine into one list to reduce code duplication            
                port_ids.insert(port_ids.end(), in_port_ids.begin(), in_port_ids.end());
            }

            for(auto p_id: port_ids){
                auto port = new EventPort();

                //setup the port
                port->id = p_id;
                port->component_id = c_id;
                port->name = get_data_value(p_id, "label");
                port->topic_name = get_data_value(p_id, "topicName");
                port->kind = get_data_value(p_id, "kind");
                port->middleware = get_data_value(p_id, "middleware");
                //TODO: OVERRIDDEN
                port->middleware = "ZMQ";

                //Register Only OutEventPortInstances
                if(port->kind == "OutEventPortInstance"){
                    //Setup Port number
                    if(node && port->middleware == "ZMQ"){
                        //Set Port Number only for ZMQ
                        node->port_count ++;
                        port->port_number = node->port_count;
                        //Construct a TCP Address
                        port->port_address = "tcp://" + node->ip_address + ":" + std::to_string(port->port_number);
                    }

                }

                if(event_ports_.count(p_id) == 0){
                    event_ports_[p_id] = port;
                }else{
                    std::cout << "Got Duplicate EventPort: " << p_id << std::endl;
                }

                //Add the Attribute to the Component
                component->event_port_ids.push_back(p_id);
            }

            if(component_instances_.count(c_id) == 0){
                component_instances_[c_id] = component;
            }else{
                std::cout << "Got Duplicate Component: " << c_id << std::endl;
            }
        }
    }

    if(deployment_edge_ids_.size() > 0){
        //TODO: Add fail cases
        return true;
    } 
    return false;
}

std::string ExecutionManager::get_attribute(std::string id, std::string attr_name){
    if(graphml_parser_){
        return graphml_parser_->get_attribute(id, attr_name);
    }
    return "";
}
std::string ExecutionManager::get_data_value(std::string id, std::string key_name){
    if(graphml_parser_){
        return graphml_parser_->get_data_value(id, key_name);
    }
    return "";
}

void ExecutionManager::execution_loop(){
    std::map<std::string, NodeManager::ControlMessage*> deployment_map;

    //Get the Deployed Hardware nodes. Construct STARTUP messages
    for(auto n : hardware_nodes_){
        HardwareNode* node = n.second;

        if(node && !node->component_ids.empty()){
            auto cm = new NodeManager::ControlMessage();
            cm->mutable_node()->set_name(node->name);
            //Fake STARTUP Message
            cm->set_type(NodeManager::ControlMessage::STARTUP);
            deployment_map[node->id] = cm;
        }
    }

    //get component instances
    for(auto d : deployed_instance_map_){
        std::string c_id = d.first;
        std::string h_id = d.second;

        NodeManager::Node* node_pb = 0;
        if(deployment_map.count(h_id)){
            node_pb = deployment_map[h_id]->mutable_node();
        }

        auto component = get_component(c_id);
        auto hardware_node = get_hardware_node(h_id);

        if(hardware_node && component && node_pb){
            std::cout << "Node: " << hardware_node->name << " Deploys: " << component->name << std::endl;
            
            NodeManager::Component* component_pb = node_pb->add_components();
            
            component_pb->set_id(component->id);
            component_pb->set_name(component->name);

            //Get the Component Attributes
            for(auto a_id : component->attribute_ids){
                auto attribute = get_attribute(a_id);
                if(attribute){
                    auto attr_pb = component_pb->add_attributes();

                    attr_pb->set_name(get_data_value(a_id, "label"));

                    auto type = get_data_value(a_id, "type");
                    auto value = get_data_value(a_id, "value");

                    if(type == "DoubleNumber" || type == "Float"){
                        attr_pb->set_type(NodeManager::Attribute::DOUBLE);
                        attr_pb->set_d(std::stod(value));
                    } else if(type.find("String") != std::string::npos){
                        attr_pb->set_type(NodeManager::Attribute::STRING);
                        set_attr_string(attr_pb, value);
                    } else {
                        attr_pb->set_type(NodeManager::Attribute::INTEGER);
                        attr_pb->set_i(std::stoi(value));
                    }
                }
            }

            //Get Component Instance EventPorts
            auto port_ids = graphml_parser_->find_nodes("OutEventPortInstance", c_id);
            {
                auto in_port_ids = graphml_parser_->find_nodes("InEventPortInstance", c_id);
                //Combine into one list to reduce code duplication            
                port_ids.insert(port_ids.end(), in_port_ids.begin(), in_port_ids.end());
            }
            
            
            {
                //Add Periodic event port
                //TODO: Scrape GraphmL to determine values and location
                auto port_pb = component_pb->add_ports();
                port_pb->set_name("periodic_event");
                port_pb->set_type(NodeManager::EventPort::PERIODIC);

                auto duration = port_pb->add_attributes();
                duration->set_name("duration");
                duration->set_type(NodeManager::Attribute::INTEGER);
                duration->set_i(500);
            }

            for(auto p_id: port_ids){
                auto event_port = get_event_port(p_id);
                
                if(event_port){
                    //Add a Port the the Component
                    auto port_pb = component_pb->add_ports();

                    //Get the Port Name
                    port_pb->set_name(event_port->name);
                    //p->set_id(p_id)

                    std::string kind = get_data_value(p_id, "kind");

                    if(event_port->kind == "OutEventPortInstance"){
                        port_pb->set_type(NodeManager::EventPort::OUT);
                    } else if(event_port->kind == "InEventPortInstance"){
                        port_pb->set_type(NodeManager::EventPort::IN);                    
                    }

                    std::string port_middleware = event_port->middleware;
                    NodeManager::EventPort::Middleware mw;
                    
                    //Parse the middleware
                    if(!NodeManager::EventPort_Middleware_Parse(port_middleware, &mw)){
                        std::cout << "Cannot Parse Middleware: " << port_middleware << std::endl;
                        //mw = NodeManager::EventPort::UNKNOWN;
                        mw = NodeManager::EventPort::ZMQ;
                    }
                    port_pb->set_middleware(mw);


                    //Set port port number
                    auto topic_pb = port_pb->add_attributes();
                    topic_pb->set_name("topic_name");
                    topic_pb->set_type(NodeManager::Attribute::STRING);
                    //TODO: actually set Topic Name port number.
                    set_attr_string(topic_pb, event_port->topic_name); 
                    
                    
                    auto domain_id = port_pb->add_attributes();
                    domain_id->set_name("domain_id");
                    domain_id->set_type(NodeManager::Attribute::INTEGER);
                    domain_id->set_i(0);

                    auto broker = port_pb->add_attributes();
                    broker->set_name("broker");
                    broker->set_type(NodeManager::Attribute::STRING);
                    set_attr_string(broker, "localhost:5672"); 

                    if(port_pb->type() == NodeManager::EventPort::OUT){
                        HardwareNode* node = get_hardware_node(component->node_id);
                        if(node){
                            if(event_port->port_number > 0){
                                //Set Publisher TCP Address
                                auto publisher_addr_pb = port_pb->add_attributes();
                                publisher_addr_pb->set_name("publisher_address");
                                publisher_addr_pb->set_type(NodeManager::Attribute::STRINGLIST);
                                set_attr_string(publisher_addr_pb, event_port->port_address);
                            }

                           

                            //Set port port number
                            auto publisher_pb = port_pb->add_attributes();
                            publisher_pb->set_name("publisher_name");
                            publisher_pb->set_type(NodeManager::Attribute::STRING);
                            set_attr_string(publisher_pb, component->name + event_port->name);

                        }
                    }else if(port_pb->type() == NodeManager::EventPort::IN){
                        auto publisher_addr_pb = port_pb->add_attributes();
                        publisher_addr_pb->set_name("publisher_address");
                        publisher_addr_pb->set_type(NodeManager::Attribute::STRINGLIST);

                        //FIND END POINTS
                        for(auto e_id : assembly_edge_ids_){
                            auto s_id = graphml_parser_->get_attribute(e_id, "source");
                            auto t_id = graphml_parser_->get_attribute(e_id, "target");
                            EventPort* s = get_event_port(s_id);
                            EventPort* t = get_event_port(t_id);
                            if(t == event_port && s->port_number > 0){
                                set_attr_string(publisher_addr_pb, s->port_address);
                            }
                        }

                        //Set port port number
                        auto subscriber_pb = port_pb->add_attributes();
                        subscriber_pb->set_name("subscriber_name");
                        subscriber_pb->set_type(NodeManager::Attribute::STRING);
                        set_attr_string(subscriber_pb, component->name + event_port->name);
                    }
                }
            }
        }
    }

    for(auto a: deployment_map){
        std::string filter_name = a.second->mutable_node()->name() + "*";
        std::cout << "Master sending Action to: " << filter_name << std::endl;
        //std::cout << a.second->DebugString() << std::endl;
        zmq_master_->send_action(filter_name, a.second);
    }
}