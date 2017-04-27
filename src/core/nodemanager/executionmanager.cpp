#include "executionmanager.h"
#include "executionparser/modelparser.h"
#include <iostream>
#include <chrono>
#include <algorithm>
#include <map>
#include "../controlmessage/controlmessage.pb.h"

#include "../../re_common/zmq/protowriter/protowriter.h"
#include "executionparser/modelparser.h"
#include "executionparser/datatypes.h"

#include "execution.hpp"

void set_attr_string(NodeManager::Attribute* attr, std::string val){
    attr->add_s(val);
}

ExecutionManager::ExecutionManager(std::string endpoint, std::string graphml_path, double execution_duration, Execution* execution){
    //Setup writer
    proto_writer_ = new zmq::ProtoWriter();
    proto_writer_->BindPublisherSocket(endpoint);

    execution_ = execution;
    execution_->AddTerminateCallback(std::bind(&ExecutionManager::TerminateExecution, this));

    //Setup the parser
    auto start = std::chrono::steady_clock::now();
    model_parser_ = new Graphml::ModelParser(graphml_path);
    bool success = ConstructControlMessages();
    auto end = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "* Deployment Parsed In: " << ms.count() << " us" << std::endl;
    std::cout << "--------[Slave Registration]--------" << std::endl;
    //Start Execution thread for 60!
    execution_thread_ = new std::thread(&ExecutionManager::ExecutionLoop, this, execution_duration);
}

void ExecutionManager::PushMessage(std::string topic, google::protobuf::MessageLite* message){
    proto_writer_->PushMessage(topic, message);
}

std::vector<std::string> ExecutionManager::GetNodeManagerSlaveAddresses(){
    return required_slave_addresses_;
}

void ExecutionManager::SlaveOnline(std::string response, std::string endpoint, std::string slave_host_name){
    bool slave_online = response == "OKAY";

    if(slave_online){
        std::cout << "* Slave: '" << slave_host_name << "' @ " << endpoint << " Online!" << std::endl;
        std::unique_lock<std::mutex>(mutex_);

        //Look through the deployment map for instructions to send the newly online slave
        for(auto a: deployment_map_){
            //Match the host_name 
            std::string host_name = a.second->mutable_node()->mutable_info()->name();

            if(slave_host_name == host_name){
                //std::cout << "Sending Startup Instructions: " << host_name << std::endl;
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

void ExecutionManager::HandleSlaveOnline(std:: string endpoint){
    //Get the initial size
    int initial_size = inactive_slave_addresses_.size();
    //Find the itterator position of the element
    inactive_slave_addresses_.erase(std::remove(inactive_slave_addresses_.begin(), inactive_slave_addresses_.end(), endpoint), inactive_slave_addresses_.end());
    
    if(initial_size > 0 && inactive_slave_addresses_.empty()){
        ActivateExecution();
    }
}

std::string ExecutionManager::GetNodeNameFromNodeManagerAddress(std::string tcp_address){
    //Trim off the tcp:// and port
    auto first = tcp_address.find_last_of("/");
    auto last = tcp_address.find_last_of(":");
    if(first == std::string::npos || last == std::string::npos){
        std::cerr << "* Cannot Parse TCP: " << tcp_address << std::endl;
    }
    first += 1;

    auto address = tcp_address.substr(first, last - first);
    auto node = model_parser_->GetHardwareNodeByIPAddress(address);
    
    std::string str;
    if(node){
        str = node->name;
    }
    return str;
}

std::string ExecutionManager::GetModelLoggerAddressFromNodeName(std::string host_name){
    auto node = model_parser_->GetHardwareNodeByName(host_name);
    std::string str;
    if(node){
        str = node->GetModelLoggerAddress();
    }
    return str;
}

std::string ExecutionManager::GetModelLoggerModeFromNodeName(std::string host_name){
    auto node = model_parser_->GetHardwareNodeByName(host_name);
    std::string str = "OFF";
    if(node && node->logging_profile_id != ""){
        auto profile = model_parser_->GetLoggingProfile(node->logging_profile_id);
        str = profile->mode;
    }
    return str;
}

bool ExecutionManager::ConstructControlMessages(){
    std::unique_lock<std::mutex>(mutex_);

    //Construct the Protobuf messages
    auto nodes = model_parser_->GetHardwareNodes();
    //Get the Deployed Hardware nodes. Construct STARTUP messages
    for(auto node : nodes){
        if(node->is_deployed()){
            auto cm = new NodeManager::ControlMessage();

            // Set the node name of the protobuf message
            cm->mutable_node()->mutable_info()->set_name(node->name);
            // Set the configure messages
            cm->set_type(NodeManager::ControlMessage::STARTUP);
            deployment_map_[node->id] = cm;
        }
    }

    //get component instances
    for(auto component : model_parser_->GetComponentInstances()){
        auto c_id = component->id;
        auto h_id = component->deployed_node_id;
        auto hardware_node = model_parser_->GetHardwareNode(h_id);

        NodeManager::Node* node_pb = 0;
        if(deployment_map_.count(h_id)){
            node_pb = deployment_map_[h_id]->mutable_node();
        }

        if(hardware_node && component && node_pb){
            auto component_pb = node_pb->add_components();
            auto component_info_pb = component_pb->mutable_info();
            
            component_info_pb->set_id(component->id);
            component_info_pb->set_name(component->name);
            component_info_pb->set_type(component->type_name);

            //Get the Component Attributes
            for(auto a_id : component->attribute_ids){
                auto attribute = model_parser_->GetAttribute(a_id);
                if(attribute){
                    auto attr_pb = component_pb->add_attributes();
                    auto attr_info_pb = attr_pb->mutable_info();

                    attr_info_pb->set_id(a_id);
                    attr_info_pb->set_name(attribute->name);
                    auto type = attribute->type;
                    auto value = attribute->value;

                    if(type == "DoubleNumber" || type == "Float"){
                        attr_pb->set_kind(NodeManager::Attribute::DOUBLE);

                        double d_value;
                        try{
                            d_value = std::stod(value);
                        }catch(std::invalid_argument){
                            d_value = 0;
                        }
                        attr_pb->set_d(d_value);

                    } else if(type.find("String") != std::string::npos){
                        attr_pb->set_kind(NodeManager::Attribute::STRING);
                        set_attr_string(attr_pb, value);
                    } else {
                        
                        attr_pb->set_kind(NodeManager::Attribute::INTEGER);

                        int i_value;
                        try{
                            i_value = std::stoi(value);
                        }catch(std::invalid_argument){
                            i_value = 0;
                        }
                        attr_pb->set_i(i_value);
                    }
                }
            }
            //Get the ComponentInstance Ports
            for(auto p_id: component->event_port_ids){
                auto event_port = model_parser_->GetEventPort(p_id);
                
                if(event_port){
                    //Add a Port the the Component
                    auto port_pb = component_pb->add_ports();
                    auto port_info_pb = port_pb->mutable_info();

                    port_pb->set_namespace_name(event_port->namespace_name);

                    //Get the Port Name
                    port_info_pb->set_id(p_id);
                    port_info_pb->set_name(event_port->name);
                    port_info_pb->set_type(event_port->message_type);

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

                        //Set the middleware
                        port_pb->set_middleware(mw);

                        bool is_rti = mw == NodeManager::EventPort::RTI;
                        bool is_ospl = mw == NodeManager::EventPort::OSPL;
                        bool is_qpid = mw == NodeManager::EventPort::QPID;
                        bool is_zmq = mw == NodeManager::EventPort::ZMQ;

                        bool is_outport = k == NodeManager::EventPort::OUT_PORT;
                        bool is_inport = k == NodeManager::EventPort::IN_PORT;
                        
                        if(is_rti || is_ospl || is_qpid){
                             //Set the topic_name
                            auto topic_pb = port_pb->add_attributes();
                            auto topic_info_pb = topic_pb->mutable_info();
                            topic_info_pb->set_name("topic_name");
                            topic_pb->set_kind(NodeManager::Attribute::STRING);
                            set_attr_string(topic_pb, event_port->topic_name); 
                        }

                        if(is_rti || is_ospl){
                            //Set the domain_id
                            auto domain_pb = port_pb->add_attributes();
                            auto domain_info_pb = domain_pb->mutable_info();
                            domain_info_pb->set_name("domain_id");
                            domain_pb->set_kind(NodeManager::Attribute::INTEGER);
                            domain_pb->set_i(0);
                            
                            //QOS Profile Path
                            auto qos_path_pb = port_pb->add_attributes();
                            auto qos_path_info_pb = qos_path_pb->mutable_info();
                            qos_path_info_pb->set_name("qos_profile_path");
                            qos_path_pb->set_kind(NodeManager::Attribute::STRING);
                            set_attr_string(qos_path_pb, event_port->qos_profile_path);

                            //QOS Profile Name
                            auto qos_name_pb = port_pb->add_attributes();
                            auto qos_name_info_pb = qos_name_pb->mutable_info();
                            qos_name_info_pb->set_name("qos_profile_name");
                            qos_name_pb->set_kind(NodeManager::Attribute::STRING);
                            set_attr_string(qos_name_pb, event_port->qos_profile_name);

                            if(is_outport){
                                //Set publisher name
                                auto pub_name_pb = port_pb->add_attributes();
                                auto pub_name_info_pb = pub_name_pb->mutable_info();
                                pub_name_info_pb->set_name("publisher_name");
                                pub_name_pb->set_kind(NodeManager::Attribute::STRING);
                                set_attr_string(pub_name_pb, component->name + event_port->name);
                            }
                            if(is_inport){
                                //Set subscriber name
                                //TODO: Allow modelling of this
                                auto sub_name_pb = port_pb->add_attributes();
                                auto sub_name_info_pb = sub_name_pb->mutable_info();
                                sub_name_info_pb->set_name("subscriber_name");
                                sub_name_pb->set_kind(NodeManager::Attribute::STRING);
                                set_attr_string(sub_name_pb, component->name + event_port->name);
                            }    
                        }

                        if(is_qpid){
                            //Set the broker address
                            //TODO: Need this in graphml
                            auto broker_pb = port_pb->add_attributes();
                            auto broker_info_pb = broker_pb->mutable_info();
                            broker_info_pb->set_name("broker");
                            broker_pb->set_kind(NodeManager::Attribute::STRING);
                            set_attr_string(broker_pb, "localhost:5672"); 
                        }

                        if(is_zmq){
                            if(is_outport && event_port->port_number > 0){
                                auto pub_addr_pb = port_pb->add_attributes();
                                auto pub_addr_info_pb = pub_addr_pb->mutable_info();
                                pub_addr_info_pb->set_name("publisher_address");
                                pub_addr_pb->set_kind(NodeManager::Attribute::STRINGLIST);
                                set_attr_string(pub_addr_pb, event_port->port_address);
                            }

                            if(is_inport){
                                //Construct a publisher_address list
                                auto pub_addr_pb = port_pb->add_attributes();
                                auto pub_addr_info_pb = pub_addr_pb->mutable_info();
                                pub_addr_info_pb->set_name("publisher_address");
                                pub_addr_pb->set_kind(NodeManager::Attribute::STRINGLIST);

                                //Find the end points and push them back onto the publisher_address list
                                for(auto port_id : event_port->connected_port_ids){
                                    auto outeventport = model_parser_->GetEventPort(port_id);
                                    if(outeventport && outeventport->port_number > 0){
                                        set_attr_string(pub_addr_pb, outeventport->port_address);
                                    }
                                }
                            }
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

    std::cout << "------------[Deployment]------------";
    bool okay = false;
    for(auto node : nodes){
        if(node->is_deployed()){
            std::cout << std::endl << "* Node: '" << node->name << "' Deploys:" << std::endl;
            //Push this node onto the required slaves list
            required_slave_addresses_.push_back(node->GetNodeManagerSlaveAddress());

            for(auto c_id: node->component_ids){
                auto component = model_parser_->GetComponentInstance(c_id);
                if(component){
                    std::cout << "** " << component->name << " [" << component->type_name << "]" << std::endl;
                }
            }
            okay = true;
        }
    }

    inactive_slave_addresses_ = required_slave_addresses_;
    
    //TODO: Add fail cases
    return okay;
}

void ExecutionManager::ActivateExecution(){
    //Obtain lock
    std::unique_lock<std::mutex> lock(activate_mutex_);
    //Notify
    activate_lock_condition_.notify_all();
}
void ExecutionManager::TerminateExecution(){
    //Set termination flag
    terminate_flag_ = true;
    //Obtain lock
    std::unique_lock<std::mutex> lock(terminate_mutex_);
    //Notify
    terminate_lock_condition_.notify_all();
}

bool ExecutionManager::Finished(){
    return finished_;
}

void ExecutionManager::ExecutionLoop(double duration_sec){

    auto execution_duration = std::chrono::duration<double>(duration_sec);
    {
        //Obtain lock
        std::unique_lock<std::mutex> lock(activate_mutex_);
        //Wait for notify
        activate_lock_condition_.wait(lock);
    }
    std::cout << "-------------[Execution]------------" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    std::cout << "* Sending ACTIVATE" << std::endl;
    //Send Activate function
    auto activate = new NodeManager::ControlMessage();
    activate->set_type(NodeManager::ControlMessage::ACTIVATE);
    PushMessage("*", activate);

    if(!terminate_flag_){
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

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    std::cout << "* Sending TERMINATE" << std::endl;
    //Send Terminate Function
    auto terminate = new NodeManager::ControlMessage();
    terminate->set_type(NodeManager::ControlMessage::TERMINATE);
    PushMessage("*", terminate);
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    finished_ = true;
    execution_->Interrupt();
}