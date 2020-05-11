#include "nodemanager.h"
#include "nodemanagerconfig.h"
#include <boost/asio/io_service.hpp>
#include <boost/process/async_pipe.hpp>
#include <boost/process/io.hpp>
#include <boost/process/search_path.hpp>
#include <boost/program_options.hpp>
#include "environmentrequester.h"
#include <fstream>
#include "nodemanagerregistration.pb.h"
#include "requester.hpp"
#include <optional>
#include <utility>

namespace re::NodeManager {

NodeManager::NodeManager(NodeConfig config) :
    node_config_{std::move(config)},
    environment_manager_replier_{config.qpid_broker_endpoint,
                                 BuildControlTopicName(node_config_.uuid), ""},
    epm_registration_replier_{config.qpid_broker_endpoint,
                              BuildEpmRegistrationTopicName(node_config_.uuid), ""}
{
    // Test that we can find the EPM executable before doing anything else.
    // Fail early my dudes.
    FindEpmExecutable(node_config_.re_bin_path);

    epm_registration_replier_.run(
        [this](const EpmRegistrationRequest& request) { return HandleEpmRegistration(request); });

    // Start handle request listener.
    environment_manager_replier_.run([this](const EnvironmentManagerRequestType& request) {
        return HandleEpmManagementRequest(request);
    });

    std::cout << "[NodeManager] - Started with uuid: (" << node_config_.uuid << ")" << std::endl;
    // We now have all of our threads/repliers set up so we can register with the env manager.
    // Register with environment manager. Throws on failure.
    RegisterNodeManager(node_config_);
    std::cout << "[NodeManager] - Registered with environment manager on broker at:\n    ("
              << node_config_.qpid_broker_endpoint.to_string() << ")" << std::endl;
}

/// Callback function to handle any EpmManagementRequests made on topic
///  "<node_manager_uuid>_control_topic"
auto NodeManager::HandleEpmManagementRequest(const EnvironmentManagerRequestType& request)
    -> EnvironmentManagerReplyType
{
    // Parse message and start EPM if required.
    if(request.has_new_epm()) {
        try {
            auto epm_uuid = NewEpm(types::Uuid{request.new_epm().experiment_uuid()},
                                   request.new_epm().experiment_name());

            // Respond with EPM's uuid s.t. the experiment manager can send config info to the EPM's
            // config queue.
            EnvironmentManagerReplyType response;
            response.set_success(true);
            response.set_epm_uuid(epm_uuid.to_string());
            return response;

        } catch(const std::exception& ex) {
            std::cerr << "[NodeManager] - Failed to START epm on node:\n    ("
                      << node_config_.uuid.to_string() << "): " << ex.what() << std::endl;
            EnvironmentManagerReplyType response;
            response.set_success(false);
            response.set_failure_message("Failed to start new epm on node: "
                                         + node_config_.uuid.to_string()
                                         + " With message: " + ex.what());
            return response;
        }
    } else if(request.has_stop_epm()) {
        // Stopping an epm this way should be seen as a last resort.
        auto&& sub_message = request.stop_epm();
        auto epm_uuid = types::Uuid{sub_message.epm_uuid()};

        try {
            StopEpm(epm_uuid);
        } catch(const std::exception& ex) {
            std::cerr << "[NodeManager] - Failed to STOP epm on node:\n    ("
                      << node_config_.uuid.to_string() << "): " << ex.what() << std::endl;
            EnvironmentManagerReplyType reply;
            reply.set_success(false);
            reply.set_failure_message(ex.what());
            return reply;
        }
        EnvironmentManagerReplyType response;
        response.set_success(true);
        return response;
    } else if(request.has_heartbeat()) {
        EnvironmentManagerReplyType response;
        response.set_success(true);
        return response;
    } else {
        std::cerr << "Unhandled or unset oneof field in " << request.GetTypeName() << std::endl;
        EnvironmentManagerReplyType response;
        response.set_success(false);
        response.set_failure_message("Unknown failure starting new epm on node: "
                                     + node_config_.uuid.to_string());
        return response;
    }
}

auto NodeManager::Run() -> void
{
    run_handle_ = std::async(std::launch::async, &NodeManager::HeartbeatLoop, this);
    // Note, this does nothing currently
    Deregister();
}

/// Tries to register this NodeManager with the EnvironmentManager. Uses topic:
///  "nodemanager_registration_topic".
/// Throws on any sort of registration failure.
auto NodeManager::RegisterNodeManager(const NodeConfig& config) -> void
{
    NodeManagerRegistration request;

    request.mutable_registration()->set_uuid(config.uuid.to_string());
    request.mutable_registration()->set_node_ip_address(config.ip_address.to_string());
    request.mutable_registration()->set_library_path(config.lib_root_dir);
    if(config.hostname) {
        request.mutable_registration()->set_hostname(config.hostname.value());
    }

    try {
        auto requester = GetRegistrationRequester(config);

        const std::chrono::milliseconds timeout{5000};
        size_t retry_count{0};
        while(retry_count < 3) {
            try {
                auto reply = requester.request(request, timeout);
                if(!reply.success()) {
                    throw std::runtime_error("Registration with environment manager failed. "
                                             "Message from remote: "
                                             + reply.message());
                }
                return;
            } catch(const std::runtime_error& ex) {
                throw ex;
            } catch(const std::exception& ex) {
                retry_count += 1;
            }
            std::this_thread::sleep_for(timeout);
        }
    } catch(const std::exception& ex) {
        // We've failed to connect to a receiver. This probably means the env manager isn't up or we
        // have the wrong address for the qpid broker

        // TODO: This also causes the underlying qpid client to emit a cerr log message. This can be
        //  disabled following the instructions found here:
        //  https://qpid.apache.org/releases/qpid-cpp-1.39.0/messaging-api/book/ch01s15.html
        throw std::runtime_error("Failed to connect to environment manager using broker address: ("
                                 + config.qpid_broker_endpoint.to_string()
                                 + "). Check that there is an environment manager connected to "
                                   "this broker.");
    }
    throw std::runtime_error("Could not connect to environment manager on broker address: "
                             + config.qpid_broker_endpoint.to_string());
}

auto NodeManager::BuildControlTopicName(const types::Uuid& uuid) -> std::string
{
    return uuid.to_string() + "_control_topic";
}

/// Starts a new ExperimentProcessManager with the given configuration details.
/// EPM will communicate directly with EnvironmentManager(ExperimentManager) for further, experiment
///  specific, configuration details.
auto NodeManager::NewEpm(const types::Uuid& experiment_uuid, const std::string& experiment_name)
    -> types::Uuid
{
    std::cout << "[NodeManager] - Starting new epm for experiment:\n    " << experiment_name << " ("
              << experiment_uuid << ")" << std::endl;

    namespace bp = boost::process;
    auto request_uuid = types::Uuid{};
    auto epm_start_command = BuildEpmStartCommand(node_config_, experiment_uuid, request_uuid);

    auto epm_process = std::make_unique<bp::child>(epm_start_command);

    try {
        // Wait for our epm to send its registration message back to the node manager. Return its
        // uuid s.t. the environment manager knows where to find it.
        auto epm_uuid = WaitForEpmRegistrationMessage(request_uuid);
        epm_process_handles_.insert({epm_uuid, std::move(epm_process)});
        std::cout << "[NodeManager] - Successfully STARTED epm:\n    (" << epm_uuid << ")"
                  << std::endl;
        return epm_uuid;
    } catch(const std::exception& ex) {
        // If starting the EPM failed in any way, kill it to tidy up and re throw
        epm_process->terminate();
        throw;
    }
}

/// Waits on a semaphore that is notified in the epm registration replier thread that runs:
///  HandleEpmRegistration
auto NodeManager::WaitForEpmRegistrationMessage(const types::Uuid& request_uuid) -> types::Uuid
{
    // How long we should wait for the EPM to start
    // TODO: Tune this number?
    auto registration_wait_timeout = std::chrono::milliseconds(1000);

    std::unique_lock lock{epm_registration_mutex_};
    if(epm_registration_semaphore_.wait_for(lock, registration_wait_timeout, [this, request_uuid]() {
           // Check that we have a registration to process and
           // that the registration id matches.
           //  XXX: This could be a problem?
           return !epm_registrations_.empty()
                  && epm_registrations_.front().request_uuid == request_uuid;
       })) {
        auto epm_uuid = epm_registrations_.front().epm_uuid;
        epm_registrations_.pop();

        return epm_uuid;
    } else {
        throw std::runtime_error("EPM startup timed out.");
    }
}

auto NodeManager::BuildEpmRegistrationTopicName(const types::Uuid& uuid) -> std::string
{
    return uuid.to_string() + "_epm_registration";
}

/// Used as callback for the NodeManager's EPM registration listener.
auto NodeManager::HandleEpmRegistration(const EpmRegistrationRequest& request)
    -> EpmRegistrationReply
{
    // TODO: Build list of pending registration uuids. Check against that before we notify.
    auto request_uuid = types::Uuid{request.request_uuid()};
    auto epm_uuid = types::Uuid{request.epm_uuid()};
    {
        std::lock_guard lock{epm_registration_mutex_};
        epm_registrations_.push({request_uuid, epm_uuid});
    }
    epm_registration_semaphore_.notify_one();
    EpmRegistrationReply reply;
    reply.set_success(true);
    return reply;
}

/// Constructs a string used to run EPM through boost::process::child
auto NodeManager::BuildEpmStartCommand(const NodeConfig& node_config,
                                       types::Uuid experiment_uuid,
                                       types::Uuid creation_request_id) -> std::string
{
    auto epm_exe_path = FindEpmExecutable(node_config.re_bin_path);
    return epm_exe_path + " --experiment_uuid=" + experiment_uuid.to_string()
           + " --creation_request_uuid=" + creation_request_id.to_string()
           + " --qpid_broker_endpoint=" + node_config.qpid_broker_endpoint.to_string()
           + " --ip_address=" + node_config.ip_address.to_string() + " --lib_root_dir="
           + node_config.lib_root_dir + " --re_bin_dir=" + node_config.re_bin_path
           + " --registration_entity_uuid=" + node_config.uuid.to_string();
}

/// Finds EPM executable at path specified. Not using std::filesystem::path as some compilers don't
///  yet support it.
/// Throws if EPM executable is not found.
auto NodeManager::FindEpmExecutable(const std::string& re_bin_path) -> std::string
{
    namespace bp = boost::process;
    auto epm_exe_path = bp::search_path("experiment_process_manager", {re_bin_path});
    if(epm_exe_path.empty()) {
        throw std::runtime_error("Experiment process manager executable not found in path: "
                                 + re_bin_path);
    }
    return epm_exe_path.string();
}

/// Hard terminates process running EPM. Uses SIGTERM
/// Throws on EPM uuid not found.
void NodeManager::StopEpm(types::Uuid uuid)
{
    if(epm_process_handles_.count(uuid) != 1) {
        throw std::runtime_error("Could not find epm to stop.");
    }
    epm_process_handles_.at(uuid)->terminate();
    epm_process_handles_.erase(uuid);

    std::cout << "[NodeManager] - Successfully STOPPED epm:\n    (" << uuid << ")" << std::endl;
}

auto NodeManager::Deregister() -> void
{
    // TODO: implement.
}

auto NodeManager::HeartbeatLoop() -> void
{
    auto heartbeat_period = std::chrono::seconds(3);

    while(true) {
        std::unique_lock lock{heartbeat_mutex_};
        if(heartbeat_cv_.wait_for(lock, heartbeat_period,
                                  [this] { return end_heartbeat_.load(); })) {
            // If heartbeat_cv_.wait_for() returns false, we've woken with our end flag set.
            // We should stop heartbeating at this point
            break;
        } else {
            try {
                Heartbeat();
            } catch(const std::exception& ex) {
                std::cerr << "[NodeManager] - Lost connection to environment manager on:\n    ("
                          << node_config_.qpid_broker_endpoint << ")" << std::endl;
                break;
            }
        }
    }
}

auto NodeManager::Heartbeat() -> void
{
    auto requester = GetRegistrationRequester(node_config_);
    NodeManagerRegistration request;
    request.mutable_heartbeat()->set_uuid(node_config_.uuid.to_string());
    auto reply = requester.request(request, std::chrono::seconds(3));
}
auto NodeManager::Stop() -> void
{
    end_heartbeat_ = true;
    {
        std::lock_guard lock{heartbeat_mutex_};
        heartbeat_cv_.notify_all();
    }
    run_handle_.wait();
}

auto NodeManager::GetRegistrationRequester(const NodeConfig& config)
    -> network::Requester<NodeManagerRegistration, NodeManagerRegistrationReply>
{
    return network::Requester<NodeManagerRegistration, NodeManagerRegistrationReply>{
        config.qpid_broker_endpoint, node_manager_registration_topic_, ""};
}
} // namespace re::NodeManager
