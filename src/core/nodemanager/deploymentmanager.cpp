#include "deploymentmanager.h"

#include <iostream>

#include "execution.hpp"

#include "../../re_common/zmq/protoreceiver/protoreceiver.h"
#include "../controlmessage/controlmessage.pb.h"
#include "../controlmessage/translate.h"

#include <google/protobuf/util/json_util.h>
#include <google/protobuf/message.h>

DeploymentManager::DeploymentManager(std::string library_path, Execution* execution){
    library_path_ = library_path;
    execution_ = execution;

    //Construct a live receiever
    subscriber_ = new zmq::ProtoReceiver();
    //Get all Main messages
    subscriber_->Filter("*");
    
    //Subscribe to NodeManager::ControlMessage Types
    auto cm_callback = std::bind(&DeploymentManager::GotControlMessage, this, std::placeholders::_1);
    subscriber_->RegisterNewProto(NodeManager::ControlMessage::default_instance(), cm_callback);

    if(!control_queue_thread_){
        //Construct a thread to process the control queue
        control_queue_thread_ = new std::thread(&DeploymentManager::ProcessControlQueue, this);
    }

    subscriber_->Start();
}

DeploymentManager::~DeploymentManager(){
    if(deployment_){
        std::cout << "~1DeploymentManager" << std::endl;
        delete deployment_;
        deployment_ = 0;
        std::cout << "~2DeploymentManager" << std::endl;
    }
}

bool DeploymentManager::SetupControlMessageReceiver(std::string pub_endpoint, std::string host_name){
    if(subscriber_){
        std::cout << "Subscribing to: " << pub_endpoint << " Filter: " << host_name << "*" << std::endl;
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

void DeploymentManager::GotControlMessage(google::protobuf::MessageLite* ml){
    auto control_message = (NodeManager::ControlMessage*)ml;

    //Gain mutex lock and append message to queue
    std::unique_lock<std::mutex> lock(notify_mutex_);
    control_message_queue_.push(control_message);
    notify_lock_condition_.notify_all();
}

void DeploymentManager::ProcessControlQueue(){
    std::queue<NodeManager::ControlMessage*> queue_;
    
    bool terminate = false;
    while(!terminate){
        {
            //Gain Mutex
            std::unique_lock<std::mutex> lock(notify_mutex_);
            
            //If we have no messages, we should wait for a signal
            if(control_message_queue_.empty()){
                //Check terminate flag
                notify_lock_condition_.wait(lock);
            }
            //Swap out the queue's and release the mutex
            control_message_queue_.swap(queue_);
        }

         //Process the queue
         while(!queue_.empty()){
            auto cm = queue_.front();

            switch(cm->type()){
                case NodeManager::ControlMessage::STARTUP:{
                    if(!deployment_){
                        deployment_ = new NodeContainer(library_path_);    
                        deployment_->Configure(cm);
                    }
                    break;
                }
                case NodeManager::ControlMessage::ACTIVATE:
                    if(deployment_){
                        deployment_->ActivateAll();
                    }
                    break;
                case NodeManager::ControlMessage::PASSIVATE:
                    if(deployment_){
                        std::cout << "Got PassivateAll Message" << std::endl;
                        deployment_->PassivateAll();
                        std::cout << "Finished PassivateAll Message" << std::endl;
                    }
                    break;
                case NodeManager::ControlMessage::TERMINATE:
                    if(deployment_){
                        std::cout << "Got TERMINATE Message" << std::endl;
                        Terminate();
                        terminate = true;
                        std::cout << "Finished TERMINATE message" << std::endl;
                    }
                    break;
                case NodeManager::ControlMessage::SET_ATTRIBUTE:
                    if(deployment_){
                        deployment_->Configure(cm);
                    }
                    break;
                default:
                    break;
            }

            queue_.pop();
        }
    }
}

NodeContainer* DeploymentManager::get_deployment(){
    return deployment_;
}

void DeploymentManager::Terminate(){
    deployment_->Teardown();
    execution_->Interrupt();

    if(control_queue_thread_){
        control_queue_thread_->join();
        delete control_queue_thread_;
        control_queue_thread_ = 0;
    }
}
