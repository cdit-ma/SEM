#include "epmregistryimpl.h"
#include "nodemanager.h"
#include "nodemanager_config/nodemanagerconfig.h"

#include <csignal>

std::optional<sem::node_manager::NodeManager> node_manager;
std::function<void(void)> interrupt_function;

void signal_handler(int sig)
{
    if(node_manager) {
        node_manager->shutdown();
    } else {
        std::cerr << "Signal handler called before NodeManager initialisation complete"
                  << std::endl;
    }
}

auto main(int argc, char** argv) -> int
{
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    using namespace sem::node_manager;
    try {
        // Parse our command line args, these are consumed at this point.
        auto config = NodeConfig::HandleArguments(argc, argv);
        if(config.has_value()) {
            // Save our config file immediately in case we had to generate a uuid
            //  We do this before testing for a vaild epm exe s.t. we always fail with the same uuid
            NodeConfig::SaveConfigFile(config.value());

            // Test that we can find the EPM executable before doing anything else.
            // Fail early my dudes.
            config->find_epm_executable();

            std::unique_ptr<epm_registry::EpmRegistry> epm_registry =
                std::make_unique<epm_registry::EpmRegistryImpl>(config.value());

            node_manager.emplace(config.value(), *epm_registry);

            node_manager->wait();
        }
    } catch(const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}