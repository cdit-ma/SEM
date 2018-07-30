#ifndef ENVIRONMENT_MANAGER_PORT_H
#define ENVIRONMENT_MANAGER_PORT_H

#include <mutex>
#include <unordered_map>
#include "uniquequeue.hpp"
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
        Port(Environment& environment, Component& parent, const NodeManager::Port& port);
        ~Port();
        std::string GetName() const;
        std::string GetId() const;
        Port::Kind GetKind() const;
        Port::Middleware GetMiddleware() const;
        
        NodeManager::Port* GetUpdate();
        NodeManager::Port* GetProto();
        void SetDirty();
        bool IsDirty() const;
    
        Component& GetComponent() const;
        Node& GetNode() const;
        Experiment& GetExperiment() const;
        Environment& GetEnvironment() const;

    //TODO: MOVE INTO CORRECT INSTATATIONS      
    //ZMQ: 
        std::string GetProducerPort() const;
        std::string GetProducerEndpoint() const;
    //
        std::string GetTopic() const;

    //tao:
        std::vector<std::string> GetTaoServerName() const;
        std::string GetTaoNamingServiceEndpoint() const;
        
        
    private:
        void SetType(const std::string& type);
        void SetProducerPort(const std::string& publisher_port);
        void SetTopic(const std::string& topic_name);

        void AddInternalConnectedPortId(const std::string& port_id);
        void AddExternalConnectedPortId(const std::string& port_id);

        void AddAttribute(const NodeManager::Attribute& attribute);
        
        const std::set<std::string>& GetInternalConnectedPortIds();
        const std::set<std::string>& GetExternalConnectedPortIds();
        

        static Kind TranslateProtoKind(const NodeManager::Port::Kind kind);
        static NodeManager::Port::Kind TranslateInternalKind(const Port::Kind middleware);
        static NodeManager::Middleware TranslateInternalMiddleware(const Port::Middleware middleware);
        static Middleware TranslateProtoMiddleware(const NodeManager::Middleware middleware);

        //
        void FillPortPb(NodeManager::Port& port_pb) = 0;

        static void FillZmqPortPb(NodeManager::Port& port_pb, EnvironmentManager::Port& port);
        static void FillDdsPortPb(NodeManager::Port& port_pb, EnvironmentManager::Port& port);
        static void FillQpidPortPb(NodeManager::Port& port_pb, EnvironmentManager::Port& port);
        static void FillTaoPortPb(NodeManager::Port& port_pb, EnvironmentManager::Port& port);

        static void FillTopicPb(NodeManager::Port& port_pb, EnvironmentManager::Port& port);
    private:
        Environment& environment_;
        Component& component_;

        std::string id_;
        std::string name_;
        std::string type_;
        std::vector<std::string> namespaces_;
        Kind kind_;
        Middleware middleware_;

        bool dirty_ = false;


        std::string topic_name_;
        std::string producer_port_;;

        std::set<std::string> connected_internal_port_ids_;
        std::set<std::string> connected_external_port_ids_;

        std::vector<std::string> tao_server_name_list_;

        std::unordered_map<std::string, std::unique_ptr<Attribute> > attributes_;
};
};

#endif //ENVIRONMENT_MANAGER_COMPONENT_H