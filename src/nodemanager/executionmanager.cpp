#include "executionmanager.h"
#include "executionparser/modelparser.h"
#include <iostream>
#include <chrono>
#include <algorithm>
#include <unordered_map>
#include "controlmessage/controlmessage.pb.h"

#include <re_common/zmq/protowriter/protowriter.h>
#include "executionparser/modelparser.h"
#include "executionparser/datatypes.h"

#include "execution.hpp"

#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cctype>

bool str2bool(std::string str) {
    try{
        return std::stoi(str);
    }catch(std::invalid_argument){

    }

    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    std::istringstream is(str);
    bool b;
    is >> std::boolalpha >> b;
    int int_val = 0;
    return b;
}


void set_attr_string(NodeManager::Attribute* attr, const std::string& val){
    attr->add_s(val);
}

ExecutionManager::ExecutionManager(const std::string& endpoint, const std::string& graphml_path, double execution_duration, Execution* execution){
    if(execution){
        //Setup writer
        proto_writer_ = new zmq::ProtoWriter();
        proto_writer_->BindPublisherSocket(endpoint);

        execution_ = execution;
        execution_->AddTerminateCallback(std::bind(&ExecutionManager::TerminateExecution, this));
    }
    master_endpoint_ = endpoint;

    //Setup the parser
    auto start = std::chrono::steady_clock::now();
    model_parser_ = new Graphml::ModelParser(graphml_path);
    parse_succeed_ = ConstructControlMessages();
    auto end = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    if(parse_succeed_){
        std::cout << "* Deployment Parsed In: " << ms.count() << " us" << std::endl;
    }else{
        std::cout << "* Deployment Parsing Failed!" << std::endl;
    }

    if(parse_succeed_ && execution){
        std::cout << "--------[Slave Registration]--------" << std::endl;
        //Start Execution thread for 60!
        execution_thread_ = new std::thread(&ExecutionManager::ExecutionLoop, this, execution_duration);
    }
}

void ExecutionManager::PushMessage(const std::string& topic, google::protobuf::MessageLite* message){
    proto_writer_->PushMessage(topic, message);
}

std::vector<std::string> ExecutionManager::GetSlaveAddresses(){
    std::vector<std::string> slave_addresses;
    for(const auto& ss : slave_states_){
        slave_addresses.push_back(ss.first);
    }
    return slave_addresses;
}



bool ExecutionManager::IsValid(){
    return parse_succeed_;
}


std::vector<NodeManager::ControlMessage*> ExecutionManager::getNodeStartupMessage(){
    std::vector<NodeManager::ControlMessage*> messages;
    for(auto a : deployment_map_){
        messages.push_back(a.second);
    }
    return messages;
}

bool ExecutionManager::HandleSlaveResponseMessage(const std::string& slave_address, const NodeManager::StartupResponse& response){
    auto slave_state = SlaveState::ERROR_;
    auto slave_host_name = GetSlaveNameFromAddress(slave_address);
    if(response.IsInitialized()){
        slave_state = response.success() ? SlaveState::ONLINE : SlaveState::ERROR_;
    }

    if(slave_state == SlaveState::ERROR_){
        std::cerr << "* Slave: '" << slave_host_name << "' @ " << slave_address << " Error!" << std::endl;
        for(const auto& error_str : response.error_codes()){
            std::cerr << "* " << error_str << std::endl;
        }
    }else{
        std::cout << "* Slave: '" << slave_host_name << "' @ " << slave_address << " Online" << std::endl;
    }

    if(slave_states_.count(slave_address)){
        slave_states_[slave_address] = slave_state;
        
        if(GetSlaveStateCount(SlaveState::OFFLINE) == 0){
            bool should_execute = GetSlaveStateCount(SlaveState::ERROR_) == 0;
            TriggerExecution(should_execute);
        }
        return true;
    }
    return false;
}   

const NodeManager::Startup ExecutionManager::GetSlaveStartupMessage(const std::string& address){
    NodeManager::Startup startup;
    
    const auto& host_name = GetSlaveNameFromAddress(address);
    
    auto node = model_parser_->GetHardwareNodeByName(host_name);
    
    if(node){
        //Set the logging settings
        auto logging_profile = model_parser_->GetLoggingProfile(node->logging_profile_id);
        if(logging_profile){
            if(logging_profile->mode == "OFF"){
                startup.mutable_logger()->set_mode(NodeManager::Logger::OFF);
            }else if(logging_profile->mode == "LIVE"){
                startup.mutable_logger()->set_mode(NodeManager::Logger::LIVE);
            }else if(logging_profile->mode == "CACHED"){
                startup.mutable_logger()->set_mode(NodeManager::Logger::CACHED);
            }
        }
        startup.mutable_logger()->set_publisher_address(node->GetModelLoggerAddress());
        startup.set_host_name(host_name);
    }

     for(const auto& a: deployment_map_){
        if(host_name == a.second->host_name()){
            startup.set_allocated_configure(new NodeManager::ControlMessage(*(a.second)));
        }
    }
    startup.set_publisher_address(master_endpoint_);
    return startup;
}

int ExecutionManager::GetSlaveStateCount(const SlaveState& state){
    int count = 0;
    for(const auto& ss : slave_states_){
        if(ss.second == state){
            count ++;
        }
    }
    return count;
}


std::string ExecutionManager::GetSlaveNameFromAddress(const std::string& tcp_address){
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

NodeManager::Attribute_Kind GetAttributeType(const std::string& type){
    if(type == "Integer"){
        return NodeManager::Attribute::INTEGER;
    }else if(type == "Boolean"){
        return NodeManager::Attribute::BOOLEAN;
    }else if(type == "Character"){
        return NodeManager::Attribute::CHARACTER;
    }else if(type == "String"){
        return NodeManager::Attribute::STRING;
    }else if(type == "Double"){
        return NodeManager::Attribute::DOUBLE;
    }else if(type == "Float"){
        return NodeManager::Attribute::FLOAT;
    }
    std::cerr << "Unhandle Graphml Attribute Type: '" << type << "'" << std::endl;
    return NodeManager::Attribute::STRING;
}

bool ExecutionManager::ConstructControlMessages(){
    std::unique_lock<std::mutex>(mutex_);

    if(!model_parser_->IsValid()){
        return false;
    }
    //Construct the Protobuf messages
    auto nodes = model_parser_->GetHardwareNodes();
    //Get the Deployed Hardware nodes. Construct STARTUP messages
    for(auto node : nodes){
        if(node->is_deployed()){
            auto cm = new NodeManager::ControlMessage();

            auto node_pb = cm->add_nodes();
            cm->set_host_name(node->name);



            // Set the node name of the protobuf message
            node_pb->mutable_info()->set_name(node->name);
            node_pb->mutable_info()->set_id(node->id);

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

            node_pb = deployment_map_[h_id]->mutable_nodes(0);
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
                    attr_pb->set_kind(GetAttributeType(attribute->type));
                    
                    auto value = attribute->value;

                    switch(attr_pb->kind()){
                        case NodeManager::Attribute::FLOAT:
                        case NodeManager::Attribute::DOUBLE:{
                            double double_val = 0;
                            try{
                                double_val = std::stod(value);
                            }catch(std::invalid_argument){
                                double_val = 0;
                            }
                            attr_pb->set_d(double_val);
                            break;
                        }
                        case NodeManager::Attribute::CHARACTER:{
                            //need to trim out ' characters
                            auto char_str = value;
                            char_str.erase(std::remove(char_str.begin(), char_str.end(), '\''), char_str.end());
                            if(char_str.length() == 1){
                                attr_pb->set_i(char_str[0]);
                            }else{
                                std::cerr << "Character: '" << value << "' isn't length one!" << std::endl;
                            }
                            break;
                        }
                        case NodeManager::Attribute::BOOLEAN:{
                            bool val = str2bool(value);
                            attr_pb->set_i(val);
                            break;
                        }
                        case NodeManager::Attribute::INTEGER:{
                            int int_val = 0;
                            try{
                                int_val = std::stoi(value);
                            }catch(std::invalid_argument){
                                int_val = 0;
                            }
                            attr_pb->set_i(int_val);
                            break;
                        }
                        case NodeManager::Attribute::STRING:{
                            auto str = value;
                            str.erase(std::remove(str.begin(), str.end(), '"'), str.end());
                            set_attr_string(attr_pb, str);
                            break;
                        }
                        default:
                            std::cerr << "Got unhandled Attribute type: " << NodeManager::Attribute_Kind_Name(attr_pb->kind()) << std::endl;
                            break;
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
                    } else{
                        std::cerr << "INVALID PORT KIND: " << event_port->kind << std::endl;
                    }
                    
                    port_pb->set_kind(k);
                    //Get the Middleware for the ports
                    if(k != NodeManager::EventPort::PERIODIC_PORT){
                        const std::string& port_middleware = event_port->middleware;
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
                        bool is_tao = mw == NodeManager::EventPort::TAO;

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
                            
                            //QOS Profile Name
                            auto qos_name_pb = port_pb->add_attributes();
                            auto qos_name_info_pb = qos_name_pb->mutable_info();
                            qos_name_info_pb->set_name("qos_profile_name");
                            qos_name_pb->set_kind(NodeManager::Attribute::STRING);
                            set_attr_string(qos_name_pb, event_port->qos_profile_name);

                            //QOS Profile Path
                            auto qos_path_pb = port_pb->add_attributes();
                            auto qos_path_info_pb = qos_path_pb->mutable_info();
                            qos_path_info_pb->set_name("qos_profile_path");
                            qos_path_pb->set_kind(NodeManager::Attribute::STRING);
                            set_attr_string(qos_path_pb, event_port->qos_profile_path);

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

                        if(is_tao){
                            if(is_inport){
                                auto pub_name_pb = port_pb->add_attributes();
                                auto pub_name_info_pb = pub_name_pb->mutable_info();
                                pub_name_info_pb->set_name("publisher_name");
                                pub_name_pb->set_kind(NodeManager::Attribute::STRING);
                                set_attr_string(pub_name_pb, event_port->name);

                                auto pub_addr_pb = port_pb->add_attributes();
                                auto pub_addr_info_pb = pub_addr_pb->mutable_info();
                                pub_addr_info_pb->set_name("publisher_address");
                                pub_addr_pb->set_kind(NodeManager::Attribute::STRING);
                                set_attr_string(pub_addr_pb, event_port->port_address);
                            }
                            if(is_outport){
                                auto pub_name_pb = port_pb->add_attributes();
                                auto pub_name_info_pb = pub_name_pb->mutable_info();
                                pub_name_info_pb->set_name("publisher_names");
                                pub_name_pb->set_kind(NodeManager::Attribute::STRINGLIST);

                                auto pub_addr_pb = port_pb->add_attributes();
                                auto pub_addr_info_pb = pub_addr_pb->mutable_info();
                                pub_addr_info_pb->set_name("publisher_address");
                                pub_addr_pb->set_kind(NodeManager::Attribute::STRINGLIST);

                                //Find endpoints and push them onto address list
                                for(auto port_id : event_port->connected_port_ids){
                                    auto ineventport = model_parser_->GetEventPort(port_id);
                                    if(ineventport && ineventport->port_number > 0){
                                        set_attr_string(pub_name_pb, ineventport->name);
                                        set_attr_string(pub_addr_pb, ineventport->named_port_address);
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

    std::cout << "------------[Deployment]------------" << std::endl; 
    bool okay = false;
    for(auto node : nodes){
        if(node->is_deployed()){
            std::cout << "* Slave: '" << node->name << "' Deploys:" << std::endl;

            const auto& slave_address = node->GetNodeManagerSlaveAddress();
            slave_states_[slave_address] = SlaveState::OFFLINE;
            
            for(auto c_id: node->component_ids){
                auto component = model_parser_->GetComponentInstance(c_id);
                if(component){
                    std::cout << "** " << component->name << " [" << component->type_name << "]" << std::endl;
                }
            }
            okay = true;
        }
    }
    //TODO: Add fail cases
    return okay;
}

void ExecutionManager::TriggerExecution(bool execute){
    //Obtain lock
    std::unique_lock<std::mutex> lock(execution_mutex_);
    terminate_flag_ = !execute;
    //Notify
    execution_lock_condition_.notify_all();
}

void ExecutionManager::TerminateExecution(){
    //Interupt 
    TriggerExecution(false);

    if(execution_thread_){
        //Notify
        execution_thread_->join();
    }
}

void ExecutionManager::ExecutionLoop(double duration_sec){
    auto execution_duration = std::chrono::duration<double>(duration_sec);
    
    bool execute = true;
    {
        //Wait to be executed or terminated
        std::unique_lock<std::mutex> lock(execution_mutex_);
        execution_lock_condition_.wait(lock);
        if(terminate_flag_){
            execute = false;
        }
    }

    std::cout << "-------------[Execution]------------" << std::endl;

    if(execute){
        std::cout << "* Activating Deployment" << std::endl;
        
        //Send Activate function
        auto activate = new NodeManager::ControlMessage();
        activate->set_type(NodeManager::ControlMessage::ACTIVATE);
        PushMessage("*", activate);

        {
            std::unique_lock<std::mutex> lock(execution_mutex_);
            if(duration_sec == -1){
                //Wait indefinately
                execution_lock_condition_.wait(lock, [this]{return this->terminate_flag_;});
            }else{
                execution_lock_condition_.wait_for(lock, execution_duration, [this]{return this->terminate_flag_;});
            }
        }

        std::cout << "* Passivating Deployment" << std::endl;
        //Send Terminate Function
        auto passivate = new NodeManager::ControlMessage();
        passivate->set_type(NodeManager::ControlMessage::PASSIVATE);
        PushMessage("*", passivate);
    }
    
    std::cout << "* Terminating Deployment" << std::endl;
    //Send Terminate Function
    auto terminate = new NodeManager::ControlMessage();
    terminate->set_type(NodeManager::ControlMessage::TERMINATE);
    PushMessage("*", terminate);

    std::this_thread::sleep_for(std::chrono::seconds(2));
    execution_->Interrupt();
}
