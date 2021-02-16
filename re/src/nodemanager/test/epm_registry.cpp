//
// Created by mitchell on 12/11/20.
//

#include "build_time_variables.hpp"
#include "epmregistryimpl.h"
#include "gtest/gtest.h"

namespace sem::node_manager::epm_registry::test {

auto get_node_config() -> NodeConfig
{
    return {types::Ipv4::localhost(),
            types::Ipv4::localhost(),
            types::SocketAddress::unspecified(),
            "test_lib_root_dir",
            types::Uuid{},
            "test_hostname",
            std::string(sem::node_manager::test::build_time::get_binary_directory_path())};
}
TEST(epm_registry, constuctor_test)
{
    NodeConfig config{get_node_config()};
    EpmRegistryImpl registry{config};
}

TEST(epm_registry, registration_test)
{
    NodeConfig config{get_node_config()};
    EpmRegistryImpl registry{config};
    auto experiment_uuid = types::Uuid{};
    auto container_uuid = types::Uuid{};
    auto epm_uuid = registry.start_epm(experiment_uuid, container_uuid);

    auto epm_info = registry.get_epm_info(epm_uuid);
    ASSERT_EQ(types::Ipv4::localhost(), epm_info.data_ip_address);
    ASSERT_EQ(types::Ipv4::localhost(), epm_info.control_endpoint.ip());

    registry.remove_epm(epm_uuid);
}
} // namespace sem::node_manager::epm_registry::test