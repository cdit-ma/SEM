#include "nodemanager.h"
#include "nodemanagerconfig.h"

auto main(int argc, char** argv) -> int
{
    using namespace re::node_manager;
    try {
        // Parse our command line args, these are consumed at this point.
        auto config = NodeConfig::HandleArguments(argc, argv);
        if(config.has_value()) {
            // Save our config file immediately in case we had to generate a uuid
            NodeConfig::SaveConfigFile(config.value());
            re::node_manager::NodeManager node_manager{config.value()};
            node_manager.wait();
        }
    } catch(const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}