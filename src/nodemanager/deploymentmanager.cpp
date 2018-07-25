#include "deploymentmanager.h"

#include <iostream>

#include <re_common/util/execution.hpp>

#include <re_common/zmq/protoreceiver/protoreceiver.h>
#include <re_common/zmq/environmentrequester/environmentrequester.h>

DeploymentManager::DeploymentManager(bool on_master_node,
                                    const std::string& library_path,
                                    Execution* execution,
                                    const std::string& experiment_id,
                                    const std::string& ip_address,
                                    const std::string& environment_manager_endpoint){
    std::unique_lock<std::mutex> lock(mutex_);


    

    on_master_node_ = on_master_node;
    library_path_ = library_path;
    execution_ = execution;
    experiment_id_ = experiment_id;
    ip_address_ = ip_address;
    environment_manager_endpoint_ = environment_manager_endpoint;

    //Construct a live receiever
    subscriber_ = new zmq::ProtoReceiver();

    execution_->AddTerminateCallback(std::bind(&DeploymentManager::InteruptQueueThread, this));

    //Subscribe to NodeManager::ControlMessage Types
    subscriber_->RegisterProtoCallback<NodeManager::ControlMessage>(std::bind(&DeploymentManager::GotControlMessage, this, std::placeholders::_1));

    //Construct a thread to process the control queue
    control_queue_future_ = std::async(std::launch::async, &DeploymentManager::ProcessControlQueue, this);

    registrant_ = std::unique_ptr<zmq::Registrant>(new zmq::Registrant(*this));
}

std::string DeploymentManager::GetSlaveIPAddress(){
    return ip_address_;
}

std::string DeploymentManager::GetMasterRegistrationEndpoint(){
    return master_registration_endpoint_;
}

bool DeploymentManager::QueryEnvironmentManager(){

    EnvironmentRequester requester(environment_manager_endpoint_, experiment_id_, EnvironmentRequester::DeploymentType::RE_SLAVE);
    requester.Init(environment_manager_endpoint_);

    while(true){
        NodeManager::ControlMessage response;
        
        try{
            response = requester.NodeQuery(ip_address_);
        }catch(const std::runtime_error& ex){
            //Commlunication with environment manager has likely timed out. Return blank string.
            std::cerr << "Response from env manager timed out, terminating" << std::endl;
            return false;
        }

        switch(response.type()){
            case NodeManager::ControlMessage::TERMINATE:{
                //Node isn't part of execution
                return false;
            }
            case NodeManager::ControlMessage::CONFIGURE:{
                auto& node = response.nodes(0);
            
                for(auto& attribute : node.attributes()){
                    if(attribute.info().name() == "master_registration_endpoint"){
                        master_registration_endpoint_ = attribute.s(0);
                    }else if(attribute.info().name() == "master_publisher_endpoint"){
                        master_publisher_endpoint_ = attribute.s(0);
                    }
                }
                //Probably a success
                return master_registration_endpoint_.size() && master_registration_endpoint_.size();
            }
            default:{
                std::this_thread::sleep_for(std::chrono::seconds(1));
                //Please Continue
                break;
            }
        }
    }

    return false;
}

NodeManager::SlaveStartupResponse DeploymentManager::HandleSlaveStartup(const NodeManager::SlaveStartup startup){
    NodeManager::SlaveStartupResponse slave_response;
    bool success = true;

    const auto& host_name = startup.slave_host_name();
    
   

    //Setup our subscriber
    if(subscriber_){
        subscriber_->Connect(master_publisher_endpoint_);
        subscriber_->Filter("*");
        subscriber_->Filter(host_name + "*");
        subscriber_->Start();
    }else{
        success = false;
    }

    try{
        if(success){
            ConfigureDeploymentContainers(startup.configure());
        }
    }catch(const std::exception& ex){
        slave_response.add_error_codes(ex.what());
        success = false;
    }

    slave_response.set_slave_ip(ip_address_);
    slave_response.set_success(success);
    return slave_response;
}

void DeploymentManager::InteruptQueueThread(){
    std::unique_lock<std::mutex> lock(notify_mutex_);
    if(!terminate_){
        terminate_ = true;
    }
    notify_lock_condition_.notify_all();
}

DeploymentManager::~DeploymentManager(){
    std::unique_lock<std::mutex> lock(mutex_);
    
    if(subscriber_){
        subscriber_->Terminate();
        delete subscriber_;
        subscriber_ =  0;
    }


    control_queue_future_.get();
}

void DeploymentManager::Teardown(){
    ModelLogger::shutdown_logger();
    if(!on_master_node_){
        execution_->Interrupt();
    }
}

void DeploymentManager::GotControlMessage(const NodeManager::ControlMessage& control_message){
    //Gain mutex lock and append message to queue
    std::unique_lock<std::mutex> lock(notify_mutex_);
    //Have to copy the message onto our queue
    control_message_queue_.emplace(control_message);
    notify_lock_condition_.notify_all();
}

void DeploymentManager::ConfigureDeploymentContainers(const NodeManager::ControlMessage& control_message){
    for(const auto& node : control_message.nodes()){
        const auto& node_name = node.info().name();
        
        std::shared_ptr<DeploymentContainer> node_container;
        if(deployment_containers_.count(node_name)){
            node_container = deployment_containers_[node_name];
        }else{
            //Construct one
            node_container = std::make_shared<DeploymentContainer>();
            deployment_containers_[node_name] = node_container;
            node_container->SetLibraryPath(library_path_);
        }
        
        if(node_container){
            node_container->Configure(node);
        }else{
            throw std::runtime_error("DeploymentManager: Cannot find node: " + node_name);
        }
    }
}

std::shared_ptr<DeploymentContainer> DeploymentManager::GetDeploymentContainer(const std::string& node_name){
    if(deployment_containers_.count(node_name)){
        return deployment_containers_[node_name];
    }
    return nullptr;
}

void DeploymentManager::ProcessControlQueue(){
    std::queue<NodeManager::ControlMessage> queue_;
    
    while(true){
        {
            //Gain Mutex
            std::unique_lock<std::mutex> lock(notify_mutex_);
            notify_lock_condition_.wait(lock, [this]{return terminate_ || !control_message_queue_.empty();});
            //Swap out the queue's and release the mutex
            control_message_queue_.swap(queue_);
            
            if(terminate_ && queue_.empty()){
                return;
            }
        }

        //Process the queue
        while(!queue_.empty()){
            auto control_message = std::move(queue_.front());
            queue_.pop();
            auto start = std::chrono::steady_clock::now();
            switch(control_message.type()){
                case NodeManager::ControlMessage::CONFIGURE:
                case NodeManager::ControlMessage::STARTUP:
                case NodeManager::ControlMessage::SET_ATTRIBUTE:{
                    ConfigureDeploymentContainers(control_message);
                    break;
                }
                case NodeManager::ControlMessage::ACTIVATE:{
                    for(const auto& c : deployment_containers_){
                        c.second->Activate();
                    }
                    break;
                }
                case NodeManager::ControlMessage::PASSIVATE:{
                    for(const auto& c : deployment_containers_){
                        c.second->Passivate();
                    }
                    break;
                }
                case NodeManager::ControlMessage::TERMINATE:{
                    for(const auto& c : deployment_containers_){
                        c.second->Terminate();
                    }

                    ModelLogger::shutdown_logger();
                    
                    //Send the NodeManagerMaster that we are dead
                    registrant_->SendTerminated();
                    
                    //Interupt and die
                    InteruptQueueThread();
                    if(!on_master_node_){
                        execution_->Interrupt();
                    }
                    break;
                }
                default:
                    break;
            }

            auto ms = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start);
            std::cout << "* " << NodeManager::ControlMessage_Type_Name(control_message.type()) << " Deployment took: " << ms.count() << " us" << std::endl;
        }
    }
}
