#ifndef LOGAN_MANAGEDSERVER_H
#define LOGAN_MANAGEDSERVER_H

#include <util/execution.hpp>
#include <comms/environmentrequester/environmentrequester.h>

#include "server/server.h"

namespace NodeManager{
    class EnvironmentMessage;
}

class ManagedServer{
    public:
        class NotNeededException : public std::runtime_error{
            public:
                NotNeededException(const std::string& what_arg) : std::runtime_error(what_arg){};
        };

        ManagedServer(Execution& exectution, const std::string& experiment_name, const std::string& ip_address, const std::string& environment_manager_endpoint);
        void Terminate();
        void DelayedTerminate();
    private:
        void HandleExperimentUpdate(NodeManager::EnvironmentMessage& message);
        std::unique_ptr<EnvironmentRequest::HeartbeatRequester> requester_;

        //Environment vars.
        const std::string ip_address_;
        const std::string experiment_name_;
        const std::string environment_manager_address_;

        std::vector<std::unique_ptr<Server> > servers_;

        //Ref to our execution
        Execution& execution_;
};

#endif //LOGAN_MANAGEDSERVER_H
