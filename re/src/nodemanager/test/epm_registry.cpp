//
// Created by mitchell on 12/11/20.
//

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
            "/home/mitchell/work/SEM/cmake-build-debug/bin/"};
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

    registry.remove_epm(epm_uuid);

}
} // namespace sem::node_manager::epm_registry::test