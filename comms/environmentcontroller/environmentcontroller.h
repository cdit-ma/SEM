#ifndef ENVIRONMENTMANAGER_ENVIRONMENTCONTROLLER_H
#define ENVIRONMENTMANAGER_ENVIRONMENTCONTROLLER_H

#include <string>
#include <vector>

#include <proto/controlmessage/controlmessage.pb.h>
#include <zmq/protorequester/protorequester.hpp>

namespace EnvironmentManager{
    class EnvironmentController{
        public:
            EnvironmentController(const std::string& environment_manager_endpoint);
            std::vector<std::string> ShutdownExperiment(const std::string& experiment_name, bool is_regex);
            std::unique_ptr<NodeManager::RegisterExperimentReply> AddExperiment(const std::string& experiment_name, const std::string& graphml_path);
            std::vector<std::string> ListExperiments();
            std::string InspectExperiment(const std::string& experiment_name);
        private:
            zmq::ProtoRequester requester_;
    };
};

#endif //ENVIRONMENTMANAGER_ENVIRONMENTCONTROLLER_H