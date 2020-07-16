#include "executionmanager.h"
#include <chrono>
#include "helper.h"
#include <iostream>

ExecutionManager::ExecutionManager(
                            Execution& execution,
                            int duration_sec,
                            const std::string& experiment_name,
                            const std::string& master_publisher_endpoint,
                            const std::string& master_registration_endpoint,
                            const std::string& master_heartbeat_endpoint,
                            const std::string& experiment_logger_endpoint
                            ):
    execution_(execution),
    experiment_name_(experiment_name),
    master_publisher_endpoint_(master_publisher_endpoint),
    master_registration_endpoint_(master_registration_endpoint),
    experiment_logger_endpoint_(experiment_logger_endpoint) 
{
    //Register our Termination Function
    execution_.AddTerminateCallback(std::bind(&ExecutionManager::Terminate, this));

    //Setup our writer
    proto_writer_ = std::unique_ptr<zmq::ProtoWriter>(new zmq::ProtoWriter());
    proto_writer_->BindPublisherSocket(master_publisher_endpoint_);

    experiment_logger_ = std::unique_ptr<Logan::ExperimentLogger>(new Logan::ExperimentLogger(experiment_name_, experiment_logger_endpoint_, ::Logger::Mode::LIVE));

    //Starting our HeartBeat Requester with the EnvironmentManager
    requester_ = std::unique_ptr<EnvironmentRequest::HeartbeatRequester>(new EnvironmentRequest::HeartbeatRequester(master_heartbeat_endpoint, std::bind(&ExecutionManager::HandleExperimentUpdate, this, std::placeholders::_1)));
    requester_->SetTimeoutCallback(std::bind(&Execution::Interrupt, &execution_));

    RequestDeployment();

    //Construct the execution future
    execution_future_ = std::async(std::launch::async, &ExecutionManager::ExecutionLoop, this, duration_sec, execute_promise_.get_future(), terminate_promise_.get_future());

    //Bind Registration functions
    slave_registration_handler_ = std::unique_ptr<zmq::ProtoReplier>(new zmq::ProtoReplier());
    slave_registration_handler_->RegisterProtoCallback<NodeManager::SlaveStartupRequest, NodeManager::SlaveStartupReply>("SlaveStartup", std::bind(&ExecutionManager::HandleSlaveStartup, this, std::placeholders::_1));
    slave_registration_handler_->RegisterProtoCallback<NodeManager::SlaveConfiguredRequest, NodeManager::SlaveConfiguredReply>("SlaveConfigured", std::bind(&ExecutionManager::HandleSlaveConfigured, this, std::placeholders::_1));
    slave_registration_handler_->RegisterProtoCallback<NodeManager::SlaveTerminatedRequest, NodeManager::SlaveTerminatedReply>("SlaveTerminated", std::bind(&ExecutionManager::HandleSlaveTerminated, this, std::placeholders::_1));
    slave_registration_handler_->RegisterProtoCallback<NodeManager::SlaveHeartbeatRequest, NodeManager::SlaveHeartbeatReply>("SlaveHeartbeat", std::bind(&ExecutionManager::HandleSlaveHeartbeat, this, std::placeholders::_1));
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
    auto now = std::chrono::steady_clock::now();

    for(const auto& node : control_message_->nodes()){
        for(const auto& container : node.containers()){
            const auto& slave_key = GetSlaveKey(node.ip_address(), container.info().id());

            auto insert = slave_status_.emplace(slave_key, SlaveStatus{SlaveState::OFFLINE, now});
            if(!insert.second){
                throw std::runtime_error("Got duplicate slaves in ControlMessage with IP: '" + slave_key + "'");
            }
        }
    }
}

ExecutionManager::SlaveState ExecutionManager::GetSlaveState(const std::string& slave_key){
    return GetSlaveStatus(slave_key).state;
}

void ExecutionManager::SetSlaveState(const std::string& slave_key, SlaveState state){
    GetSlaveStatus(slave_key).state = state;
    HandleSlaveStateChange();
}

void ExecutionManager::SetSlaveAlive(const std::string& slave_key){
    GetSlaveStatus(slave_key).heartbeat_time = std::chrono::steady_clock::now();
}

ExecutionManager::SlaveStatus& ExecutionManager::GetSlaveStatus(const std::string& slave_key){
    if(slave_status_.count(slave_key)){
        return slave_status_.at(slave_key);
    }else if(late_joiner_slave_status_.count(slave_key)){
        return late_joiner_slave_status_.at(slave_key);
    }else{
        throw std::runtime_error("Slave with Key: '" + slave_key + "' not in experiment");
    }
}

int ExecutionManager::GetSlaveStateCount(const SlaveState& state){
    int count = 0;
    for(const auto& ss : slave_status_){
        if(ss.second.state == state){
            count ++;
        }
    }
    return count;
}

void ExecutionManager::HandleSlaveStateChange(){
    //Whats our executions state
    auto slave_count = slave_status_.size();

    auto configured_count = GetSlaveStateCount(SlaveState::CONFIGURED);
    auto terminated_count = GetSlaveStateCount(SlaveState::TERMINATED);
    auto error_count = GetSlaveStateCount(SlaveState::Error);
    
    if(configured_count == slave_count){
        //All Slaves are configured, so Start
        // REVIEW (Mitch): This future/promise pair is getting used as a semaphore, this is a bad D:
        execute_promise_.set_value();
    }
    else if((configured_count + error_count) == slave_count){
        //All Slaves are either configured or have errored, so fail
        std::runtime_error error(std::to_string(error_count) + " slaves has errors.");
        execute_promise_.set_exception(std::make_exception_ptr(error));
    }
    slave_state_cv_.notify_all();
}

// REVIEW (Mitch): This function should be called AllSlavesTerminatedOrTimedOut
// REVIEW (Mitch): This function also hides any particular slave failure on heartbeat timeout
bool ExecutionManager::AllSlavesTerminated(){
    const auto now = std::chrono::steady_clock::now();

    if(execution_valid_){
        // REVIEW (Mitch): Re-designing this algorithm's control flow is probably a good idea
        for(const auto& ss : slave_status_){
            if(ss.second.state == SlaveState::TERMINATED){
                continue;
            }else{
                auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now - ss.second.heartbeat_time);
                if(milliseconds.count() > 5000){
                    continue;
                }
            }
            return false;
        }
    }
    return true;
}


const std::string ExecutionManager::GetSlaveKey(const std::string& ip, const std::string& container_id){
    return ip + "_" + container_id;
}

const std::string ExecutionManager::GetSlaveKey(const NodeManager::SlaveId& slave){
    return GetSlaveKey(slave.ip_address(), slave.container_id());
}

// REVIEW (Mitch): Slave operation failure should be handled when they're encountered. Currently
//  we're waiting till we have messages from all slaves before acting on any one failure.
std::unique_ptr<NodeManager::SlaveHeartbeatReply> ExecutionManager::HandleSlaveHeartbeat(const NodeManager::SlaveHeartbeatRequest request){
    using namespace NodeManager;
    const auto& slave_key = GetSlaveKey(request.id());
    {
        std::lock_guard<std::mutex> slave_lock(slave_state_mutex_);
        SetSlaveAlive(slave_key);
    }
    return std::unique_ptr<SlaveHeartbeatReply>(new SlaveHeartbeatReply());
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
                        reply->set_host_name(node.info().name());
                        reply->set_allocated_configuration(new Container(container));
                        //Set the slave state as Registered
                        SetSlaveState(slave_key, SlaveState::Registered);
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
    if(GetSlaveState(slave_key) == SlaveState::Registered){
        reply = std::unique_ptr<SlaveConfiguredReply>(new SlaveConfiguredReply());
        auto slave_state = SlaveState::CONFIGURED;
        if(request.success()){
            std::cout << "* Slave: " << slave_key << " Online" << std::endl;
        }else{
            std::cerr << "* Slave: " << slave_key << " Error!" << std::endl;
            for(const auto& error_str : request.error_messages()){
                std::cerr << "* [" << slave_key << "]: " << error_str << std::endl;
            }
            slave_state = SlaveState::Error;
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

Logger::LifeCycleEvent TranslateLifecycle(const NodeManager::ControlMessage::Type& state){
    switch(state){
        case NodeManager::ControlMessage::ACTIVATE:
            return Logger::LifeCycleEvent::ACTIVATED;
        case NodeManager::ControlMessage::CONFIGURE:
            return Logger::LifeCycleEvent::CONFIGURED;
        case NodeManager::ControlMessage::PASSIVATE:
            return Logger::LifeCycleEvent::PASSIVATED;
        case NodeManager::ControlMessage::TERMINATE:
            return Logger::LifeCycleEvent::TERMINATED;
        default:
            throw std::runtime_error("Unhandled State");
    }
}

void ExecutionManager::PushStateChange(const NodeManager::ControlMessage::Type& state){
    PushControlMessage("*", ConstructStateControlMessage(state));
    try{
        std::cerr << "* Experiment State Change: [" << NodeManager::ControlMessage::Type_Name(state) << "]" << std::endl;
        auto lifecycle_event = TranslateLifecycle(state);
        if(experiment_logger_){
            experiment_logger_->LogLifecycleEvent(lifecycle_event);
        }
    }catch(const std::exception& ex){
        // REVIEW (Mitch): Do something with this exception
    }
}

void ExecutionManager::ExecutionLoop(int duration_sec, std::future<void> execute_future, std::future<void> terminate_future){
    using namespace NodeManager;

    bool should_execute = true;
    try{
        if(execute_future.valid()){
            // REVIEW (Mitch): this future is used as a semaphore. It gets by a call to
            //  execute_promise_.set_value(). Use a condition variable here?
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

        PushStateChange(ControlMessage::ACTIVATE);
        

        if(terminate_future.valid()){
            std::future_status status = std::future_status::ready;
            if(duration_sec != -1){
                //Sleep for the predetermined time
                status = terminate_future.wait_for(std::chrono::seconds(duration_sec));
            }

            if(status == std::future_status::ready){
                try{
                    // REVIEW (Mitch): what can this throw and under which circumstances?
                    terminate_future.get();
                }catch(const std::exception& ex){
                }
            }
        }
        PushStateChange(ControlMessage::PASSIVATE);
    }

    PushStateChange(ControlMessage::TERMINATE);

    std::cout << "--------[Slave De-registration]--------" << std::endl;
    {
        while(true){
            std::unique_lock<std::mutex> slave_lock(slave_state_mutex_);
            //Wait for all slaves to be offline or timed out
            if(slave_state_cv_.wait_for(slave_lock, std::chrono::seconds(1), [this]{return AllSlavesTerminated();})){
                break;
            }
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
