#include "endpointtracker.h"

#include <utility>
namespace re::EnvironmentManager {

auto EndpointTracker::empty() const -> bool
{
    return available_ports_.empty();
}

types::SocketAddress EndpointTracker::GetEndpoint()
{
    std::unique_lock<std::mutex> lock(port_mutex_);
    if(available_ports_.empty()) {
        throw std::out_of_range("Out of ports on ip address: " + ip_.to_string());
    }
    auto port = available_ports_.front();
    available_ports_.pop();

    return types::SocketAddress(ip_, port);
}

auto EndpointTracker::FreeEndpoint(types::SocketAddress endpoint) -> void
{
    std::unique_lock<std::mutex> lock(port_mutex_);
    available_ports_.push(endpoint.port());
}

std::string EndpointTracker::GetName()
{
    return name_;
}

void EndpointTracker::SetName(const std::string& name)
{
    name_ = name;
}

auto EndpointTracker::GetIp() -> types::Ipv4
{
    return ip_;
}

EndpointTracker::EndpointTracker(std::string  name,
                                 types::Ipv4 ip_address,
                                 types::unique_queue<uint16_t> port_set) :
    ip_{ip_address}, available_ports_{std::move(port_set)}, name_{std::move(name)}
{
}
} // namespace re::EnvironmentManager