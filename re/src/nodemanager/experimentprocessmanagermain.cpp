#include "experimentprocessmanager.h"
auto main(int argc, char** argv) -> int
{
    try {
        auto config = re::NodeManager::ExperimentProcessManager::HandleArguments(argc, argv);
        auto epm = re::NodeManager::ExperimentProcessManager(config);
        epm.Start();

    } catch(const std::invalid_argument& ex) {
        // Return non-zero on failure
        return 1;
    } catch(const std::exception& ex) {
        std::cout << ex.what() << std::endl;
        // Return non-zero on failure
        return 1;
    }
    return 0;
}