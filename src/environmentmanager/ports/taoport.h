#ifndef ENVIRONMENT_MANAGER_TAO_PORT_H
#define ENVIRONMENT_MANAGER_TAO_PORT_H

#include "port.h"
#include <re_common/proto/controlmessage/controlmessage.pb.h>

namespace EnvironmentManager{
    namespace tao{
        class Port : public ::EnvironmentManager::Port{
            public:
                Port(Component& parent, const NodeManager::Port& port);
                Port(Experiment& parent, const NodeManager::ExternalPort& port);
            protected:
                const std::string& GetOrbEndpoint() const;
                const std::string& GetServerKind() const;
                const std::string& GetNamingServiceEndpoint() const;
                const std::vector<std::string>& GetServerName() const;

                void FillPortPb(NodeManager::Port& port_pb);
            private:
                std::vector<std::string> server_name_;
                std::string naming_service_endpoint_;
                std::string orb_endpoint_;
                std::string server_kind_;
        };
    };
};

#endif //ENVIRONMENT_MANAGER_TAO_PORT_H