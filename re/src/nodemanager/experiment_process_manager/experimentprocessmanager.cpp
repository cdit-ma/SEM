#include "experimentprocessmanager.h"

#include <boost/program_options.hpp>
#include <memory>
#include <utility>

#include <fstream>
#include "epmstdout.pb.h"
#include "publisher.hpp"
#include "requester.hpp"

namespace re::NodeManager {



ExperimentProcessManager::ExperimentProcessManager(ExperimentProcessManagerConfig config) :
    epm_config_{std::move(config)},
{
}

/// Evil hack
auto ExperimentProcessManager::SendStdOut() -> void
{
    using MessageType = re::network::protocol::epmstdout::Message;
    re::network::Publisher<MessageType> publisher{epm_config_.qpid_broker_endpoint,
                                                  BuildEpmStdOutTopic(), ""};

    while(!stop_std_out_capture_) {
        PushStdOutMessage(publisher);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    PushStdOutMessage(publisher);
}
void ExperimentProcessManager::PushStdOutMessage(network::Publisher<StdOutMessage>& publisher)
{
    std::stringstream tmp_buffer{};
    buffer_.swap(tmp_buffer);
    auto str = tmp_buffer.str();
    if(!str.empty()) {
        StdOutMessage message;
        message.set_output(str);
        publisher.publish(message);
    }
}
/// Callback function given to epm_control_replier_. Handles requests to start/stop experiment
///  processes (currently DeploymentManagers)
auto ExperimentProcessManager::HandleEpmControl(const ControlRequest& request) -> ControlReply
{
    if(request.has_new_experiment_process()) {
        try {
            auto&& sub_message = request.new_experiment_process();
            auto deployment_manager = std::make_unique<DeploymentManager>(
                epm_config_.experiment_uuid, epm_config_.qpid_broker_endpoint,
                sub_message.experiment_name(), epm_config_.ip_address, sub_message.container_id(),
                sem::types::SocketAddress(sub_message.master_publisher_endpoint()),
                sem::types::SocketAddress(sub_message.master_registration_endpoint()),
                // This library path should look something like "lib_root_dir/<experiment_uuid>/lib
                epm_config_.lib_root_dir + "/" + sub_message.library_path());
            std::cout << "[ExperimentProcessManager] - Started new DeploymentManager." << std::endl;
            deployment_managers_.push_back(std::move(deployment_manager));

            ControlReply reply;
            reply.set_success(true);
            return reply;
        } catch(const std::exception& ex) {
            return GetErrorReply(ex.what());
        }
    } else if(request.has_stop()) {
        try {
            for(auto& deployment_manager : deployment_managers_) {
                deployment_manager->Terminate();
            }
            StopStdOutCapture();
            ControlReply reply;
            reply.set_success(true);
            {
                std::unique_lock lock(running_mutex_);
                finished_ = true;
                running_cv_.notify_one();
            }
            return reply;
        } catch(const std::exception& ex) {
            return GetErrorReply(ex.what());
        }
    } else {
        return GetErrorReply("Unhandled oneof type in ControlRequest");
    }
}

/// Send registration request to our node manager (if this is started as a docker container, we're
///  actually registering with the environment manager).
auto ExperimentProcessManager::RegisterWithNodeManager(const sem::types::Uuid& request_id) -> void
{
    network::Requester<RegistrationRequest, RegistrationReply> node_manager_registration_requester{
        epm_config_.qpid_broker_endpoint, BuildEpmRegistrationTopic(), ""};

    RegistrationRequest request;
    request.set_epm_uuid(epm_config_.epm_uuid.to_string());
    request.set_request_uuid(request_id.to_string());
    try {
        // TODO: Tune this timeout
        auto epm_registration_timeout = std::chrono::seconds(1);
        auto reply = node_manager_registration_requester.request(request, epm_registration_timeout);
        if(!reply.success()) {
            throw std::runtime_error(reply.error_message());
        }
    } catch(const std::runtime_error& ex) {
        throw ex;
    } catch(const std::exception& ex) {
        throw ex;
    }
}


void ExperimentProcessManager::StopStdOutCapture()
{
    stop_std_out_capture_ = true;
    std_out_sender_future_.wait();

    // Restore stdout. See docs for std::cout.rdbuf()
    std::cout.rdbuf(original_std_out_stream_pointer_);
}

} // namespace re::NodeManager
