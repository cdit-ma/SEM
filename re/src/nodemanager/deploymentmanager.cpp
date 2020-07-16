#include "deploymentmanager.h"

#include <iostream>
#include <memory>

#include "execution.hpp"

#include "environmentrequester.h"
#include "helper.h"
#include "protoreceiver.h"

DeploymentManager::DeploymentManager(Execution& execution,
                                     const std::string& experiment_name,
                                     const std::string& ip_address,
                                     const std::string& container_id,
                                     const std::string& master_publisher_endpoint,
                                     const std::string& master_registration_endpoint,
                                     const std::string& library_path,
                                     bool is_master_node) :
    execution_(execution),
    experiment_name_(experiment_name),
    ip_address_(ip_address),
    container_id_(container_id),
    library_path_(library_path),
    is_master_node_(is_master_node)
{
    // REVIEW (Mitch): Lambda here?
    execution_.AddTerminateCallback(std::bind(&DeploymentManager::Terminate, this));

    // Construct a live receiever
    proto_receiever_ = std::unique_ptr<zmq::ProtoReceiver>(new zmq::ProtoReceiver());
    proto_requester_ = std::unique_ptr<zmq::ProtoRequester>(
        new zmq::ProtoRequester(master_registration_endpoint));

    // REVIEW (Mitch): Convention reversal, zmq callbacks are genrally called Handle*
    //  in this case the Handle function is called in our "process" thread once we notify our
    //  condition variable
    // Subscribe to NodeManager::ControlMessage Types
    proto_receiever_->RegisterProtoCallback<NodeManager::ControlMessage>(
        std::bind(&DeploymentManager::GotExperimentUpdate, this, std::placeholders::_1));
    proto_receiever_->Connect(master_publisher_endpoint);
    proto_receiever_->Filter("*");

    // Construct a thread to process the control queue
    control_queue_future_ = std::async(std::launch::async, &DeploymentManager::ProcessControlQueue,
                                       this);

    try {
        RequestDeployment();
    } catch(const std::exception& ex) {
        Terminate();
        throw;
    }
}

std::unique_ptr<NodeManager::SlaveId> DeploymentManager::GetSlaveID() const
{
    using namespace NodeManager;
    auto slave = std::unique_ptr<SlaveId>(new SlaveId());
    slave->set_ip_address(ip_address_);
    slave->set_container_id(container_id_);
    return slave;
}

void DeploymentManager::RequestDeployment()
{
    using namespace NodeManager;

    std::unique_ptr<SlaveStartupReply> startup_reply;

    {
        // Get Experiment From Node Manager Master
        SlaveStartupRequest request;
        request.set_allocated_id(GetSlaveID().release());

        // Retry three times
        int retry_count = 0;
        while(true) {
            try {
                auto reply_future =
                    proto_requester_->SendRequest<SlaveStartupRequest, SlaveStartupReply>(
                        "SlaveStartup", request, 5000);
                startup_reply = reply_future.get();
                break;
            } catch(const zmq::TimeoutException& ex) {
                retry_count++;
            } catch(const zmq::RMIException& ex) {
                throw;
            } catch(const std::exception& ex) {
                retry_count++;
            }
            if(retry_count > 3) {
                throw std::runtime_error("DeploymentManager::RequestDeployment failed after three "
                                         "attempts.");
            }
        }
    }

    // Tell Node Manager Master that We are Configured
    {
        SlaveConfiguredRequest request;
        request.set_allocated_id(GetSlaveID().release());

        try {
            HandleContainerUpdate(startup_reply->host_name(), startup_reply->configuration());
            request.set_success(true);
        } catch(const std::exception& ex) {
            request.set_success(false);
            request.add_error_messages(ex.what());
        }

        // Start our heartbeater
        heartbeater_ = std::unique_ptr<SlaveHeartbeater>(
            new SlaveHeartbeater(*proto_requester_, ip_address_, container_id_));
        heartbeater_->SetTimeoutCallback(std::bind(&Execution::Interrupt, &execution_));

        auto reply_future =
            proto_requester_->SendRequest<SlaveConfiguredRequest, SlaveConfiguredReply>(
                "SlaveConfigured", request, 1000);

        reply_future.get();
    }
}

void DeploymentManager::HandleExperimentUpdate(const NodeManager::ControlMessage& control_message)
{
    for(const auto& node : control_message.nodes()) {
        if(node.ip_address() == ip_address_) {
            for(const auto& container : node.containers()) {
                if(container.info().id() == container_id_) {
                    HandleContainerUpdate(node.info().name(), container);
                }
            }
        }
    }
}

void DeploymentManager::HandleContainerUpdate(const std::string& host_name,
                                              const NodeManager::Container& container)
{
    std::lock_guard<std::mutex> lock(container_mutex_);
    const auto& id = container.info().id();

    if(deployment_containers_.count(id)) {
        deployment_containers_.at(id)->Configure(container);
    } else {
        deployment_containers_.emplace(id, std::unique_ptr<DeploymentContainer>(
                                               new DeploymentContainer(experiment_name_, host_name,
                                                                       library_path_, container)));
    }
}

void DeploymentManager::InteruptControlQueue()
{
    std::unique_lock<std::mutex> lock(queue_mutex_);
    terminate_ = true;
    queue_condition_.notify_all();
}

void DeploymentManager::Terminate()
{
    InteruptControlQueue();

    if(control_queue_future_.valid()) {
        control_queue_future_.get();
    }
}

DeploymentManager::~DeploymentManager()
{
    try {
        Terminate();
    } catch (const std::exception& ex) {
        std::cerr << "! Caught exception in DeploymentManager destructor: " << ex.what() << std::endl;
    } catch (...) {
        std::cerr << "! Caught unhandled exception in DeploymentManager destructor" << std::endl;
    }
}

void DeploymentManager::GotExperimentUpdate(const NodeManager::ControlMessage& control_message)
{
    // Gain mutex lock and append message to queue
    std::unique_lock<std::mutex> lock(queue_mutex_);
    // Have to copy the message onto our queue
    control_message_queue_.emplace(std::unique_ptr<NodeManager::ControlMessage>(
        new NodeManager::ControlMessage(control_message)));
    queue_condition_.notify_all();
}

void DeploymentManager::ProcessControlQueue()
{
    std::queue<std::unique_ptr<NodeManager::ControlMessage>> queue;

    while(true) {
        // REVIEW (Mitch): Should we break this construct out into a utility function/class?
        //  This pattern is used very frequently:
        /*
         * {
         *      gain mutex
         *      wait on condition variable with predicate
         *      execute locked behaviour
         * }
         */
        {
            // Gain Mutex
            std::unique_lock<std::mutex> lock(queue_mutex_);
            queue_condition_.wait(lock,
                                  [this] { return terminate_ || !control_message_queue_.empty(); });
            // Swap out the queue's and release the mutex
            control_message_queue_.swap(queue);

            if(terminate_ && queue.empty()) {
                break;
            }
        }

        // Process the queue
        while(!queue.empty()) {
            auto control_message = std::move(queue.front());
            queue.pop();
            auto start = std::chrono::steady_clock::now();

            auto type = control_message->type();

            switch(type) {
                // REVIEW (Mitch): Why is Configure different to Activate, passivate, terminate?
                case NodeManager::ControlMessage::CONFIGURE:
                case NodeManager::ControlMessage::STARTUP:
                case NodeManager::ControlMessage::SET_ATTRIBUTE: {
                    HandleExperimentUpdate(*control_message);
                    break;
                }
                case NodeManager::ControlMessage::ACTIVATE: {
                    std::lock_guard<std::mutex> lock(container_mutex_);

                    for(const auto& c : deployment_containers_) {
                        c.second->Activate();
                    }
                    break;
                }
                case NodeManager::ControlMessage::PASSIVATE: {
                    std::lock_guard<std::mutex> lock(container_mutex_);
                    for(const auto& c : deployment_containers_) {
                        c.second->Passivate();
                    }
                    break;
                }
                case NodeManager::ControlMessage::TERMINATE: {
                    {
                        std::lock_guard<std::mutex> lock(container_mutex_);

                        for(const auto& c : deployment_containers_) {
                            c.second->Terminate();
                        }
                        deployment_containers_.clear();
                    }
                    InteruptControlQueue();
                    break;
                }
                // REVIEW (Mitch): All usages of switch case default should be reviewed.
                default:
                    break;
            }
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - start);
            std::cout << "* " << NodeManager::ControlMessage_Type_Name(type)
                      << " Deployment took: " << ms.count() << " ms" << std::endl;
        }
    }

    // Tell Node Manager Master that We are Terminated
    {
        using namespace NodeManager;
        SlaveTerminatedRequest request;
        request.set_allocated_id(GetSlaveID().release());
        auto reply_future =
            proto_requester_->SendRequest<SlaveTerminatedRequest, SlaveTerminatedReply>(
                "SlaveTerminated", request, 5000);
        reply_future.get();
        // Destroy our heartbeater
        heartbeater_.reset();
    }

    if(!is_master_node_) {
        execution_.Interrupt();
    }
}
