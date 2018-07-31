#ifndef ENVIRONMENT_MANAGER_PORT_H
#define ENVIRONMENT_MANAGER_PORT_H

#include <mutex>
#include <unordered_map>
#include <memory>
#include <re_common/proto/controlmessage/controlmessage.pb.h>

namespace EnvironmentManager{
class Environment;
class Experiment;
class Node;
class Component;
class Attribute;
class Port{
    public:
        enum class Kind{
            NoKind = 0,
            Periodic = 1,
            Publisher = 2,
            Subscriber = 3,
            Requester = 4,
            Replier = 5,
        };
        enum class Middleware{
            NoMiddleware = 0,
            Zmq = 1,
            Rti = 2,
            Ospl = 3,
            Qpid = 4,
            Tao = 5,
        };
        virtual ~Port();

        static std::unique_ptr<Port> ConstructPort(Component& parent, const NodeManager::Port& port);
        static std::unique_ptr<Port> ConstructBlackboxPort(Experiment& parent, const NodeManager::ExternalPort& port);

        std::string GetName() const;
        std::string GetId() const;
        Port::Kind GetKind() const;
        Port::Middleware GetMiddleware() const;

        const std::vector<std::reference_wrapper<Port> > GetConnectedPorts() const;
        
        NodeManager::Port* GetUpdate();
        NodeManager::Port* GetProto();
        void SetDirty();
        bool IsDirty() const;
        bool IsBlackbox() const;
    

        Component& GetComponent() const;
        bool GotComponent() const;
        Node& GetNode() const;
        Experiment& GetExperiment() const;
        Environment& GetEnvironment() const;

    protected:
        Port(Component& parent, const NodeManager::Port& port);
        Port(Experiment& parent, const NodeManager::ExternalPort& port);
        Kind kind_;
        const std::set<std::string>& GetInternalConnectedPortIds() const;;
        const std::set<std::string>& GetExternalConnectedPortIds() const;
        void FillTopicPb(NodeManager::Port& port_pb);
        
    private:
        void SetType(const std::string& type);

        void AddInternalConnectedPortId(const std::string& port_id);
        void AddExternalConnectedPortId(const std::string& port_id);

        void AddAttribute(const NodeManager::Attribute& attribute);
        
        

        static Kind TranslateProtoKind(const NodeManager::Port::Kind kind);
        static NodeManager::Port::Kind TranslateInternalKind(const Port::Kind middleware);
        static NodeManager::Middleware TranslateInternalMiddleware(const Port::Middleware middleware);
        static Middleware TranslateProtoMiddleware(const NodeManager::Middleware middleware);

        virtual void FillPortPb(NodeManager::Port& port_pb) = 0;

        /*static void FillZmqPortPb(NodeManager::Port& port_pb, EnvironmentManager::Port& port);
        static void FillDdsPortPb(NodeManager::Port& port_pb, EnvironmentManager::Port& port);
        static void FillQpidPortPb(NodeManager::Port& port_pb, EnvironmentManager::Port& port);
        static void FillTaoPortPb(NodeManager::Port& port_pb, EnvironmentManager::Port& port);*/


        Experiment& experiment_;
        Component* component_ = 0;

        std::string id_;
        std::string name_;
        std::string type_;
        std::vector<std::string> namespaces_;
        Middleware middleware_;

        bool dirty_ = false;

        std::set<std::string> connected_internal_port_ids_;
        std::set<std::string> connected_external_port_ids_;

        std::unordered_map<std::string, std::unique_ptr<Attribute> > attributes_;
};
};

#endif //ENVIRONMENT_MANAGER_COMPONENT_H