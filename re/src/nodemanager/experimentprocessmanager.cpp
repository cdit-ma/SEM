#include "experimentprocessmanager.h"

#include <boost/program_options.hpp>
#include <memory>
#include <utility>

#include <fstream>
#include "epmstdout.pb.h"
#include "publisher.hpp"
#include "requester.hpp"

namespace re::NodeManager {

/**
 * Accepts the following command line args:
 * experiment_uuid[e] experiment uuid
 * creation_request_uuid[r] uuid of the request used to start this EPM
 * qpid_broker_endpoint[q] qpid broker endpoint
 * ip_addres[i] ip address
 * lib_root_dir[l]
 * re_bin_dir[b]
 * registration_entity_uuid[u]
 */
auto ExperimentProcessManager::HandleArguments(int argc, char** argv)
    -> ExperimentProcessManagerConfig
{
    namespace po = boost::program_options;
    po::options_description command_line_options("Node manager options");
    std::string experiment_uuid_string{};
    std::string creation_request_uuid_string{};
    std::string qpid_broker_endpoint_string{};
    std::string ip_address_string{};
    std::string lib_root_dir{};
    std::string re_bin_dir{};
    std::string registration_entity_uuid_string{};

    command_line_options.add_options()("experiment_uuid,e",
                                       po::value<std::string>(&experiment_uuid_string)->required(),
                                       "Experiment uuid");

    command_line_options.add_options()(
        "creation_request_uuid,r",
        po::value<std::string>(&creation_request_uuid_string)->required(), "Creation request uuid");

    command_line_options.add_options()(
        "qpid_broker_endpoint,q", po::value<std::string>(&qpid_broker_endpoint_string)->required(),
        "qpid broker endpoint");

    command_line_options.add_options()("ip_address,i",
                                       po::value<std::string>(&ip_address_string)->required(),
                                       "ip address");

    command_line_options.add_options()("lib_root_dir,l",
                                       po::value<std::string>(&lib_root_dir)->required(),
                                       "component library root directory path");

    command_line_options.add_options()("re_bin_dir,b",
                                       po::value<std::string>(&re_bin_dir)->required(),
                                       "re binary directory path");

    command_line_options.add_options()(
        "registration_entity_uuid,u",
        po::value<std::string>(&registration_entity_uuid_string)->required(),
        "re binary directory path");

    command_line_options.add_options()("help,h", "help");

    po::variables_map parsed_options;
    po::store(po::parse_command_line(argc, argv, command_line_options), parsed_options);
    po::notify(parsed_options);

    try {
        ExperimentProcessManagerConfig config{types::Uuid{},
                                              types::Uuid{experiment_uuid_string},
                                              types::Uuid{creation_request_uuid_string},
                                              types::Uuid{registration_entity_uuid_string},
                                              types::Ipv4::from_string(ip_address_string),
                                              types::SocketAddress::from_string(
                                                  qpid_broker_endpoint_string),
                                              lib_root_dir,
                                              re_bin_dir};
        return config;
    } catch(const std::exception& ex) {
        std::cout << command_line_options << std::endl;
        throw std::invalid_argument("Failed to parse EPM options.");
    }
}

ExperimentProcessManager::ExperimentProcessManager(ExperimentProcessManagerConfig config) :
    epm_config_{std::move(config)},
    epm_control_replier_{epm_config_.qpid_broker_endpoint, BuildEpmControlTopic(), ""}
{
    // Start experiment management listener
    epm_control_replier_.run(
        [this](const ControlRequest& request) { return HandleEpmControl(request); });

    // Tell node manager that we've successfully started, throws if registration fails.
    RegisterWithNodeManager(epm_config_.creation_request_uuid);

    original_std_out_stream_pointer_ = std::cout.rdbuf(buffer_.rdbuf());
    std_out_sender_future_ = std::async(std::launch::async, &ExperimentProcessManager::SendStdOut,
                                        this);
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
                types::SocketAddress::from_string(sub_message.master_publisher_endpoint()),
                types::SocketAddress::from_string(sub_message.master_registration_endpoint()),
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
auto ExperimentProcessManager::GetErrorReply(const std::string& error_message) -> ControlReply
{
    ControlReply reply;
    reply.set_success(false);
    reply.set_error_message(error_message);
    return reply;
}

/// Send registration request to our node manager (if this is started as a docker container, we're
///  actually registering with the environment manager).
auto ExperimentProcessManager::RegisterWithNodeManager(const types::Uuid& request_id) -> void
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

auto ExperimentProcessManager::BuildEpmControlTopic() -> std::string
{
    return epm_config_.epm_uuid.to_string() + "_epm_control";
}

auto ExperimentProcessManager::BuildEpmRegistrationTopic() -> std::string
{
    return epm_config_.registration_entity_uuid.to_string() + "_epm_registration";
}

auto ExperimentProcessManager::BuildEpmStdOutTopic() -> std::string
{
    return epm_config_.experiment_uuid.to_string() + "_epm_std_out";
}
void ExperimentProcessManager::StopStdOutCapture()
{
    stop_std_out_capture_ = true;
    std_out_sender_future_.wait();

    // Restore stdout. See docs for std::cout.rdbuf()
    std::cout.rdbuf(original_std_out_stream_pointer_);
}

auto ExperimentProcessManager::Start() -> void
{
    {
        std::unique_lock lock(running_mutex_);
        running_cv_.wait(lock, [this] { return finished_; });
    }
}
} // namespace re::NodeManager
