#ifndef ENVIRONMENT_MANAGER_PORTTRACKER_H
#define ENVIRONMENT_MANAGER_PORTTRACKER_H

#include <mutex>
#include "ipv4.hpp"
#include "socketaddress.hpp"
#include "uniquequeue.hpp"
#include <unordered_map>
#include <utility>

namespace re::EnvironmentManager {

// REVIEW (Mitch): Move this to a util library. This could be a fairly generic wrapper around a
//  unique_queue, unsure what to name.

/// Simple wrapper around a unique_queue. Facilitates tracking of available ports on a particular
///  ip_address
class EndpointTracker {
public:
    EndpointTracker(std::string name,
                    sem::types::Ipv4 ip_address,
                    sem::types::unique_queue<uint16_t> port_set);

    [[nodiscard]] auto empty() const -> bool;

    auto GetEndpoint() -> sem::types::SocketAddress;

    auto FreeEndpoint(sem::types::SocketAddress endpoint) -> void;
    auto GetName() -> std::string;
    auto SetName(const std::string& name) -> void;

    auto GetIp() -> sem::types::Ipv4;

private:
    std::string name_;
    sem::types::Ipv4 ip_;
    std::mutex port_mutex_;
    sem::types::unique_queue<uint16_t> available_ports_;
};
}; // namespace re::EnvironmentManager

#endif // ENVIRONMENT_MANAGER_PORTTRACKER_H