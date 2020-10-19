#include "qpidport.h"
#include "../environment.h"
#include "../experiment.h"
#include "helper.h"

namespace re::EnvironmentManager::qpid {

Port::Port(Component& parent, const NodeManager::Port& port) :
    EnvironmentManager::Port(parent, port), broker_address_(GetEnvironment().GetAmqpBrokerAddress())
{
    // Only set these vars if we're a publisher or replier port. These will be populated when
    // requested for all other kinds of port.
    if(kind_ == EnvironmentManager::Port::Kind::Publisher
       || kind_ == EnvironmentManager::Port::Kind::Replier) {
        topic_name_ = NodeManager::GetAttribute(port.attributes(), "topic_name").s(0);
    }
}

Port::Port(Experiment& parent, const NodeManager::ExternalPort& port) :
    EnvironmentManager::Port(parent, port),
    broker_address_(types::SocketAddress::from_string(
        NodeManager::GetAttribute(port.attributes(), "broker_address").s(0)))
{
    if(topic_name_.empty()) {
        throw std::runtime_error("QPID External Port requires a topic name");
    }
}

Port::Port(const types::Uuid& event_uuid,
           Component& parent,
           const Representation::ExperimentDefinition& experiment_definition,
           const Representation::MiddlewarePortInstance& port) :
    EnvironmentManager::Port(event_uuid, parent, experiment_definition, port),
    broker_address_{GetEnvironment().GetAmqpBrokerAddress()}
{
    try{
        topic_name_ = port.GetTopicName().value();
    } catch(const std::bad_optional_access& ex) {
        std::cerr << "Bad optional access in qpid port" << std::endl;
    }
}

std::string Port::GetTopic() const
{
    return topic_name_;
}

types::SocketAddress Port::GetBrokerAddress() const
{
    return broker_address_;
}

void Port::FillPortPb(NodeManager::Port& port_pb)
{
    auto attrs = port_pb.mutable_attributes();

    const auto& kind = GetKind();

    // If we're a replier, we can set our server name(topic) and naming service endpoint based on
    // internal information.
    if(kind == Kind::Replier || kind == Kind::Publisher) {
        std::set<std::string> blackbox_brokers;
        std::set<std::string> blackbox_topic_names;

        std::string broker_address = GetBrokerAddress().to_string();
        std::string topic_name = GetTopic();

        // Populate publish destination info based on any connection to blackbox ports.
        for(const auto& connected_port_ref : GetConnectedPorts()) {
            const auto& connected_port = connected_port_ref.get();
            const auto& qpid_port = (const Port&)connected_port;
            if(connected_port.IsBlackbox()) {
                blackbox_brokers.insert(qpid_port.GetBrokerAddress().to_string());
                blackbox_topic_names.insert(qpid_port.GetTopic());
            }
        }

        if(!blackbox_brokers.empty()) {
            if(blackbox_brokers.size() > 1) {
                std::cerr << "* Experiment[" << GetExperiment().GetName()
                          << "]: Has multiple blackbox QPID Brokers connected to Port: '" << GetId()
                          << "'" << std::endl;
            }
            broker_address = *(blackbox_brokers.begin());
            std::cerr << "* Experiment[" << GetExperiment().GetName()
                      << "]: Has blackbox connected to port: '" << GetId()
                      << "'. Overriding broker address!" << std::endl;
        }

        if(!blackbox_topic_names.empty()) {
            if(blackbox_topic_names.size() > 1) {
                std::cerr << "* Experiment[" << GetExperiment().GetName()
                          << "]: Has multiple blackbox topics connected to Port: '" << GetId()
                          << "'" << std::endl;
            }
            topic_name = *(blackbox_topic_names.begin());
            std::cerr << "* Experiment[" << GetExperiment().GetName()
                      << "]: Has blackbox connected to port: '" << GetId()
                      << "'. Overriding topic name!" << std::endl;
        }

        NodeManager::SetStringAttribute(attrs, "topic_name", topic_name);
        NodeManager::SetStringAttribute(attrs, "broker", broker_address);

    } else if(kind == Kind::Requester || kind == Kind::Subscriber) {
        std::set<std::string> brokers;
        std::set<std::string> topic_names;

        // Connect all Internal ports
        for(const auto& connected_port_ref : GetConnectedPorts()) {
            const auto& connected_port = connected_port_ref.get();
            const auto& qpid_port = (const Port&)connected_port;

            brokers.insert(qpid_port.GetBrokerAddress().to_string());
            topic_names.insert(qpid_port.GetTopic());
        }

        // If we have any size other than ONE for either list, warn user.
        if(topic_names.size() > 1) {
            std::cerr << "* Experiment[" << GetExperiment().GetName()
                      << "]: Has multiple topics connected to Port: '" << GetId() << "'"
                      << std::endl;
        }
        if(topic_names.empty()) {
            std::cerr << "* Experiment[" << GetExperiment().GetName() << "]: Port: '" << GetId()
                      << "' Has no connected ports, defaulting to user set topic_name."
                      << std::endl;
        }

        if(brokers.size() > 1) {
            std::cerr << "* Experiment[" << GetExperiment().GetName()
                      << "]: Has multiple QPID Brokers connected to Port: '" << GetId() << "'"
                      << std::endl;
        }
        if(brokers.empty()) {
            std::cerr << "* Experiment[" << GetExperiment().GetName() << "]: Port: '" << GetId()
                      << "' Has no connected ports, defaulting to user set broker_address."
                      << std::endl;
        }

        std::string broker = !brokers.empty() ? *brokers.begin() : GetBrokerAddress().to_string();
        NodeManager::SetStringAttribute(attrs, "broker", broker);

        std::string topic_name = !topic_names.empty() ? *topic_names.begin() : GetTopic();
        NodeManager::SetStringAttribute(attrs, "topic_name", topic_name);
    }
}

} // namespace re::EnvironmentManager::qpid
