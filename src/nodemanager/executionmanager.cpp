#include "executionmanager.h"
#include <chrono>
#include <proto/controlmessage/helper.h>
#include <iostream>

ExecutionManager::ExecutionManager(
                            Execution& execution,
                            int duration_sec,
                            const std::string& experiment_name,
                            const std::string& master_publisher_endpoint,
                            const std::string& master_registration_endpoint,
                            const std::string& master_heartbeat_endpoint):
    execution_(execution),
    experiment_name_(experiment_name),
    master_publisher_endpoint_(master_publisher_endpoint),
    master_registration_endpoint_(master_registration_endpoint)
{
    //Register our Termination Function
    execution_.AddTerminateCallback(std::bind(&ExecutionManager::Terminate, this));

    //Setup our writer
    proto_writer_ = std::unique_ptr<zmq::ProtoWriter>(new zmq::ProtoWriter());
    proto_writer_->BindPublisherSocket(master_publisher_endpoint_);

    //Starting our HeartBeat Requester with the EnvironmentManager
    requester_ = std::unique_ptr<EnvironmentRequest::HeartbeatRequester>(new EnvironmentRequest::HeartbeatRequester(master_heartbeat_endpoint, std::bind(&ExecutionManager::HandleExperimentUpdate, this, std::placeholders::_1)));
    requester_->SetTimeoutCallback(std::bind(&Execution::Interrupt, &execution_));

    RequestDeployment();

    //Construct the execution future
    execution_future_ = std::async(std::launch::async, &ExecutionManager::ExecutionLoop, this, duration_sec, execute_promise_.get_future(), terminate_promise_.get_future(), slave_deregistration_promise_.get_future());

    //Bind Registration functions
    slave_registration_handler_ = std::unique_ptr<zmq::ProtoReplier>(new zmq::ProtoReplier());
    slave_registration_handler_->RegisterProtoCallback<NodeManager::SlaveStartupRequest, NodeManager::SlaveStartupReply>("SlaveStartup", std::bind(&ExecutionManager::HandleSlaveStartup, this, std::placeholders::_1));
    slave_registration_handler_->RegisterProtoCallback<NodeManager::SlaveConfiguredRequest, NodeManager::SlaveConfiguredReply>("SlaveConfigured", std::bind(&ExecutionManager::HandleSlaveConfigured, this, std::placeholders::_1));
    slave_registration_handler_->RegisterProtoCallback<NodeManager::SlaveTerminatedRequest, NodeManager::SlaveTerminatedReply>("SlaveTerminated", std::bind(&ExecutionManager::HandleSlaveTerminated, this, std::placeholders::_1));
    slave_registration_handler_->Bind(master_registration_endpoint_);
    slave_registration_handler_->Start();
};

ExecutionManager::~ExecutionManager(){
    Terminate();
}

void ExecutionManager::Terminate(){
    auto error = std::runtime_error("Shutdown");
    try{
        execute_promise_.set_exception(std::make_exception_ptr(error));
    }catch(const std::exception& ex){
    
    }

    try{
        terminate_promise_.set_exception(std::make_exception_ptr(error));
    }catch(const std::exception& ex){
    
    }

    try{
        std::lock_guard<std::mutex> lock(slave_state_mutex_);
        if(!execution_valid_){
            //Only Ignore Deregistration if we didn't hit proper execution
            slave_deregistration_promise_.set_exception(std::make_exception_ptr(error));
        }
    }catch(const std::exception& ex){
    
    }

    if(execution_future_.valid()){
        try{
            execution_future_.get();
        }catch(const std::exception& ex){
            std::cerr << "ExecutionManager::Terminate: " << ex.what() << std::endl;
        }
    }
}

void ExecutionManager::RequestDeployment(){
    using namespace NodeManager;
    //Request the Experiment Info
    auto environment_message = requester_->GetExperimentInfo();

    //Take a copy of the control message
    std::lock_guard<std::mutex> lock(control_message_mutex_);
    control_message_ = std::unique_ptr<ControlMessage>(new ControlMessage(environment_message->control_message()));

    std::lock_guard<std::mutex> slave_lock(slave_state_mutex_);
    //Construct a set of slaves to wait
    for(const auto& node : control_message_->nodes()){
        for(const auto& container : node.containers()){
            const auto& slave_key = GetSlaveKey(node.ip_address(), container.info().id());

            auto insert = slave_states_.emplace(slave_key, SlaveState::OFFLINE);
            if(!insert.second){
                throw std::runtime_error("Got duplicate slaves in ControlMessage with IP: '" + slave_key + "'");
            }
        }
    }
}

ExecutionManager::SlaveState ExecutionManager::GetSlaveState(const std::string& slave_key){
    if(slave_states_.count(slave_key)){
        return slave_states_.at(slave_key);
    }else if(late_joiner_slave_states_.count(slave_key)){
        return late_joiner_slave_states_.at(slave_key);
    }else{
        throw std::runtime_error("Slave with Key: '" + slave_key + "' not in experiment");
    }
}

void ExecutionManager::SetSlaveState(const std::string& ip_address, SlaveState state){
    slave_states_.at(ip_address) = state;
    HandleSlaveStateChange();
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

void ExecutionManager::HandleSlaveStateChange(){
    //Whats our executions state
    auto slave_count = slave_states_.size();
    auto configured_count = GetSlaveStateCount(SlaveState::CONFIGURED);
    auto terminated_count = GetSlaveStateCount(SlaveState::TERMINATED);
    auto error_count = GetSlaveStateCount(SlaveState::ERROR_);
    
    if(configured_count == slave_count){
        //All Slaves are configured, so Start
        execute_promise_.set_value();
    }
    else if((configured_count + error_count) == slave_count){
        //All Slaves are either configured or have errored, so fail
        std::runtime_error error(std::to_string(error_count) + " slaves has errors.");
        execute_promise_.set_exception(std::make_exception_ptr(error));
    }
    else if(terminated_count == slave_count){
        //All Slaves are terminated
        slave_deregistration_promise_.set_value();
    }
}


const std::string ExecutionManager::GetSlaveKey(const std::string& ip, const std::string& container_id){
    return ip + "_" + container_id;
}

const std::string ExecutionManager::GetSlaveKey(const NodeManager::SlaveId& slave){
    return GetSlaveKey(slave.ip_address(), slave.container_id());
}

std::unique_ptr<NodeManager::SlaveStartupReply> ExecutionManager::HandleSlaveStartup(const NodeManager::SlaveStartupRequest& request){
    using namespace NodeManager;
    std::unique_ptr<SlaveStartupReply> reply;
    const auto& slave_key = GetSlaveKey(request.id());

    std::lock_guard<std::mutex> slave_lock(slave_state_mutex_);
    if(GetSlaveState(slave_key) == SlaveState::OFFLINE){
        std::lock_guard<std::mutex> control_lock(control_message_mutex_);

        for(const auto& node : control_message_->nodes()){
            if(request.id().ip_address() == node.ip_address()){
                for(const auto& container : node.containers()){
                    if(request.id().container_id() == container.info().id()){
                        //Give them there right container
                        reply = std::unique_ptr<SlaveStartupReply>(new SlaveStartupReply());
                        reply->set_allocated_configuration(new Container(container));
                        //Set the slave state as Registered
                        SetSlaveState(slave_key, SlaveState::REGISTERED);
                    }
                }

            }
        }
    }else{
        throw std::runtime_error("Slave with Key: '" + slave_key + "' not in correct state");
    }
    return reply;
}

std::unique_ptr<NodeManager::SlaveConfiguredReply> ExecutionManager::HandleSlaveConfigured(const NodeManager::SlaveConfiguredRequest& request){
    using namespace NodeManager;
    std::unique_ptr<SlaveConfiguredReply> reply;
    const auto& slave_key = GetSlaveKey(request.id());

    std::lock_guard<std::mutex> slave_lock(slave_state_mutex_);
    if(GetSlaveState(slave_key) == SlaveState::REGISTERED){
        reply = std::unique_ptr<SlaveConfiguredReply>(new SlaveConfiguredReply());
        auto slave_state = SlaveState::CONFIGURED;
        if(request.success()){
            std::cout << "* Slave: " << slave_key << " Online" << std::endl;
        }else{
            std::cerr << "* Slave: " << slave_key << " Error!" << std::endl;
            for(const auto& error_str : request.error_messages()){
                std::cerr << "* [" << slave_key << "]: " << error_str << std::endl;
            }
            slave_state = SlaveState::ERROR_;
        }
        //Set the slave state as Configured or Error'd
        SetSlaveState(slave_key, slave_state);
    }else{
        throw std::runtime_error("Slave with IP: '" + slave_key + "' not in correct state");
    }
    return reply;
}

std::unique_ptr<NodeManager::SlaveTerminatedReply> ExecutionManager::HandleSlaveTerminated(const NodeManager::SlaveTerminatedRequest& request){
    using namespace NodeManager;
    const auto& slave_key = GetSlaveKey(request.id());
    std::lock_guard<std::mutex> slave_lock(slave_state_mutex_);
    auto reply = std::unique_ptr<SlaveTerminatedReply>(new SlaveTerminatedReply());
    SetSlaveState(slave_key, SlaveState::TERMINATED);
    return reply;
}

void ExecutionManager::PushControlMessage(const std::string& topic, std::unique_ptr<NodeManager::ControlMessage> message){
    if(proto_writer_){
        proto_writer_->PushMessage(topic, std::move(message));
    }
}

void ExecutionManager::HandleExperimentUpdate(const NodeManager::EnvironmentMessage& environment_update){
    using namespace NodeManager;
    //TODO: filter only nodes we want to update???
    //can we even do that??
    const auto& type = environment_update.type();

    switch(type){
        case EnvironmentMessage::CONFIGURE_EXPERIMENT:{
            auto control_message = std::unique_ptr<ControlMessage>(new ControlMessage(environment_update.control_message()));
            PushControlMessage("*", std::move(control_message));
            break;
        }
        case EnvironmentMessage::SHUTDOWN_EXPERIMENT:{
            execution_.Interrupt();  
            break;

        }
        default:{
            throw std::runtime_error("Unhandle EnvironmentMessage Type: " + EnvironmentMessage_Type_Name(type));
        }
    }
}


std::unique_ptr<NodeManager::ControlMessage> ExecutionManager::ConstructStateControlMessage(NodeManager::ControlMessage::Type type){
    auto message = std::unique_ptr<NodeManager::ControlMessage>(new NodeManager::ControlMessage());
    message->set_type(type);
    return message;
}

void ExecutionManager::ExecutionLoop(int duration_sec, std::future<void> execute_future, std::future<void> terminate_future, std::future<void> slave_deregistration_future){
    using namespace NodeManager;

    bool should_execute = true;
    try{
        if(execute_future.valid()){
            execute_future.get();
            //Sleep to allow time for the publisher to be bound
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        std::lock_guard<std::mutex> lock(slave_state_mutex_);
        execution_valid_ = true;
    }catch(const std::exception& ex){
        should_execute = false;
    }

    if(should_execute){
        std::cout << "-------------[Execution]------------" << std::endl;
        
        std::cout << "* Activating Deployment" << std::endl;
        PushControlMessage("*", ConstructStateControlMessage(ControlMessage::ACTIVATE));

        if(terminate_future.valid()){
            std::future_status status = std::future_status::ready;
            if(duration_sec != -1){
                //Sleep for the predetermined time
                status = terminate_future.wait_for(std::chrono::seconds(duration_sec));
            }

            if(status == std::future_status::ready){
                try{
                    terminate_future.get();
                }catch(const std::exception& ex){
                }
            }
        }
        std::cout << "* Passivating Deployment" << std::endl;
        PushControlMessage("*", ConstructStateControlMessage(ControlMessage::PASSIVATE));
    }
    
    std::cout << "* Terminating Deployment" << std::endl;
    PushControlMessage("*", ConstructStateControlMessage(ControlMessage::TERMINATE));

    std::cout << "--------[Slave De-registration]--------" << std::endl;
    if(slave_deregistration_future.valid()){
        try{
            slave_deregistration_future.get();
        }catch(const std::exception& ex){
        }
    }

    try{
        std::cout << "--------[Removing Deployment]--------" << std::endl;
        requester_->RemoveDeployment();
    }catch(const std::exception& ex){
        std::cerr << "* Removing Deployment Exception: " << ex.what() << std::endl;
    }

    //Interupt the Execution
    std::lock_guard<std::mutex> lock(execution_mutex_);
    execution_.Interrupt();
}
