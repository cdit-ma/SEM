#ifndef ENVIRONMENTMANAGER_ENVIRONMENTCONTROLLER_H
#define ENVIRONMENTMANAGER_ENVIRONMENTCONTROLLER_H

#include <string>
#include <vector>

#include <zmq/protorequester/protorequester.hpp>

namespace EnvironmentManager{
    class EnvironmentController{
        public:
            EnvironmentController(const std::string& environment_manager_endpoint);
            void ShutdownExperiment(const std::string& experiment_name);
            std::vector<std::string> ListExperiments();
        private:
            zmq::ProtoRequester requester_;
    };
};

#endif //ENVIRONMENTMANAGER_ENVIRONMENTCONTROLLER_H