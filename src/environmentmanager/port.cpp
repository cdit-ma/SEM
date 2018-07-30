#include "port.h"
#include "node.h"
#include "component.h"
#include "environment.h"
#include "experiment.h"
#include "attribute.h"

using namespace EnvironmentManager;

Port::Port(Environment& environment, Component& parent, const NodeManager::Port& port) :
    environment_(environment),
    component_(parent){
    

    id_ = port.info().id();
    name_ = port.info().name();
    kind_ = TranslateProtoKind(port.kind());
    middleware_ = TranslateProtoMiddleware(port.middleware());
    
    //Append to the list of namespaces
    for(const auto& ns : port.info().namespaces()){
        namespaces_.emplace_back(ns);
    }

    //Use the exact type
    SetType(port.info().type());

    for(const auto& attribute : port.attributes()){
        if(attribute.info().name() == "topic_name"){
            SetTopic(attribute.s(0));
        }
        AddAttribute(attribute);
    }

    //If we're a zmq publisher or zmq replier port, assign a publisher port
    if(middleware_ == EnvironmentManager::Port::Middleware::Zmq){
        if(kind_ == EnvironmentManager::Port::Kind::Publisher || kind_ == EnvironmentManager::Port::Kind::Replier){
            auto ip = GetNode().GetIp();
            SetProducerPort(GetEnvironment().GetPort(ip));
        }
    }

    //Set our server name to be our port name + our id to handle replication
    if(middleware_ == EnvironmentManager::Port::Middleware::Tao){
        const auto& orb_port = GetNode().AssignOrbPort();
        if(kind_ == EnvironmentManager::Port::Kind::Replier){
            SetProducerPort(orb_port);
        }

        for(auto& server_name_segment : port.server_name()){
            tao_server_name_list_.push_back(server_name_segment);
        }

        std::string topic = GetName() + "_" + GetId();
        tao_server_name_list_.push_back(topic);

        //TODO: Check if we are using a topic_name
        SetTopic(topic);
    }

    for(const auto& connected_id : port.connected_external_ports()){
        AddExternalConnectedPortId(connected_id);
        switch(kind_){
            case EnvironmentManager::Port::Kind::Publisher:
            case EnvironmentManager::Port::Kind::Replier:{
                GetExperiment().AddExternalProducerPort(connected_id, id_);
                break;
            }
            case EnvironmentManager::Port::Kind::Subscriber:
            case EnvironmentManager::Port::Kind::Requester:{
                GetExperiment().AddExternalConsumerPort(connected_id, id_);
                break;
            }
            default:
                break;
        }
    }

    for(const auto& connected_id : port.connected_ports()){
        AddInternalConnectedPortId(connected_id);
    }
}

Port::~Port(){
    //Dont need to handle freeing tao ports, this is done at node level.
    if(middleware_ == Middleware::Zmq){
        if(kind_ == Kind::Publisher || kind_ == Kind::Replier){
            GetEnvironment().FreePort(GetNode().GetIp(), producer_port_);
        }
    }

     //Set if we are external
    for(const auto& connected_id : connected_external_port_ids_){
        switch(kind_){
            case EnvironmentManager::Port::Kind::Publisher:
            case EnvironmentManager::Port::Kind::Replier:{
                GetExperiment().RemoveExternalProducerPort(connected_id, id_);
                break;
            }
            case EnvironmentManager::Port::Kind::Subscriber:
            case EnvironmentManager::Port::Kind::Requester:{
                GetExperiment().RemoveExternalConsumerPort(connected_id, id_);
                break;
            }
            default:
                break;
        }
    }
}

void Port::AddAttribute(const NodeManager::Attribute& attribute){
    attributes_.insert(std::make_pair(attribute.info().id(), 
            std::unique_ptr<EnvironmentManager::Attribute>(
                new EnvironmentManager::Attribute(attribute))));
}

std::string Port::GetId() const{
    return id_;
};
std::string Port::GetName() const{
    return name_;
};
Port::Kind Port::GetKind() const{
    return kind_;
}
Port::Middleware Port::GetMiddleware() const{
    return middleware_;
}

Component& Port::GetComponent() const{
    return component_;
}
Node& Port::GetNode() const{
    return component_.GetNode();
}
Experiment& Port::GetExperiment() const{
    return GetNode().GetExperiment();
}
Environment& Port::GetEnvironment() const{
    return environment_;
}

void Port::SetType(const std::string& type){
    type_ = type;
}

void Port::SetTopic(const std::string& topic_name){
    topic_name_ = topic_name;
}

void Port::SetProducerPort(const std::string& producer_port){
    producer_port_ = producer_port;
}

std::string Port::GetProducerPort() const{
    return producer_port_;
}

std::string Port::GetTopic() const{
    return topic_name_;
}

std::vector<std::string> Port::GetTaoServerName() const{
    return tao_server_name_list_;
}

std::string Port::GetTaoNamingServiceEndpoint() const{
    return "corbaloc:iiop:" + GetEnvironment().GetTaoNamingServiceAddress();
}

void Port::AddInternalConnectedPortId(const std::string& port_id){
    connected_internal_port_ids_.insert(port_id);
}

void Port::AddExternalConnectedPortId(const std::string& port_id){
    connected_external_port_ids_.insert(port_id);
}

const std::set<std::string>& Port::GetInternalConnectedPortIds(){
    return connected_internal_port_ids_;
}

const std::set<std::string>& Port::GetExternalConnectedPortIds(){
    return connected_external_port_ids_;
}

void Port::SetDirty(){
    component_.SetDirty();
    dirty_ = true;
}

bool Port::IsDirty() const{
    return dirty_;
}

NodeManager::Port* Port::GetUpdate(){
    if(dirty_){
        auto port = GetProto();
        dirty_ = false;
        return port;
    }
    return nullptr;
}

NodeManager::Port* Port::GetProto(){
    auto port = new NodeManager::Port();
    auto port_info = port->mutable_info();
    port_info->set_name(name_);
    port_info->set_id(id_);
    port_info->set_type(type_);
    for(const auto& ns : namespaces_){
        port_info->add_namespaces(ns);
    }
    port->set_kind(TranslateInternalKind(kind_));
    port->set_middleware(TranslateInternalMiddleware(middleware_));

    for(const auto& connected_port : connected_internal_port_ids_){
        port->add_connected_ports(connected_port);
    }

    for(const auto& external_port : connected_external_port_ids_){
        port->add_connected_external_ports(external_port);
    }

    switch(middleware_){
        case Middleware::Zmq:{
            FillZmqPortPb(*port, *this);
            break;
        }
        case Middleware::Rti:
        case Middleware::Ospl:{
            FillDdsPortPb(*port, *this);
            break;
        }
        case Middleware::Qpid:{
            FillQpidPortPb(*port, *this);
            break;
        }
        case Middleware::Tao:{
            FillTaoPortPb(*port, *this);
            break;
        }
        default:{
            break;
        }
    }

    for(const auto& attribute : attributes_){
        auto attribute_proto = attribute.second->GetProto();
        if(attribute_proto){
            port->mutable_attributes()->AddAllocated(attribute_proto);
        }
    }

    return port;
}

Port::Kind Port::TranslateProtoKind(const NodeManager::Port::Kind kind){
    const static std::map<NodeManager::Port::Kind, Port::Kind> kind_map({
        {NodeManager::Port::NO_KIND, Port::Kind::NoKind},
        {NodeManager::Port::PERIODIC, Port::Kind::Periodic},
        {NodeManager::Port::PUBLISHER, Port::Kind::Publisher},
        {NodeManager::Port::SUBSCRIBER, Port::Kind::Subscriber},
        {NodeManager::Port::REQUESTER, Port::Kind::Requester},
        {NodeManager::Port::REPLIER, Port::Kind::Replier}});

    if(kind_map.count(kind)){
        return kind_map.at(kind);
    }
    return Port::Kind::NoKind;
}

NodeManager::Port::Kind Port::TranslateInternalKind(const Port::Kind kind){
    const static std::map<Port::Kind, NodeManager::Port::Kind> kind_map({
        {Port::Kind::NoKind, NodeManager::Port::NO_KIND},
        {Port::Kind::Periodic, NodeManager::Port::PERIODIC},
        {Port::Kind::Publisher, NodeManager::Port::PUBLISHER},
        {Port::Kind::Subscriber, NodeManager::Port::SUBSCRIBER},
        {Port::Kind::Requester, NodeManager::Port::REQUESTER},
        {Port::Kind::Replier, NodeManager::Port::REPLIER}});

    if(kind_map.count(kind)){
        return kind_map.at(kind);
    }
    return NodeManager::Port::NO_KIND;
}

Port::Middleware Port::TranslateProtoMiddleware(const NodeManager::Middleware middleware){
    const static std::map<NodeManager::Middleware, Port::Middleware> mw_map({
        {NodeManager::Middleware::NO_MIDDLEWARE, Port::Middleware::NoMiddleware},
        {NodeManager::Middleware::ZMQ, Port::Middleware::Zmq},
        {NodeManager::Middleware::RTI, Port::Middleware::Rti},
        {NodeManager::Middleware::OSPL, Port::Middleware::Ospl},
        {NodeManager::Middleware::QPID, Port::Middleware::Qpid},
        {NodeManager::Middleware::TAO, Port::Middleware::Tao}});
    if(mw_map.count(middleware)){
        return mw_map.at(middleware);
    }
    return Port::Middleware::NoMiddleware;
}

NodeManager::Middleware Port::TranslateInternalMiddleware(const Port::Middleware middleware){
    const static std::map<Port::Middleware, NodeManager::Middleware> mw_map({
        {Port::Middleware::NoMiddleware, NodeManager::Middleware::NO_MIDDLEWARE},
        {Port::Middleware::Zmq, NodeManager::Middleware::ZMQ},
        {Port::Middleware::Rti, NodeManager::Middleware::RTI},
        {Port::Middleware::Ospl, NodeManager::Middleware::OSPL},
        {Port::Middleware::Qpid, NodeManager::Middleware::QPID},
        {Port::Middleware::Tao, NodeManager::Middleware::TAO}});
    if(mw_map.count(middleware)){
        return mw_map.at(middleware);
    }
    return NodeManager::Middleware::NO_MIDDLEWARE;
}

std::string Port::GetProducerEndpoint() const{
    const auto& node = GetNode();
    const auto& ip = node.GetIp();
    switch(middleware_){
        case Port::Middleware::Tao:{
            const auto& port = node.GetOrbPort();
            const auto& topic = GetTopic();
            return "corbaloc:iiop:" + ip + ":" + port + "/" + topic;
        }
        case Port::Middleware::Zmq:{
            const auto& port = GetProducerPort();
            return "tcp://" + ip + ":" + port;
        }
        default:
            break;
    }
    throw std::runtime_error("GetProducerEndpoint(): Middleware doens't provide Publisher Endpoint");
}

void Port::FillZmqPortPb(NodeManager::Port& port_pb, EnvironmentManager::Port& port){
    const auto& port_kind = port.GetKind();

    //Construct an attribute
    auto addr_pb = port_pb.add_attributes();

    if(port_kind == Kind::Publisher || port_kind == Kind::Subscriber){
        addr_pb->mutable_info()->set_name("publisher_address");
        addr_pb->set_kind(NodeManager::Attribute::STRINGLIST);
    }else if(port_kind == Kind::Requester || port_kind == Kind::Replier){
        addr_pb->mutable_info()->set_name("server_address");
        addr_pb->set_kind(NodeManager::Attribute::STRING);
    }
        
    if(port_kind == Kind::Publisher || port_kind == Kind::Replier){
        const auto& endpoint = port.GetProducerEndpoint();
        addr_pb->add_s(endpoint);
    }else if(port_kind == Kind::Subscriber || port_kind == Kind::Requester){
        //Connect all Internal ports
        for(auto port_id : port.GetInternalConnectedPortIds()){
            auto& publisher_port = port.GetExperiment().GetPort(port_id);
            const auto& endpoint = publisher_port.GetProducerEndpoint();
            addr_pb->add_s(endpoint);
        }
        //Connect all External Ports
        for(const auto& port_id : port.GetExternalConnectedPortIds()){
            //Get the External Port Label from the Internal ID
            auto external_port_label = port.GetExperiment().GetExternalPortLabel(port_id);
            
            for(auto& publisher_port : port.GetEnvironment().GetExternalProducerPorts(external_port_label)){
                const auto& endpoint = publisher_port.get().GetProducerEndpoint();
                addr_pb->add_s(endpoint);
            }
        }
    }
}

void Port::FillDdsPortPb(NodeManager::Port& port_pb, EnvironmentManager::Port& port){
    auto domain_pb = port_pb.add_attributes();
    domain_pb->mutable_info()->set_name("domain_id");
    domain_pb->set_kind(NodeManager::Attribute::INTEGER);
    domain_pb->set_i(0);

    FillTopicPb(port_pb, port);
}

void Port::FillQpidPortPb(NodeManager::Port& port_pb, EnvironmentManager::Port& port){
    auto broker_pb = port_pb.add_attributes();
    broker_pb->mutable_info()->set_name("broker");
    broker_pb->set_kind(NodeManager::Attribute::STRING);
    broker_pb->add_s(port.GetEnvironment().GetAmqpBrokerAddress());
    
    FillTopicPb(port_pb, port);
}

void Port::FillTopicPb(NodeManager::Port& port_pb, EnvironmentManager::Port& port){
    auto topic_pb = port_pb.add_attributes();
    topic_pb->mutable_info()->set_name("topic_name");
    topic_pb->set_kind(NodeManager::Attribute::STRING);
    if(port.GetKind() == Kind::Requester || port.GetKind() == Kind::Publisher){
        topic_pb->add_s(port.GetTopic());
    }else if(port.GetKind() == Kind::Subscriber || port.GetKind() == Kind::Requester){
        std::set<std::string> topic_names;
        //Connect all Internal ports
        for(auto port_id : port.GetInternalConnectedPortIds()){
            auto& publisher_port = port.GetExperiment().GetPort(port_id);
            const auto& topic_name = publisher_port.GetTopic();
            topic_names.insert(topic_name);
        }
        //Connect all External Ports
        for(const auto& port_id : port.GetExternalConnectedPortIds()){
            //Get the External Port Label from the Internal ID
            auto external_port_label = port.GetExperiment().GetExternalPortLabel(port_id);
            
            for(auto& publisher_port : port.GetEnvironment().GetExternalProducerPorts(external_port_label)){
                const auto& topic_name = publisher_port.get().GetTopic();
                topic_names.insert(topic_name);
            }
        }

        //Use the first topic_name defined
        for(const auto& topic_name : topic_names){
            topic_pb->add_s(topic_name);
            break;
        }

        if(topic_names.size() > 0){
            std::cerr << "* Experiment[" << port.GetExperiment().GetName() << "]: Has multiple topics connected to Port: '" << port.GetId() << "'" << std::endl;
        }
    }
}

void Port::FillTaoPortPb(NodeManager::Port& port_pb, EnvironmentManager::Port& port){
    auto& node = port.GetNode();
    auto orb_pb = port_pb.add_attributes();
    orb_pb->mutable_info()->set_name("orb_endpoint");
    orb_pb->set_kind(NodeManager::Attribute::STRING);
    orb_pb->add_s("iiop://" + node.GetIp() + ":" + node.GetOrbPort());

    //If we're a replier, we can set our server name(topic) and naming service endpoint based on internal information.
    if(port.GetKind() == Kind::Replier){
        auto name_pb = port_pb.add_attributes();
        name_pb->mutable_info()->set_name("server_name");
        name_pb->set_kind(NodeManager::Attribute::STRINGLIST);
        auto tao_name_server_list = port.GetTaoServerName();
        for(auto& server_name_segment_ : tao_name_server_list){
            name_pb->add_s(server_name_segment_);
        }

        auto naming_service_pb = port_pb.add_attributes();
        naming_service_pb->mutable_info()->set_name("naming_service_endpoint");
        naming_service_pb->set_kind(NodeManager::Attribute::STRING);
        naming_service_pb->add_s(port.GetTaoNamingServiceEndpoint());
    }

    //If we're a requester, get server name and naming service from connected replier port.
    if(port.GetKind() == Kind::Requester){
        //Connect all Internal ports
        for(auto port_id : port.GetInternalConnectedPortIds()){
            //Get our connected port
            auto& publisher_port = port.GetExperiment().GetPort(port_id);

            //Get server name and naming service endpoint
            const auto& tao_name_server_list = publisher_port.GetTaoServerName();
            const auto& naming_service_endpoint = publisher_port.GetTaoNamingServiceEndpoint();

            auto name_pb = port_pb.add_attributes();
            name_pb->mutable_info()->set_name("server_name");
            name_pb->set_kind(NodeManager::Attribute::STRINGLIST);
            for(auto& server_name_segment_ : tao_name_server_list){
                name_pb->add_s(server_name_segment_);
            }

            auto naming_service_pb = port_pb.add_attributes();
            naming_service_pb->mutable_info()->set_name("naming_service_endpoint");
            naming_service_pb->set_kind(NodeManager::Attribute::STRING);
            naming_service_pb->add_s(naming_service_endpoint);
        }


        //Connect all External Ports
        for(const auto& port_id : port.GetExternalConnectedPortIds()){
            //Get the External Port Label from the Internal ID
            auto external_port_label = port.GetExperiment().GetExternalPortLabel(port_id);
            
            for(auto& publisher_port : port.GetEnvironment().GetExternalProducerPorts(external_port_label)){
                //Get server name and naming service endpoint
                auto tao_name_server_list = publisher_port.get().GetTaoServerName();
                auto naming_service_endpoint = publisher_port.get().GetTaoNamingServiceEndpoint();

                auto name_pb = port_pb.add_attributes();
                name_pb->mutable_info()->set_name("server_name");
                name_pb->set_kind(NodeManager::Attribute::STRINGLIST);
                for(auto& server_name_segment_ : tao_name_server_list){
                    name_pb->add_s(server_name_segment_);
                }

                auto naming_service_pb = port_pb.add_attributes();
                naming_service_pb->mutable_info()->set_name("naming_service_endpoint");
                naming_service_pb->set_kind(NodeManager::Attribute::STRING);
                naming_service_pb->add_s(naming_service_endpoint);
            }
        }
    }
}