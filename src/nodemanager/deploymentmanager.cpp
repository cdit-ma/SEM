#include "deploymentmanager.h"

#include <iostream>

#include "execution.hpp"

#include <re_common/zmq/protoreceiver/protoreceiver.h>
#include "controlmessage/controlmessage.pb.h"
#include "controlmessage/translate.h"


DeploymentManager::DeploymentManager(std::string library_path, Execution* execution){
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

void DeploymentManager::InteruptQueueThread(){
    std::unique_lock<std::mutex> lock(notify_mutex_);
    if(!terminate_){
        std::cout << "* Interupting DeploymentManager" << std::endl;
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

bool DeploymentManager::SetupControlMessageReceiver(std::string pub_endpoint, std::string host_name){
    if(subscriber_){
        subscriber_->Connect(pub_endpoint);
        subscriber_->Filter(host_name + "*");
        return true;
    }
    return false;
}

bool DeploymentManager::SetupModelLogger(std::string pub_endpoint, std::string host_name, ModelLogger::Mode mode){
    return ModelLogger::setup_model_logger(host_name, pub_endpoint, mode);
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

bool DeploymentManager::ProcessStartupMessage(const std::string& startup_str){
    NodeManager::ControlMessage control_message;
    auto success = control_message.ParseFromString(startup_str);
    if(success){
        return ConfigureDeploymentContainer(control_message);
    }
    return false;
}

bool DeploymentManager::ConfigureDeploymentContainer(const NodeManager::ControlMessage& control_message){
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
                    auto success = ConfigureDeploymentContainer(control_message);
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