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
    auto start = std::chrono::system_clock::now();
    bool success = scrape_document();
    auto end = std::chrono::system_clock::now();
    std::cout << "Parsing Document Took: " << (end - start).count() << " μs" << std::endl;
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


std::string ExecutionManager::get_definition_id(std::string id){
    if(definition_ids_.count(id)){
        return definition_ids_[id];
    }
    std::string def_id;
    auto kind = get_data_value(id, "kind");
    auto definition_kind = kind;

    std::vector<std::string> strings = {"Instance", "Impl"};
    
    //Find and remove instance
    for(auto str: strings){
        auto pos = definition_kind.find(str);
        if(pos != std::string::npos){
            definition_kind.erase(pos, str.length());
        }
    }

    //Get the Definition ID of the 
    for(auto e_id : definition_edge_ids_){
        auto target = get_attribute(e_id, "target");
        auto source = get_attribute(e_id, "source");
        auto target_kind = get_data_value(target, "kind");

        if(source == id && target_kind == definition_kind){
            def_id = target;
            break;
        }
    }
    if(!def_id.empty()){
        definition_ids_[id] = def_id;
    }
    return def_id;
}

std::string ExecutionManager::get_impl_id(std::string id){
    std::string impl_id;
    //Check for a definition first.
    std::string def_id = get_definition_id(id);
    if(def_id == ""){
        def_id = id;
    }

    //Get the kind of Impl
    auto impl_kind = get_data_value(def_id, "kind") + "Impl";

    //Get the Definition ID of the 
    for(auto e_id : definition_edge_ids_){
        auto source = get_attribute(e_id, "source");
        auto target = get_attribute(e_id, "target");
        auto source_kind = get_data_value(source, "kind");

        if(target == def_id && source_kind == impl_kind){
            impl_id = source;
            break;
        }
    }
    return impl_id;

}

bool ExecutionManager::scrape_document(){
    if(graphml_parser_){
        
        deployment_edge_ids_ = graphml_parser_->find_edges("Edge_Deployment");
        assembly_edge_ids_ = graphml_parser_->find_edges("Edge_Assembly");
        definition_edge_ids_ = graphml_parser_->find_edges("Edge_Definition");
        
        //Construct a Deployment Map which points ComponentInstance - > HardwareNodes
        for(auto e_id: deployment_edge_ids_){
            auto source_id = get_attribute(e_id, "source");
            auto target_id = get_attribute(e_id, "target");
            deployed_instance_map_[source_id] = target_id;
        }

        //Parse HardwareNodes
        for(auto n_id : graphml_parser_->find_nodes("HardwareNode")){
            //Set the Node information
            auto node = new HardwareNode();
            node->id = n_id;
            node->name = get_data_value(n_id, "label");
            node->ip_address = get_data_value(n_id, "ip_address");        

            //std::cout << "Parsed: HardwareNode[" << n_id << "]: " << node->name << std::endl;

            //Get the ID's of the ComponentInstances deployed to this Node
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

            if(!hardware_nodes_.count(n_id)){
                //Add the HardwareNode to the map if we haven't seen it
                hardware_nodes_[n_id] = node;
            }else{
                //We found a duplicate node, so destroy
                std::cout << "Got Duplicate Nodes: " << n_id << std::endl;
                delete node;
            }
        }

        //Parse ComponentInstances
        for(auto c_id : graphml_parser_->find_nodes("ComponentInstance")){
            auto component = new ComponentInstance();
            component->id = c_id;
            component->name = get_data_value(c_id, "label");

            std::cout << "Parsed: ComponentInstance[" << c_id << "]: " << component->name << std::endl;

            HardwareNode* node = 0;
            if(deployed_instance_map_.count(c_id)){
                //Get the Node ID
                component->node_id = deployed_instance_map_[c_id];
                node = get_hardware_node(component->node_id);
            }
            
            //Parse Attributes
            for(auto a_id : graphml_parser_->find_nodes("AttributeInstance", c_id)){
                auto attribute = new Attribute();
                attribute->id = a_id;
                attribute->component_id = c_id;
                attribute->name = get_data_value(a_id, "label");

                if(!attributes_.count(a_id)){
                    attributes_[a_id] = attribute;
                }else{
                    std::cout << "Got Duplicate Attribute: " << a_id << std::endl;
                    delete attribute;
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

            //Parse In/OutEventPortInstance
            for(auto p_id: port_ids){
                auto port = new EventPort();
                port->id = p_id;
                port->component_id = c_id;
                port->name = get_data_value(p_id, "label");
                port->topic_name = get_data_value(p_id, "topicName");
                port->kind = get_data_value(p_id, "kind");
                port->middleware = get_data_value(p_id, "middleware");
                port->message_type = get_data_value(p_id, "type");

                //Register Only OutEventPortInstances
                if(port->kind == "OutEventPortInstance" && node && port->middleware == "ZMQ"){
                    //Set Port Number only for ZMQ
                    port->port_number = ++node->port_count;
                    
                    //Construct a TCP Address
                    port->port_address = "tcp://" + node->ip_address + ":" + std::to_string(port->port_number);
                }

                if(!event_ports_.count(p_id)){
                    event_ports_[p_id] = port;
                }else{
                    std::cout << "Got Duplicate EventPort: " << p_id << std::endl;
                    delete port;
                }

                //Add the Attribute to the Component
                component->event_port_ids.push_back(p_id);
            }

            if(!component_instances_.count(c_id)){
                component_instances_[c_id] = component;
            }else{
                std::cout << "Got Duplicate ComponentInstance: " << c_id << std::endl;
                delete component;
            }
        }

        //Parse ComponentImpl
        for(auto c_id : graphml_parser_->find_nodes("ComponentImpl")){
            //Set the ComponentInstance information
            auto component = new ComponentImpl();
            component->id = c_id;
            component->name = get_data_value(c_id, "label");
            component->definition_id = get_definition_id(c_id);


            //std::cout << "Parsed: ComponentImpl[" << c_id << "]: " << component->name << std::endl;
            
            //Parse Periodic_Events
            for(auto p_id : graphml_parser_->find_nodes("PeriodicEvent", c_id)){
                auto port = new EventPort();

                //setup the port
                port->id = p_id;
                port->component_id = c_id;
                port->name = get_data_value(p_id, "label");
                port->kind = get_data_value(p_id, "kind");
                port->frequency = get_data_value(p_id, "frequency");
                
                //std::cout << "Parsed: PeriodicEvent[" << p_id << "]: " << port->name << std::endl;

                //Add it to the ComponentImpl
                component->periodic_eventports_ids.push_back(p_id);
                
                if(!event_ports_.count(p_id)){
                    event_ports_[p_id] = port;
                }else{
                    std::cout << "Got Duplicate PeriodicEvent: " << p_id << std::endl;
                    delete port;
                }
            }

            //Look through the Definition edges to find all instances of the Component Definition
            for(auto e_id : definition_edge_ids_){
                auto source = get_attribute(e_id, "source");
                auto target = get_attribute(e_id, "target");
                auto source_kind = get_data_value(source, "kind");

                if(target == component->definition_id && source_kind == "ComponentInstance"){
                    component->instance_ids.push_back(source);
                    //Add a back link
                    if(component_instances_.count(source)){
                        auto c = component_instances_[source];
                        c->implementation_id = c_id;
                        c->definition_id = component->definition_id;
                        c->type_name = component->name;
                        //Append the PeriodicEvents ID from the Impl to the Instances EventPorts
                        c->event_port_ids.insert(c->event_port_ids.end(), component->periodic_eventports_ids.begin(), component->periodic_eventports_ids.end());
                    }
                }
            }

            if(!component_impls_.count(c_id)){
                component_impls_[c_id] = component;
            }else{
                std::cout << "Got Duplicate PeriodicEvent: " << c_id << std::endl;
                delete component;
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
            component_pb->set_type(component->type_name);

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

            for(auto p_id: component->event_port_ids){
                auto event_port = get_event_port(p_id);
                
                if(event_port){
                    //Add a Port the the Component
                    auto port_pb = component_pb->add_ports();

                    //Get the Port Name
                    port_pb->set_name(event_port->name);
                    port_pb->set_message_type(event_port->message_type);
                    //p->set_id(p_id)

                    std::string kind = get_data_value(p_id, "kind");

                    if(event_port->kind == "OutEventPortInstance"){
                        port_pb->set_type(NodeManager::EventPort::OUT);
                    } else if(event_port->kind == "InEventPortInstance"){
                        port_pb->set_type(NodeManager::EventPort::IN);                    
                    } else if(event_port->kind == "PeriodicEvent"){
                        port_pb->set_type(NodeManager::EventPort::PERIODIC);
                    }

                    if(port_pb->type() != NodeManager::EventPort::PERIODIC){


                        std::string port_middleware = event_port->middleware;
                        NodeManager::EventPort::Middleware mw;
                        
                        //Parse the middleware
                        if(!NodeManager::EventPort_Middleware_Parse(port_middleware, &mw)){
                            std::cout << "Cannot Parse Middleware: " << port_middleware << std::endl;
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
                    } else {
                        try{
                            double temp = std::stod(event_port->frequency);
                            auto frequency_pb = port_pb->add_attributes();
                            frequency_pb->set_name("frequency");
                            frequency_pb->set_type(NodeManager::Attribute::DOUBLE);
                            frequency_pb->set_d(temp);
                        } catch (...){}
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