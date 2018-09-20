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
        ManagedServer(Execution& exectution, const std::string& address, const std::string& experiment_id, const std::string& environment_manager_address);
        void Terminate();

    private:
        void HandleUpdate(NodeManager::EnvironmentMessage& message);

        //Environment vars.
        std::string address_;
        std::string experiment_id_;
        std::string environment_manager_address_;

        std::vector<std::unique_ptr<Server> > servers_;

        const static int MAX_RETRY_COUNT = 5;

        //Ref to our execution
        Execution& execution_;
        std::unique_ptr<EnvironmentRequester> requester_;
};

#endif //LOGAN_MANAGEDSERVER_H
