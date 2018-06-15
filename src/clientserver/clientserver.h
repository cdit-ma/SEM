#ifndef LOGAN_CLIENT_SERVER_H
#define LOGAN_CLIENT_SERVER_H

#include <re_common/util/execution.hpp>
#include <re_common/zmq/environmentrequester/environmentrequester.h>

#include "server/server.h"
#include "client/logcontroller.h"

namespace NodeManager{
    class EnvironmentMessage;
}

class ClientServer{
    public:
        ClientServer(Execution& exectution, const std::string& address, const std::string& experiment_id, const std::string& environment_manager_address);

        void Terminate();

    private:
        void HandleUpdate(NodeManager::EnvironmentMessage& message);

        //Environment vars.
        std::string address_;
        std::string experiment_id_;
        std::string environment_manager_address_;

        //Client and server pointers.
        std::vector<std::unique_ptr<LogController> > clients_;
        std::vector<std::unique_ptr<Server> > servers_;



        std::unique_ptr<std::thread> wait_thread_;

        //Ref to our execution
        Execution& execution_;

        std::unique_ptr<EnvironmentRequester> requester_;

};

#endif //LOGAN_CLIENT_SERVER_H
