#include "port.h"
#include "../node.h"
#include "../component.h"
#include "../environment.h"
#include "../experiment.h"
#include "../attribute.h"

#include "zmqport.h"
#include "qpidport.h"

using namespace EnvironmentManager;

std::unique_ptr<Port> Port::ConstructPort(Component& parent, const NodeManager::Port& port){
    try{
        switch(port.middleware()){
            case NodeManager::Middleware::ZMQ:{
                return std::unique_ptr<Port>(new zmq::Port(parent, port));
            }
            case NodeManager::Middleware::QPID:{
                return  std::unique_ptr<Port>(new qpid::Port(parent, port));
            }
            default:
                break;
        }
    }
    catch(const std::exception& ex){
    }
    return nullptr;
}
std::unique_ptr<Port> Port::ConstructBlackboxPort(Experiment& parent, const NodeManager::ExternalPort& port){
    return nullptr;
}

Port::Port(Component& parent, const NodeManager::Port& port) :
    experiment_(parent.GetNode().GetExperiment()),
    component_(&parent)
{
    

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

Port::Port(Experiment& parent, const NodeManager::ExternalPort& port):
experiment_(parent)
{

}


Port::~Port(){};


// Port::~Port(){
//     //Dont need to handle freeing tao ports, this is done at node level.
//     if(middleware_ == Middleware::Zmq){
//         if(kind_ == Kind::Publisher || kind_ == Kind::Replier){
//             GetEnvironment().FreePort(GetNode().GetIp(), producer_port_);
//         }
//     }

//      //Set if we are external
//     for(const auto& connected_id : connected_external_port_ids_){
//         switch(kind_){
//             case EnvironmentManager::Port::Kind::Publisher:
//             case EnvironmentManager::Port::Kind::Replier:{
//                 GetExperiment().RemoveExternalProducerPort(connected_id, id_);
//                 break;
//             }
//             case EnvironmentManager::Port::Kind::Subscriber:
//             case EnvironmentManager::Port::Kind::Requester:{
//                 GetExperiment().RemoveExternalConsumerPort(connected_id, id_);
//                 break;
//             }
//             default:
//                 break;
//         }
//     }
// }

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
    if(component_){
        return *(component_);
    }
    throw std::runtime_error("No Component");
}

Node& Port::GetNode() const{
    return GetComponent().GetNode();
}

Experiment& Port::GetExperiment() const{
    return experiment_;
}

Environment& Port::GetEnvironment() const{
    return GetExperiment().GetEnvironment();
}

void Port::SetType(const std::string& type){
    type_ = type;
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
    try{
        GetComponent().SetDirty();
        dirty_ = true;
    }catch(const std::exception& ex){

    }
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

    FillPortPb(*port);
    
    //TODO: STILL DO THIS?!
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

void Port::FillTopicPb(NodeManager::Port& port_pb){
    /*
    auto topic_pb = port_pb.add_attributes();
    topic_pb->mutable_info()->set_name("topic_name");
    topic_pb->set_kind(NodeManager::Attribute::STRING);
    if(GetKind() == Kind::Requester || GetKind() == Kind::Publisher){
        topic_pb->add_s(GetTopic());
    }else if(GetKind() == Kind::Subscriber || GetKind() == Kind::Requester){
        std::set<std::string> topic_names;
        //Connect all Internal ports
        for(auto port_id : GetInternalConnectedPortIds()){
            auto& publisher_port = GetExperiment().GetPort(port_id);
            const auto& topic_name = publisher_port.GetTopic();
            topic_names.insert(topic_name);
        }
        //Connect all External Ports
        for(const auto& port_id : GetExternalConnectedPortIds()){
            //Get the External Port Label from the Internal ID
            auto external_port_label = GetExperiment().GetExternalPortLabel(port_id);
            
            for(auto& publisher_port : GetEnvironment().GetExternalProducerPorts(external_port_label)){
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
            std::cerr << "* Experiment[" << GetExperiment().GetName() << "]: Has multiple topics connected to Port: '" << port.GetId() << "'" << std::endl;
        }
    }*/
}

std::vector<std::reference_wrapper<Port> > Port::GetConnectedPorts(){
    return {};
}

bool Port::IsBlackbox() const{
    //TODO
    return false;
}