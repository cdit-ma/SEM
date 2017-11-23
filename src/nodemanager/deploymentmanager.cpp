#include "deploymentmanager.h"

#include <iostream>

#include "execution.hpp"

#include <re_common/zmq/protoreceiver/protoreceiver.h>
#include "controlmessage/controlmessage.pb.h"
#include "controlmessage/translate.h"


DeploymentManager::DeploymentManager(const std::string& library_path, Execution* execution){
    std::unique_lock<std::mutex> lock(mutex_);
    library_path_ = library_path;
    execution_ = execution;

    //Construct a live receiever
    subscriber_ = new zmq::ProtoReceiver();

    execution_->AddTerminateCallback(std::bind(&DeploymentManager::InteruptQueueThread, this));

    //Get all Main messages
    subscriber_->Filter("*");
    
    //Subscribe to NodeManager::ControlMessage Types
    subscriber_->RegisterProtoCallback<NodeManager::ControlMessage>(std::bind(&DeploymentManager::GotControlMessage, this, std::placeholders::_1));

    if(!control_queue_thread_){
        //Construct a thread to process the control queue
        control_queue_thread_ = new std::thread(&DeploymentManager::ProcessControlQueue, this);
    }

    subscriber_->Start();
}

NodeManager::StartupResponse DeploymentManager::HandleStartup(const NodeManager::Startup startup){
    NodeManager::StartupResponse slave_response;
    bool success = true;

    const auto& host_name = startup.host_name();
    //Handle Logger setup
    {   
        const auto& logger = startup.logger();
        
        bool setup_logger = ModelLogger::setup_model_logger(host_name, logger.publisher_address(), (ModelLogger::Mode)logger.mode());
        success = setup_logger ? success : false;

        if(!setup_logger){
            slave_response.add_error_codes("Setting Model Logger Failed");
        }
    }


    //Setup our subscriber
    {
         if(subscriber_){
             if(!subscriber_->Connect(startup.publisher_address())){
                 slave_response.add_error_codes("Subscriber couldn't connect to: '" + startup.publisher_address() + "'");
                 success = false;
             }
             if(!subscriber_->Filter(host_name + "*")){
                 slave_response.add_error_codes("Subscriber couldn't attach filter: '" + host_name + "*'");
                 success = false;
             }
        }else{
            success = false;
        }
    }

    {
        bool configure_deployment = ConfigureDeploymentContainers(startup.configure());
        success = configure_deployment ? success : false;
        if(!configure_deployment){
            slave_response.add_error_codes("Deployment Containers failed to be configured");
        }
    }
    slave_response.set_host_name(host_name);
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

    if(control_queue_thread_){
        control_queue_thread_->join();
        delete control_queue_thread_;
        control_queue_thread_ = 0;
    }
}

bool DeploymentManager::TeardownModelLogger(){
    return ModelLogger::shutdown_logger();
}

void DeploymentManager::GotControlMessage(const NodeManager::ControlMessage& control_message){
    //Gain mutex lock and append message to queue
    std::unique_lock<std::mutex> lock(notify_mutex_);
    //Have to copy the message
    control_message_queue_.push(NodeManager::ControlMessage(control_message));
    notify_lock_condition_.notify_all();
}



bool DeploymentManager::ConfigureDeploymentContainers(const NodeManager::ControlMessage& control_message){
    bool success = true;
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
            success = node_container->Configure(node) ? success : false;
        }else{
            std::cerr << "DeploymentManager: Cannot find node '" << node_name << "'" << std::endl;
        }
    }
    return success;
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
            
            if(terminate_){
                return;
            }
        }

         //Process the queue
         while(!queue_.empty()){
            const auto& control_message = queue_.front();

            switch(control_message.type()){
                case NodeManager::ControlMessage::STARTUP:
                case NodeManager::ControlMessage::SET_ATTRIBUTE:{
                    auto success = ConfigureDeploymentContainers(control_message);
                    std::cout << "NodeManager::ControlMessage::Configure: " << (success ? "YES" : "NO") << std::endl;
                    break;
                }
                case NodeManager::ControlMessage::ACTIVATE:{
                    bool success = true;
                    for(auto c : deployment_containers_){
                        success = c.second->Activate() ? success : false;
                    }
                    std::cout << "NodeManager::ControlMessage::Activate: " << (success ? "YES" : "NO") << std::endl;
                    break;
                }
                case NodeManager::ControlMessage::PASSIVATE:{
                    bool success = true;
                    for(auto c : deployment_containers_){
                        success = c.second->Passivate() ? success : false;
                    }
                    std::cout << "NodeManager::ControlMessage::PASSIVATE: " << (success ? "YES" : "NO") << std::endl;
                    break;
                }
                case NodeManager::ControlMessage::TERMINATE:{
                    bool success = true;
                    for(auto c : deployment_containers_){
                        success = c.second->Terminate() ? success : false;
                    }
                    std::cout << "NodeManager::ControlMessage::TERMINATE: " << (success ? "YES" : "NO") << std::endl;
                    InteruptQueueThread();
                    execution_->Interrupt();
                    break;
                }
                default:
                    break;
            }
            queue_.pop();
        }
    }
}