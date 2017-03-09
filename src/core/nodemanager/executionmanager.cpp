#include "executionmanager.h"
#include <iostream>
#include <chrono>
#include <algorithm>
#include <map>
#include "../controlmessage/controlmessage.pb.h"

#include "../../re_common/zmq/protowriter/protowriter.h"

void set_attr_string(NodeManager::Attribute* attr, std::string val){
    attr->add_s(val);
}



ExecutionManager::ExecutionManager(std::string endpoint, std::string graphml_path, double execution_duration){
    //Setup writer
    proto_writer_ = new zmq::ProtoWriter();
    proto_writer_->BindPublisherSocket(endpoint);

    //Setup the parser
    graphml_parser_ = new GraphmlParser(graphml_path);

    auto start = std::chrono::steady_clock::now();
    bool success = ScrapeDocument();
    auto end = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "* Deployment Parsed In: " << ms.count() << " ms" << std::endl;
    std::cout << std::endl;

    //Start Execution thread for 60!
    execution_thread_ = new std::thread(&ExecutionManager::ExecutionLoop, this, execution_duration);
}

void ExecutionManager::PushMessage(std::string topic, google::protobuf::MessageLite* message){
    proto_writer_->PushMessage(topic, message);
}

std::vector<std::string> ExecutionManager::GetRequiredSlaveEndpoints(){
    return required_slaves_;
}

void ExecutionManager::SlaveOnline(std::string response, std::string endpoint, std::string slave_host_name){
    bool slave_online = response == "OKAY";

    if(slave_online){
        std::cout << "Slave: '" << slave_host_name << "' @ " << endpoint << " Online!" << std::endl;
        std::unique_lock<std::mutex>(mutex_);

        //Look through the deployment map for instructions to send the newly online slave
        for(auto a: deployment_map_){
            //Match the host_name 
            std::string host_name = a.second->mutable_node()->mutable_info()->name();

            if(slave_host_name == host_name){
                std::cout << "Sending Startup Instructions: " << host_name << std::endl;
                auto copy = new NodeManager::ControlMessage(*(a.second));
                proto_writer_->PushMessage(host_name + "*", copy);
            }   
        }
    }else{
        std::cerr << "Slave: '" << slave_host_name << "' @ " << endpoint << " Error!" << std::endl;
        std::cerr << response << std::endl;
    }

    if(slave_online){
        HandleSlaveOnline(endpoint);
    }
}

void ExecutionManager::HandleSlaveOnline(std::string endpoint){
    //Get the initial size
    int initial_size = inactive_slaves_.size();
    //Find the itterator position of the element
    inactive_slaves_.erase(std::remove(inactive_slaves_.begin(), inactive_slaves_.end(), endpoint), inactive_slaves_.end());
    
    if(initial_size > 0 && inactive_slaves_.empty()){
        ActivateExecution();
    }
}

std::string ExecutionManager::GetHostNameFromAddress(std::string address){
    for(auto hardware : hardware_nodes_){
        if(address.find(hardware.second->ip_address) != std::string::npos){
            return hardware.second->name;
        }
    }
    return "";
}

std::string ExecutionManager::GetLoggerAddressFromHostName(std::string host_name){
    std::string addr;
    for(auto h : hardware_nodes_){
        auto id = h.first;
        auto hardware = h.second;
        if(host_name == hardware->name){
            addr = GetTCPAddress(hardware->ip_address, hardware->logger_port_number);       
            break;
        }
    }
    return addr;
}

ExecutionManager::HardwareNode* ExecutionManager::GetHardwareNode(std::string id){
    ExecutionManager::HardwareNode* node = 0;

    if(hardware_nodes_.count(id)){
        node = hardware_nodes_[id];
    }
    return node;
}
ExecutionManager::ComponentInstance* ExecutionManager::GetComponent(std::string id){
    ExecutionManager::ComponentInstance* node = 0;

    if(component_instances_.count(id)){
        node = component_instances_[id];
    }
    return node;
}
ExecutionManager::EventPort* ExecutionManager::GetEventPort(std::string id){
    ExecutionManager::EventPort* node = 0;

    if(event_ports_.count(id)){
        node = event_ports_[id];
    }
    return node;
}
ExecutionManager::Attribute* ExecutionManager::GetAttribute(std::string id){
    ExecutionManager::Attribute* node = 0;

    if(attributes_.count(id)){
        node = attributes_[id];
    }
    return node;
}


std::string ExecutionManager::GetDefinitionId(std::string id){
    if(definition_ids_.count(id)){
        return definition_ids_[id];
    }
    std::string def_id;
    auto kind = GetDataValue(id, "kind");
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
        auto target = GetAttribute(e_id, "target");
        auto source = GetAttribute(e_id, "source");
        auto target_kind = GetDataValue(target, "kind");

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

std::string ExecutionManager::GetImplId(std::string id){
    std::string impl_id;
    //Check for a definition first.
    std::string def_id = GetDefinitionId(id);
    if(def_id == ""){
        def_id = id;
    }

    //Get the kind of Impl
    auto impl_kind = GetDataValue(def_id, "kind") + "Impl";

    //Get the Definition ID of the 
    for(auto e_id : definition_edge_ids_){
        auto source = GetAttribute(e_id, "source");
        auto target = GetAttribute(e_id, "target");
        auto source_kind = GetDataValue(source, "kind");

        if(target == def_id && source_kind == impl_kind){
            impl_id = source;
            break;
        }
    }
    return impl_id;

}

std::string ExecutionManager::GetTCPAddress(const std::string ip, const unsigned int port_number){
    std::string addr;
    if(!ip.empty()){
        addr += "tcp://" + ip + ":" + std::to_string(port_number);
    }
    return addr;
}
bool ExecutionManager::ScrapeDocument(){
    std::unique_lock<std::mutex>(mutex_);
    if(graphml_parser_){
        //Get the ID's of the edges
        deployment_edge_ids_ = graphml_parser_->FindEdges("Edge_Deployment");
        assembly_edge_ids_ = graphml_parser_->FindEdges("Edge_Assembly");
        definition_edge_ids_ = graphml_parser_->FindEdges("Edge_Definition");
        
        //Construct a Deployment Map which points ComponentInstance - > HardwareNodes
        for(auto e_id: deployment_edge_ids_){
            auto source_id = GetAttribute(e_id, "source");
            auto target_id = GetAttribute(e_id, "target");
            deployed_instance_map_[source_id] = target_id;
        }

        //Parse HardwareNodes
        for(auto n_id : graphml_parser_->FindNodes("HardwareNode")){
            //Set the Node information
            auto node = new HardwareNode();
            node->id = n_id;
            node->name = GetDataValue(n_id, "label");
            node->ip_address = GetDataValue(n_id, "ip_address"); 
            //Set Port Number for ZMQ Logger
            node->logger_port_number = ++node->port_count;
            

            //Get the ID's of the ComponentInstances deployed to this Node
            for(auto e_id : deployment_edge_ids_){
                auto target_id = GetAttribute(e_id, "target");

                if(n_id == target_id){
                    auto source_id = GetAttribute(e_id, "source");
                    auto source_kind = GetDataValue(source_id, "kind");
                    
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
        for(auto c_id : graphml_parser_->FindNodes("ComponentInstance")){
            auto component = new ComponentInstance();
            component->id = c_id;
            component->name = GetDataValue(c_id, "label");

            //std::cout << "Parsed: ComponentInstance[" << c_id << "]: " << component->name << std::endl;

            HardwareNode* node = 0;
            if(deployed_instance_map_.count(c_id)){
                //Get the Node ID
                component->node_id = deployed_instance_map_[c_id];
                node = GetHardwareNode(component->node_id);
            }
            
            //Parse Attributes
            for(auto a_id : graphml_parser_->FindNodes("AttributeInstance", c_id)){
                auto attribute = new Attribute();
                attribute->id = a_id;
                attribute->component_id = c_id;
                attribute->name = GetDataValue(a_id, "label");

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
            auto port_ids = graphml_parser_->FindNodes("OutEventPortInstance", c_id);
            {
                auto in_port_ids = graphml_parser_->FindNodes("InEventPortInstance", c_id);
                //Combine into one list to reduce code duplication            
                port_ids.insert(port_ids.end(), in_port_ids.begin(), in_port_ids.end());
            }

            //Parse In/OutEventPortInstance
            for(auto p_id: port_ids){
                auto port = new EventPort();
                port->id = p_id;
                port->component_id = c_id;
                port->name = GetDataValue(p_id, "label");
                port->topic_name = GetDataValue(p_id, "topicName");
                port->kind = GetDataValue(p_id, "kind");
                port->middleware = GetDataValue(p_id, "middleware");
                port->message_type = GetDataValue(p_id, "type");

                //Register Only OutEventPortInstances
                if(port->kind == "OutEventPortInstance" && node && port->middleware == "ZMQ"){
                    //Set Port Number only for ZMQ
                    port->port_number = ++node->port_count;
                    
                    //Construct a TCP Address
                    port->port_address = GetTCPAddress(node->ip_address, port->port_number);
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
        for(auto c_id : graphml_parser_->FindNodes("ComponentImpl")){
            //Set the ComponentInstance information
            auto component = new ComponentImpl();
            component->id = c_id;
            component->name = GetDataValue(c_id, "label");
            component->definition_id = GetDefinitionId(c_id);

            //Parse Periodic_Events
            for(auto p_id : graphml_parser_->FindNodes("PeriodicEvent", c_id)){
                auto port = new EventPort();

                //setup the port
                port->id = p_id;
                port->component_id = c_id;
                port->name = GetDataValue(p_id, "label");
                port->kind = GetDataValue(p_id, "kind");
                port->frequency = GetDataValue(p_id, "frequency");
                
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
                auto source = GetAttribute(e_id, "source");
                auto target = GetAttribute(e_id, "target");
                auto source_kind = GetDataValue(source, "kind");

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

    //Construct the Protobuf messages

    //Get the Deployed Hardware nodes. Construct STARTUP messages
    for(auto n : hardware_nodes_){
        auto node = n.second;

        if(node && !node->component_ids.empty()){
            auto cm = new NodeManager::ControlMessage();

            // Set the node name of the protobuf message
            cm->mutable_node()->mutable_info()->set_name(node->name);
            // Set the configure messages
            cm->set_type(NodeManager::ControlMessage::STARTUP);
            deployment_map_[node->id] = cm;
        }
    }

    //get component instances
    for(auto d : deployed_instance_map_){
        std::string c_id = d.first;
        std::string h_id = d.second;

        NodeManager::Node* node_pb = 0;
        if(deployment_map_.count(h_id)){
            node_pb = deployment_map_[h_id]->mutable_node();
        }

        auto component = GetComponent(c_id);
        auto hardware_node = GetHardwareNode(h_id);

        if(hardware_node && component && node_pb){
            auto component_pb = node_pb->add_components();
            auto component_info_pb = component_pb->mutable_info();
            
            component_info_pb->set_id(component->id);
            component_info_pb->set_name(component->name);
            component_info_pb->set_type(component->type_name);

            //Get the Component Attributes
            for(auto a_id : component->attribute_ids){
                auto attribute = GetAttribute(a_id);
                if(attribute){
                    auto attr_pb = component_pb->add_attributes();
                    auto attr_info_pb = attr_pb->mutable_info();

                    attr_info_pb->set_id(a_id);
                    attr_info_pb->set_name(GetDataValue(a_id, "label"));

                    auto type = GetDataValue(a_id, "type");
                    auto value = GetDataValue(a_id, "value");

                    if(type == "DoubleNumber" || type == "Float"){
                        attr_pb->set_kind(NodeManager::Attribute::DOUBLE);
                        attr_pb->set_d(std::stod(value));
                    } else if(type.find("String") != std::string::npos){
                        attr_pb->set_kind(NodeManager::Attribute::STRING);
                        set_attr_string(attr_pb, value);
                    } else {
                        attr_pb->set_kind(NodeManager::Attribute::INTEGER);
                        attr_pb->set_i(std::stoi(value));
                    }
                }
            }
            //Get the ComponentInstance Ports
            for(auto p_id: component->event_port_ids){
                auto event_port = GetEventPort(p_id);
                
                if(event_port){
                    //Add a Port the the Component
                    auto port_pb = component_pb->add_ports();
                    auto port_info_pb = port_pb->mutable_info();

                    //Get the Port Name
                    port_info_pb->set_id(p_id);
                    port_info_pb->set_name(event_port->name);
                    port_info_pb->set_type(event_port->message_type);


                    std::string kind = GetDataValue(p_id, "kind");

                    NodeManager::EventPort::Kind k;
                    if(event_port->kind == "OutEventPortInstance"){
                        k = NodeManager::EventPort::OUT_PORT;
                    } else if(event_port->kind == "InEventPortInstance"){
                        k = NodeManager::EventPort::IN_PORT;
                    } else if(event_port->kind == "PeriodicEvent"){
                        k = NodeManager::EventPort::PERIODIC_PORT;
                    }
                    port_pb->set_kind(k);

                    //Get the Middleware for the ports
                    if(k != NodeManager::EventPort::PERIODIC_PORT){
                        std::string port_middleware = event_port->middleware;
                        NodeManager::EventPort::Middleware mw;
                        
                        //Parse the middleware
                        if(!NodeManager::EventPort_Middleware_Parse(port_middleware, &mw)){
                            std::cout << "Cannot Parse Middleware: " << port_middleware << std::endl;
                        }

                        port_pb->set_middleware(mw);
                        
                        //Set the topic_name
                        auto topic_pb = port_pb->add_attributes();
                        auto topic_info_pb = topic_pb->mutable_info();
                        topic_info_pb->set_name("topic_name");
                        topic_pb->set_kind(NodeManager::Attribute::STRING);
                        set_attr_string(topic_pb, event_port->topic_name); 
                        
                        //Set the domain_id
                        //TODO: Need this in graphml
                        auto domain_pb = port_pb->add_attributes();
                        auto domain_info_pb = domain_pb->mutable_info();
                        domain_info_pb->set_name("domain_id");
                        domain_pb->set_kind(NodeManager::Attribute::INTEGER);
                        domain_pb->set_i(0);

                        //Set the broker address
                        //TODO: Need this in graphml
                        auto broker_pb = port_pb->add_attributes();
                        auto broker_info_pb = broker_pb->mutable_info();
                        broker_info_pb->set_name("broker");
                        broker_pb->set_kind(NodeManager::Attribute::STRING);
                        set_attr_string(broker_pb, "localhost:5672"); 


                        if(k == NodeManager::EventPort::OUT_PORT){
                            if(event_port->port_number > 0){
                                //Set Publisher TCP Address (ZMQ Only)
                                auto pub_addr_pb = port_pb->add_attributes();
                                auto pub_addr_info_pb = pub_addr_pb->mutable_info();
                                pub_addr_info_pb->set_name("publisher_address");
                                pub_addr_pb->set_kind(NodeManager::Attribute::STRINGLIST);
                                set_attr_string(pub_addr_pb, event_port->port_address);
                            }

                            //Set publisher name
                            //TODO: Allow modelling of this
                            auto pub_name_pb = port_pb->add_attributes();
                            auto pub_name_info_pb = pub_name_pb->mutable_info();
                            pub_name_info_pb->set_name("publisher_name");
                            pub_name_pb->set_kind(NodeManager::Attribute::STRING);
                            set_attr_string(pub_name_pb, component->name + event_port->name);

                        }else if(k == NodeManager::EventPort::IN_PORT){
                            //Construct a publisher_address list
                            auto pub_addr_pb = port_pb->add_attributes();
                            auto pub_addr_info_pb = pub_addr_pb->mutable_info();
                            pub_addr_info_pb->set_name("publisher_address");
                            pub_addr_pb->set_kind(NodeManager::Attribute::STRINGLIST);

                            //Find the end points and push them back onto the publisher_address list
                            for(auto e_id : assembly_edge_ids_){
                                auto s_id = graphml_parser_->GetAttribute(e_id, "source");
                                auto t_id = graphml_parser_->GetAttribute(e_id, "target");
                                EventPort* s = GetEventPort(s_id);
                                EventPort* t = GetEventPort(t_id);
                                if(t == event_port && s->port_number > 0){
                                    //Append the publisher TCP Address (ZMQ Only)
                                    set_attr_string(pub_addr_pb, s->port_address);
                                }
                            }

                            //Set subscriber name
                            //TODO: Allow modelling of this
                            auto sub_name_pb = port_pb->add_attributes();
                            auto sub_name_info_pb = sub_name_pb->mutable_info();
                            sub_name_info_pb->set_name("subscriber_name");
                            sub_name_pb->set_kind(NodeManager::Attribute::STRING);
                            set_attr_string(sub_name_pb, component->name + event_port->name);
                        }
                    }else{
                        //Periodic Events
                        try{
                            //Set the frequency of the periodic event
                            double temp = std::stod(event_port->frequency);
                            
                            auto freq_pb = port_pb->add_attributes();
                            auto freq_info_pb = freq_pb->mutable_info();

                            freq_info_pb->set_name("frequency");
                            freq_pb->set_kind(NodeManager::Attribute::DOUBLE);
                            freq_pb->set_d(temp);
                        }catch (...){}
                    }
                }
            }
        }
    }

    std::cout << "------[ Deployment Info ]------";

    for(auto n : hardware_nodes_){
        auto node = n.second;

        if(node && !node->component_ids.empty()){
            std::cout << std::endl << "* Node: '" << node->name << "' Deploys:" << std::endl;
            //Push this node onto the required slaves list
            required_slaves_.push_back(GetTCPAddress(node->ip_address, node->node_manager_port));

            for(auto c_id: node->component_ids){
                auto component = GetComponent(c_id);
                if(component){
                    std::cout << "** " << component->name << " [" << component->type_name << "]" << std::endl;
                }
            }
        }
    }

    inactive_slaves_ = required_slaves_;
    std::cout << "------------------------------" << std::endl;

    
            

    if(deployment_edge_ids_.size() > 0){
        //TODO: Add fail cases
        return true;
    } 
    return false;
}

std::string ExecutionManager::GetAttribute(std::string id, std::string attr_name){
    if(graphml_parser_){
        return graphml_parser_->GetAttribute(id, attr_name);
    }
    return "";
}
std::string ExecutionManager::GetDataValue(std::string id, std::string key_name){
    if(graphml_parser_){
        return graphml_parser_->GetDataValue(id, key_name);
    }
    return "";
}

void ExecutionManager::ActivateExecution(){
    //Obtain lock
    std::unique_lock<std::mutex> lock(activate_mutex_);
    //Notify
    activate_lock_condition_.notify_all();
}
void ExecutionManager::TerminateExecution(){
    //Obtain lock
    std::unique_lock<std::mutex> lock(terminate_mutex_);
    //Notify
    terminate_lock_condition_.notify_all();
}

void ExecutionManager::ExecutionLoop(double duration_sec){

    auto execution_duration = std::chrono::duration<double>(duration_sec);
    {
        //Obtain lock
        std::unique_lock<std::mutex> lock(activate_mutex_);
        //Wait for notify
        activate_lock_condition_.wait(lock);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    std::cout << "* Sending ACTIVATE" << std::endl;
    //Send Activate function
    auto activate = new NodeManager::ControlMessage();
    activate->set_type(NodeManager::ControlMessage::ACTIVATE);
    PushMessage("*", activate);

    {
        //Obtain lock
        std::unique_lock<std::mutex> lock(terminate_mutex_);
        //Wait for notify
        terminate_lock_condition_.wait_for(lock, execution_duration);
    }

    std::cout << "* Sending PASSIVATE" << std::endl;
    //Send Terminate Function
    auto passivate = new NodeManager::ControlMessage();
    passivate->set_type(NodeManager::ControlMessage::PASSIVATE);
    PushMessage("*", passivate);

    std::cout << "* Sending TERMINATE" << std::endl;
    //Send Terminate Function
    auto terminate = new NodeManager::ControlMessage();
    terminate->set_type(NodeManager::ControlMessage::TERMINATE);
    PushMessage("*", terminate);
}