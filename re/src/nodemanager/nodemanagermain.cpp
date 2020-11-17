#include "epmregistryimpl.h"
#include "nodemanager.h"
#include "nodemanager_config/nodemanagerconfig.h"

std::function<void(void)> interrupt_function;

void signal_handler(int sig)
{
    interrupt_function();
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
            // Test that we can find the EPM executable before doing anything else.
            // Fail early my dudes.
            config->find_epm_executable();

            // Save our config file immediately in case we had to generate a uuid
            NodeConfig::SaveConfigFile(config.value());
            std::unique_ptr<EpmRegistry> epm_registry = std::make_unique<EpmRegistryImpl>(
                config.value());

            sem::node_manager::NodeManager node_manager{config.value(), *epm_registry};

            interrupt_function = [&node_manager]() { node_manager.shutdown(); };

            node_manager.wait();
        }
    } catch(const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}