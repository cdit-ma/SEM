#ifndef RE_NODEMANAGERCONFIG_H
#define RE_NODEMANAGERCONFIG_H

#include "experimentprocessmanager.h"
#include "ipv4.hpp"
#include "socketaddress.hpp"
#include "uuid.h"
#include <future>
#include <memory>

namespace re::NodeManager {

/// Node manager daemon configuration struct.
/// Should be read from node manager configuration file.
struct NodeConfig {
    // Upon adding fields, ensure << operator is updated along with arg parsing.
    sem::types::Ipv4 ip_address;
    sem::types::SocketAddress environment_manager_registration_endpoint;
    std::string lib_root_dir{};
    sem::types::Uuid uuid;
    std::optional<std::string> hostname;
    std::string re_bin_path{};

    // Config file path is not saved to config file on write
    std::string file_path;
    [[nodiscard]] static auto
    FromIstream(std::basic_istream<char>& file_contents) -> std::optional<NodeConfig>;
    static auto SaveConfigFile(const NodeConfig& config) -> void;
    [[nodiscard]] static auto HandleArguments(int argc, char** argv) -> std::optional<NodeConfig>;

private:
    [[nodiscard]] static auto ParseArguments(int argc, char** argv) -> std::optional<std::string>;
};

inline auto operator<<(std::ostream& out, const NodeConfig& config) -> std::ostream&
{
    out << "ip_address=" << config.ip_address << '\n'
        << "environment_manager_registration_endpoint="
        << config.environment_manager_registration_endpoint << '\n'
        << "library_root=" << config.lib_root_dir << '\n'
        << "re_bin_path=" << config.re_bin_path << '\n'
        << "uuid=" << config.uuid.to_string() << '\n';
    if(config.hostname) {
        out << "host_name=" << config.hostname.value() << '\n';
    }
    return out;
}

} // namespace re::NodeManager

#endif // RE_NODEMANAGERCONFIG_H
