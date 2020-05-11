#include "nodemanager.h"
#include "nodemanagerconfig.h"
bool running = true;
void signal_handler(int sig)
{
    running = false;
}
auto main(int argc, char** argv) -> int
{
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    try {
        // Parse our command line args, these are consumed at this point.
        auto config = re::NodeManager::NodeConfig::HandleArguments(argc, argv);
        if(config.has_value()) {
            // Save our config file immediately in case we had to generate a uuid
            re::NodeManager::NodeConfig::SaveConfigFile(config.value());
            re::NodeManager::NodeManager node_manager{config.value()};
            node_manager.Run();
            // TODO: This does not stop running if we lose connection to the env manager...
            while(running) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            node_manager.Stop();
        }
    } catch(const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}