#include "nodemanagerregistry.h"
#include <network/requester.hpp>
#include <types/socketaddress.hpp>
namespace re::EnvironmentManager {
NodeManagerRegistry::NodeManagerRegistry(types::SocketAddress broker_address) :
    broker_address_{broker_address}, replier_{broker_address_, nodemanager_registration_topic_, ""}
{
    replier_.run([this](const NodeManagerRegistrationRequestType& request) {
        return HandleRequest(request);
    });

    heartbeat_loop_handle_ = std::async(std::launch::async, &NodeManagerRegistry::HeartbeatLoop,
                                        this);
}

auto NodeManagerRegistry::HeartbeatLoop() -> void
{
    auto heartbeat_period = std::chrono::seconds(3);
    while(true) {
        std::vector<types::Uuid> to_remove{};
        std::unique_lock lock{heartbeat_mutex_};
        if(heartbeat_cv_.wait_for(lock, heartbeat_period,
                                  [this] { return end_heartbeat_.load(); })) {
            // Should deregister first?
            break;
        } else {
            std::lock_guard nm_lock{node_managers_lock_};
            auto now = std::chrono::steady_clock::now();
            for(const auto& node_manager_p : node_managers_) {
                auto uuid = node_manager_p.first;
                if(now - node_manager_p.second.last_heartbeat > heartbeat_period * 2) {
                    // If difference between now and last_heartbeat is GT heartbeat_period, this NM
                    //  has timed out. Add to remove list.
                    to_remove.push_back(uuid);
                }
            }
        }
        for(const auto& uuid : to_remove) {
            std::cout << "[NodeManagerRegistry] - De-registering node: \n"
                      << "    " << node_managers_[uuid].hostname << " (" << uuid << ")"
                      << std::endl;
            node_managers_.erase(uuid);
        }
    }
}

auto NodeManagerRegistry::HandleRequest(const NodeManagerRegistrationRequestType& request)
    -> NodeManagerRegistrationReplyType
{
    if(request.has_registration()) {
        const auto& sub_message = request.registration();
        assert(!sub_message.uuid().empty());
        auto uuid = types::Uuid{sub_message.uuid()};
        NodeManagerRegistrationReplyType reply;
        std::lock_guard lock{node_managers_lock_};
        if(!node_managers_.count(uuid)) {
            // We don't have a node of this particular uuid registered, register it
            node_managers_.emplace(uuid, NodeManager{uuid, sub_message.hostname(),
                                                     std::chrono::steady_clock::now()});
            std::cout << "[NodeManagerRegistry] - Registered node: \n"
                      << "    " << sub_message.hostname() << " (" << sub_message.uuid() << ")"
                      << std::endl;

            reply.set_success(true);
            return reply;
        } else {
            // We already have this node, reply to that effect
            std::cerr << "[NodeManagerRegistry] - Attempted to re-register node manager with uuid: "
                      << sub_message.uuid() << std::endl;
            reply.set_success(false);
            reply.set_message("Already have a node manager registered with this uuid: "
                              + sub_message.uuid());
            return reply;
        }
    } else if(request.has_heartbeat()) {
        const auto& sub_message = request.heartbeat();
        assert(!sub_message.uuid().empty());
        auto uuid = types::Uuid{sub_message.uuid()};
        UpdateLastHeartbeatTime(uuid);
        NodeManagerRegistrationReplyType reply;
        return reply;
    } else {
        assert(false);
    }
    assert(false);
}

auto NodeManagerRegistry::stop() -> void
{
    end_heartbeat_ = true;
    replier_.stop();
    heartbeat_loop_handle_.get();
}

NodeManagerRegistry::~NodeManagerRegistry()
{
    stop();
}

auto NodeManagerRegistry::GetNodeManagerUuid(const std::string& hostname) -> types::Uuid
{
    std::lock_guard lock{node_managers_lock_};
    auto it = std::find_if(std::begin(node_managers_), std::end(node_managers_),
                           [hostname](auto&& pair) { return pair.second.hostname == hostname; });
    if(it == std::end(node_managers_)) {
        throw std::out_of_range("Node manager not found on hostname: " + hostname);
    }
    return it->first;
}

auto NodeManagerRegistry::NewEpm(types::Uuid node_uuid,
                                 types::Uuid experiment_uuid,
                                 const std::string& experiment_name) -> types::Uuid
{
    {
        std::lock_guard lock{node_managers_lock_};
        if(node_managers_.count(node_uuid) == 0) {
            throw std::runtime_error("Failed to register new epm: No NodeManager with uuid: ("
                                     + node_uuid.to_string() + ")");
        }
    }

    network::Requester<NodeManagerControlRequestType, NodeManagerControlReplyType> requester{
        broker_address_, BuildNodeManagerControlTopicName(node_uuid), ""};
    NodeManagerControlRequestType request;
    request.mutable_new_epm()->set_experiment_name(experiment_name);
    request.mutable_new_epm()->set_experiment_uuid(experiment_uuid.to_string());

    // TODO: Set realistic timeout.
    try {
        auto reply = requester.request(request, types::NeverTimeout());
        if(reply.success()) {
            auto new_epm_uuid = types::Uuid(reply.epm_uuid());
            std::cout << "[NodeManagerRegistry] - Launched EPM: (" << new_epm_uuid << ")\n"
                      << "    For experiment: " << experiment_name << " (" << experiment_uuid << ")"
                      << std::endl;
            epm_location_map_.insert({new_epm_uuid, node_uuid});
            return types::Uuid{reply.epm_uuid()};
        } else {
            throw std::runtime_error("Failed to register new epm: " + reply.failure_message());
        }
    } catch(const decltype(requester)::exception& ex) {
        std::lock_guard lock{node_managers_lock_};
        node_managers_.erase(node_uuid);
        throw std::runtime_error("Failed to connect to NodeManager's epm control queue. "
                                 "Deregistering NodeManager.");
    }
}

auto NodeManagerRegistry::StartExperimentProcess(types::Uuid epm_uuid,
                                                 const std::string& container_id,
                                                 types::SocketAddress manager_publisher_endpoint,
                                                 types::SocketAddress manager_registration_endpoint,
                                                 const std::string& library_path) -> void
{
    network::Requester<EpmControl, EpmControlReply> requester{broker_address_,
                                                              BuildEpmControlTopicName(epm_uuid),
                                                              ""};
    EpmControl request;
    request.mutable_new_experiment_process()->set_container_id(container_id);
    request.mutable_new_experiment_process()->set_master_publisher_endpoint(
        manager_publisher_endpoint.to_string());
    request.mutable_new_experiment_process()->set_master_registration_endpoint(
        manager_registration_endpoint.to_string());
    request.mutable_new_experiment_process()->set_library_path(library_path);

    try {
        // TODO: Set realistic timeout.
        auto reply = requester.request(request, types::NeverTimeout());

        if(reply.success()) {
            std::cout << "[NodeManagerRegistry] - Launched experiment process for container: \""
                      << container_id << "\"\n"
                      << "    On EPM: (" << epm_uuid << ")" << std::endl;
        } else {
            throw std::runtime_error("Failed to launch new experiment process: "
                                     + reply.error_message());
        }
    } catch(const std::exception& ex) {
        throw;
    }
}

auto NodeManagerRegistry::BuildNodeManagerControlTopicName(types::Uuid node_uuid) -> std::string
{
    return node_uuid.to_string() + "_control_topic";
}

auto NodeManagerRegistry::BuildEpmControlTopicName(types::Uuid epm_uuid) -> std::string
{
    return epm_uuid.to_string() + "_epm_control";
}

/**
 * Send kill messages to all epms started for given epm uuid.
 * @param experiment_uuid
 */
auto NodeManagerRegistry::StopEpm(types::Uuid epm_uuid) -> void
{
    network::Requester<EpmControl, EpmControlReply> epm_requester{
        broker_address_, BuildEpmControlTopicName(epm_uuid), ""};
    EpmControl stop_request;
    stop_request.mutable_stop();
    auto epm_reply = epm_requester.request(stop_request, types::NeverTimeout());
    if(!epm_reply.success()) {
        std::cerr << "[NodeManagerRegistry] - Failed to stop EPM:\n    (" << epm_uuid << ")"
                  << std::endl;
    }
}

auto NodeManagerRegistry::KillBareMetalEpm(types::Uuid epm_uuid) -> void
{
    if(epm_location_map_.count(epm_uuid) == 0) {
        throw std::runtime_error("No epm of uuid: " + epm_uuid.to_string());
    }
    auto node_uuid = epm_location_map_.at(epm_uuid);
    network::Requester<NodeManagerControlRequestType, NodeManagerControlReplyType> requester{
        broker_address_, BuildNodeManagerControlTopicName(node_uuid), ""};
    NodeManagerControlRequestType request;
    request.mutable_stop_epm()->set_epm_uuid(epm_uuid.to_string());

    // TODO: Set realistic timeout.
    auto reply = requester.request(request, std::chrono::seconds(5));

    if(!reply.success()) {
        std::cerr << "[NodeManagerRegistry] - Failed to stop EPM:\n    (" << epm_uuid
                  << ")\n    On node: " << node_managers_.at(node_uuid).hostname << " ("
                  << node_uuid << ")" << std::endl;
    }
}

void NodeManagerRegistry::UpdateLastHeartbeatTime(types::Uuid uuid)
{
    std::lock_guard lock{node_managers_lock_};
    node_managers_.at(uuid).last_heartbeat = std::chrono::steady_clock::now();
}
} // namespace re::EnvironmentManager
