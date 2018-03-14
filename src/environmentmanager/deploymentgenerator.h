#include "controlmessage.pb.h"
#include "deploymentrule.h"
#include "deploymentrules/zmq/zmqrule.h"

class DeploymentGenerator{
    public:
        DeploymentGenerator(Environment& environment);
        void PopulateDeployment(NodeManager::ControlMessage message);
        void TerminateDeployment(NodeManager::ControlMessage message);

    private:

        Environment& environment_;

        typedef std::function<void(const NodeManager::EnvironmentMessage& message, NodeManager::EventPort& event_port)> middleware_rule_function;
        //middleware_rule_function typedef'd above
        std::unordered_map<DeploymentRule::MiddlewareType, middleware_rule_function> configure_rule_map_;
        std::unordered_map<DeploymentRule::MiddlewareType, middleware_rule_function> terminate_rule_map_;

        Zmq::DeploymentRule* zmq_deployment_rule_;

        std::unordered_map<DeploymentRule::MiddlewareType, NodeManager::EventPort::Middleware> middleware_enum_map_;
};