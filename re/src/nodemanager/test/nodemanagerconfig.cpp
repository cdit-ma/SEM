#include "nodemanager/nodemanagerconfig.h"
#include "gtest/gtest.h"

namespace re::NodeManager::test {
auto string_to_lines(const std::string& in) -> std::vector<std::string>
{
    std::vector<std::string> out;
    std::string token;
    std::istringstream input_stream{in};
    while(std::getline(input_stream, token, '\n')) {
        out.push_back(token);
    }
    return out;
}

/// Test that we can parse a correctly formatted input stream
TEST(re_nodemanager_config, test_parse)
{
    std::istringstream input{R"--(ip_address=192.168.111.230
uuid=47503448-982e-4244-8e9a-8f7054e1ef66
environment_manager_registration_endpoint=192.168.111.230:5672
re_bin_path=/home/cdit-ma/re/bin
library_root=/home/cdit-ma/re)--"};
    std::istream input_stream{input.rdbuf()};
    // Have to specify global namespaced NodeManager
    auto config = NodeManager::NodeConfig::FromIstream(input_stream);
    ASSERT_EQ(config.value().lib_root_dir, "/home/cdit-ma/re");
    ASSERT_EQ(config.value().environment_manager_registration_endpoint, types::SocketAddress("192.168."
                                                                        "111.230:"
                                                                        "5672"));
    ASSERT_EQ(config.value().ip_address, types::Ipv4("192.168.111.230"));
    ASSERT_EQ(config.value().uuid, types::Uuid{"47503448-982e-4244-8e9a-8f7054e1ef66"});
}

/// Test that we can parse a hostname
TEST(re_nodemanager_config, valid_optional_hostname)
{
    std::istringstream input{R"--(ip_address=192.168.111.230
uuid=47503448-982e-4244-8e9a-8f7054e1ef66
environment_manager_registration_endpoint=192.168.111.230:5672
library_root=/home/cdit-ma/re
re_bin_path=/home/cdit-ma/re/bin
host_name=test_hostname)--"};
    std::istream input_stream{input.rdbuf()};
    // Have to specify global namespaced NodeManager
    auto config = NodeManager::NodeConfig::FromIstream(input_stream);
    ASSERT_EQ(config.value().lib_root_dir, "/home/cdit-ma/re");
    ASSERT_EQ(config.value().environment_manager_registration_endpoint, types::SocketAddress("192.168."
                                                                        "111.230:"
                                                                        "5672"));
    ASSERT_EQ(config.value().ip_address, types::Ipv4("192.168.111.230"));
    ASSERT_EQ(config.value().uuid, types::Uuid{"47503448-982e-4244-8e9a-8f7054e1ef66"});
    ASSERT_EQ(config.value().hostname.value(), std::string("test_hostname"));
}

/// Test that we can parse a hostname
TEST(re_nodemanager_config, optional_hostname)
{
    std::istringstream input{R"--(ip_address=192.168.111.230
uuid=47503448-982e-4244-8e9a-8f7054e1ef66
environment_manager_registration_endpoint=192.168.111.230:5672
re_bin_path=/home/cdit-ma/re/bin
library_root=/home/cdit-ma/re)--"};
    std::istream input_stream{input.rdbuf()};
    // Have to specify global namespaced NodeManager
    auto config = NodeManager::NodeConfig::FromIstream(input_stream);
    ASSERT_EQ(config.value().lib_root_dir, "/home/cdit-ma/re");
    ASSERT_EQ(config.value().environment_manager_registration_endpoint, types::SocketAddress("192.168."
                                                                        "111.230:"
                                                                        "5672"));
    ASSERT_EQ(config.value().ip_address, types::Ipv4("192.168.111.230"));
    ASSERT_EQ(config.value().uuid, types::Uuid{"47503448-982e-4244-8e9a-8f7054e1ef66"});
    ASSERT_FALSE(config.value().hostname.has_value());
}

/// Test that we're correctly setting a random uuid if we aren't given one in our input stream
TEST(re_nodemanager_config, unset_uuid)
{
    std::istringstream input{R"--(ip_address=192.168.111.230
environment_manager_registration_endpoint=192.168.111.230:5672
re_bin_path=/home/cdit-ma/re/bin
library_root=/home/cdit-ma/re)--"};
    std::istream input_stream{input.rdbuf()};
    // Have to specify global namespaced NodeManager
    auto config = NodeManager::NodeConfig::FromIstream(input_stream);
    ASSERT_EQ(config.value().lib_root_dir, "/home/cdit-ma/re");
    ASSERT_EQ(config.value().environment_manager_registration_endpoint, types::SocketAddress("192.168."
                                                                        "111.230:"
                                                                        "5672"));
    ASSERT_EQ(config.value().ip_address, types::Ipv4("192.168.111.230"));

    // re::uuid and nodemanager::nodeconfig class invariants enforce that it's a valid uuid and
    // config, check that there's something in our to_string to placate paranoia
    types::Uuid uuid = config.value().uuid;
    ASSERT_NE(uuid.to_string().size(), 0);
}

/// Test that istream -> nodemanagerconfig -> ostream works correctly. This is used to print a
/// mutated config back out to a file.
TEST(re_nodemanager_config, ostream_test)
{
    std::istringstream input{R"--(ip_address=192.168.111.230
environment_manager_registration_endpoint=192.168.111.230:5672
uuid=47503448-982e-4244-8e9a-8f7054e1ef66
re_bin_path=/home/cdit-ma/re/bin
library_root=/home/cdit-ma/re)--"};
    std::istream input_stream{input.rdbuf()};
    // Have to specify global namespaced NodeManager
    auto config = NodeManager::NodeConfig::FromIstream(input_stream);

    std::stringstream stream;
    stream << *config;
    // Ensure that we've output all of our options.
    auto lines = string_to_lines(stream.str());
    ASSERT_EQ(lines.size(), 5);

    // Iterate over lines in our "file" and make sure all of our fields have been output correctly.
    for(const auto& line : lines) {
        if(line.find("ip_address") != std::string::npos) {
            ASSERT_EQ(line, "ip_address=192.168.111.230");
            continue;
        } else if(line.find("environment_manager_registration_endpoint") != std::string::npos) {
            ASSERT_EQ(line, "environment_manager_registration_endpoint=192.168.111.230:5672");
            continue;
        } else if(line.find("library_root") != std::string::npos) {
            ASSERT_EQ(line, "library_root=/home/cdit-ma/re");
            continue;
        } else if(line.find("uuid") != std::string::npos) {
            ASSERT_EQ(line, "uuid=47503448-982e-4244-8e9a-8f7054e1ef66");
            continue;
        } else if(line.find("re_bin_path") != std::string::npos) {
            ASSERT_EQ(line, "re_bin_path=/home/cdit-ma/re/bin");
            continue;
        } else {
            FAIL();
        }
    }
}
} // namespace re::NodeManager::test