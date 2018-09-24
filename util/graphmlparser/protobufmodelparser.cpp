#include "protobufmodelparser.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <google/protobuf/util/json_util.h>
#include <proto/controlmessage/helper.h>


#include "graphmlparser.h"



ProtobufModelParser::ProtobufModelParser(const std::string& filename, const std::string& experiment_id){
    experiment_id_ = experiment_id;
    graphml_parser_ = std::unique_ptr<GraphmlParser>(new GraphmlParser(filename));
    pre_process_success_ = PreProcess();
    process_success_ = Process();
    
}

//Pretty json prints deployment protbuf controlmessage
std::string ProtobufModelParser::GetDeploymentJSON(){

    std::string output;
    google::protobuf::util::JsonPrintOptions options;
    options.add_whitespace = true;
    options.always_print_primitive_fields = false;

    google::protobuf::util::MessageToJsonString(*control_message_, &output, options);
    return output;
}

NodeManager::ControlMessage* ProtobufModelParser::ControlMessage(){
    return control_message_;
}

// Starting at a node, recurse through all edges originating from it (of a particular edge kind).
// Returns all nodes which don't have edges (of a particular edge kind) originating from them.
/* Visual Representation

    [B]-------|
                V
    [C]----->[A]---->[X]
                ʌ
                |
    [E]----->[D]
                ʌ
                |
                [F]

    Calling this function on node X would return {B,C,E,F} but ignore {A,D}
*/ 
std::set<std::string> ProtobufModelParser::GetTerminalSourcesByEdgeKind(const std::string& node_id, const std::string& edge_kind, std::set<std::string> prev_ids){
    std::set<std::string> source_ids;

    bool is_assembly_edge = edge_kind == "Edge_Assembly";
    
    //Detect cycle
    if(prev_ids.count(node_id)){
        throw std::runtime_error("Cyclic link exists in edges " + edge_kind + " containing node id: " + node_id);
    }
    prev_ids.insert(node_id);
    
    //Get all edges connected to node_id
    for(const auto& edge_id : entity_edge_ids_[node_id]){
        if(edge_kind == graphml_parser_->GetDataValue(edge_id, "kind")){
            //If node provided is the target of this edge
            if(node_id == graphml_parser_->GetAttribute(edge_id, "target")){
                //Get the source of this edge
                const auto& edge_source = graphml_parser_->GetAttribute(edge_id, "source");
                
                bool recurse_edges = true;
                if(is_assembly_edge){
                    //Treat External Delegate Ports as Terminations so we don't break intra-model connectivity
                    const auto& source_kind = graphml_parser_->GetDataValue(edge_source, "kind");
                    if(source_kind == "PubSubPortDelegate" || source_kind == "RequestPortDelegate"){
                        recurse_edges = false;
                    }
                }
                
                //Get the edges originating from the source
                const auto& connected_sources = GetTerminalSourcesByEdgeKind(edge_source, edge_kind, prev_ids);

                //If we have edges originating from the source, append them
                if(recurse_edges && connected_sources.size()){
                    for(const auto& source_id : connected_sources){
                        source_ids.insert(source_id);
                    }
                }else{
                    //If this node has no other edges, it itself is a terminal endpoint
                    //So add it to our source_ids set
                    source_ids.insert(edge_source);
                }
            }
        }
    }

    return source_ids;
}

int ProtobufModelParser::GetReplicationId(const std::string& id){
    int replication_count = 1;
    try{
        //Try and get the current assembly replication count
        replication_count = std::stoi(graphml_parser_->GetDataValue(id, "replicate_count"));
    }
    catch(const std::exception& ex){}
    return replication_count;
}

void ProtobufModelParser::GenerateReplications(Assembly* current_assembly){
    const auto& assembly_id = current_assembly->assembly_id;
    current_assembly->replication_count = GetReplicationId(assembly_id);

    //Get the list of children
    auto assembly_children = graphml_parser_->FindImmediateChildren("ComponentAssembly", assembly_id);
    auto component_children = graphml_parser_->FindImmediateChildren("ComponentInstance", assembly_id);


    for(int i = 0; i < current_assembly->replication_count; i++){
        auto replicated_assembly = current_assembly;
        if(i > 0){
            //Construct new assembly objects for Replications past one
            replicated_assembly = new Assembly(current_assembly);
        }

        replicated_assembly->replication_index = i;


        for(const auto& child_assembly_id : assembly_children){
            //Construct Assembly here
            auto child_assembly = new Assembly(graphml_parser_->GetDataValue(child_assembly_id, "label"), child_assembly_id, replicated_assembly);
            GenerateReplications(child_assembly);
        }

        for(const auto& component_child_id : component_children){
            auto component = new ComponentReplication();
            component->component_instance_id = component_child_id;
            component->parent = replicated_assembly;
            replicated_assembly->replications.push_back(component);
            component_instances_.push_back(component);
        }
    }
}

bool ProtobufModelParser::PreProcess(){
    if(!graphml_parser_){
        return false;
    }

    auto assembly_definitions = graphml_parser_->FindNodes("AssemblyDefinitions");

    if(assembly_definitions.size() != 1){
        std::cerr << "Don't have an Assembly Definition! Cannot continue" << std::endl;
        return false;
    }

    auto assembly_definition_id = assembly_definitions[0];

    //Get all ids
    hardware_node_ids_ = graphml_parser_->FindNodes("HardwareNode");
    hardware_cluster_ids_ = graphml_parser_->FindNodes("HardwareCluster");

    logging_server_ids_ = graphml_parser_->FindNodes("LoggingServer");
    logging_client_ids_ = graphml_parser_->FindNodes("LoggingProfile");

    component_ids_ = graphml_parser_->FindNodes("Component");
    component_instance_ids_ = graphml_parser_->FindNodes("ComponentInstance");
    component_assembly_ids_ = graphml_parser_->FindNodes("ComponentAssembly");

    delegates_pubsub_ids_ = graphml_parser_->FindNodes("PubSubPortDelegate");
    delegates_server_ids_ = graphml_parser_->FindNodes("RequestPortDelegate");

    model_id_ = graphml_parser_->FindNodes("Model")[0];



    //Get the ID's of the edges
    deployment_edge_ids_ = graphml_parser_->FindEdges("Edge_Deployment");
    assembly_edge_ids_ = graphml_parser_->FindEdges("Edge_Assembly");
    definition_edge_ids_ = graphml_parser_->FindEdges("Edge_Definition");
    aggregate_edge_ids_ = graphml_parser_->FindEdges("Edge_Aggregate");
    qos_edge_ids_ = graphml_parser_->FindEdges("Edge_QOS");
    data_edge_ids_ = graphml_parser_->FindEdges("Edge_Data");

    for(const auto& edge_id : graphml_parser_->FindEdges()){
        auto source_id = graphml_parser_->GetAttribute(edge_id, "source");
        auto target_id = graphml_parser_->GetAttribute(edge_id, "target");
        entity_edge_ids_[source_id].insert(edge_id);
        entity_edge_ids_[target_id].insert(edge_id);
    }

    //Construct a Deployment Map which points ComponentInstance - > HardwareNodes
    for(const auto& e_id: deployment_edge_ids_){
        auto source_id = graphml_parser_->GetAttribute(e_id, "source");
        auto target_id = graphml_parser_->GetAttribute(e_id, "target");
        deployed_entities_map_[source_id] = target_id;
    }

    top_level_assembly_ = new Assembly(graphml_parser_->GetDataValue(assembly_definition_id, "label"), assembly_definition_id);

    GenerateReplications(top_level_assembly_);

    //Construct a Deployment Map which points EventPort - > QOSProfile
    for(const auto& e_id: qos_edge_ids_){
        auto source_id = graphml_parser_->GetAttribute(e_id, "source");
        auto target_id = graphml_parser_->GetAttribute(e_id, "target");
        entity_qos_map_[source_id] = target_id;
    }


    std::vector<std::string> endpoint_ids;

    auto sub_port_ids = graphml_parser_->FindNodes("SubscriberPortInstance");
    auto rep_port_ids = graphml_parser_->FindNodes("ReplierPortInstance");

    endpoint_ids.insert(endpoint_ids.end(), sub_port_ids.begin(), sub_port_ids.end());
    endpoint_ids.insert(endpoint_ids.end(), rep_port_ids.begin(), rep_port_ids.end());
    endpoint_ids.insert(endpoint_ids.end(), delegates_pubsub_ids_.begin(), delegates_pubsub_ids_.end());
    endpoint_ids.insert(endpoint_ids.end(), delegates_server_ids_.begin(), delegates_server_ids_.end());

    //Populate port connection map using recurse edge function to follow port delegates through
    for(const auto& port_id : endpoint_ids){
        const auto& target_id = port_id;
        for(auto source_id : GetTerminalSourcesByEdgeKind(target_id, "Edge_Assembly", {})){
            AssemblyConnection edge;
            edge.source_id = source_id;
            edge.target_id = target_id;

            //Check to see if our Ports parents are the same
            auto src_pid = graphml_parser_->GetParentNode(source_id, 2);
            auto dst_pid = graphml_parser_->GetParentNode(target_id, 2);
            //Component->EventPort, so checking two parents up should tell us if we are contained within the same Assembly
            edge.inter_assembly = src_pid != dst_pid;
            assembly_map_[source_id].push_back(edge);
        }
    }

    //Find the values for the Attributes which have been data linked
    for(const auto& attribute_id : graphml_parser_->FindNodes("AttributeInstance", assembly_definition_id)){
        //Get all ids which feed attribute_id with Data
        auto data_sources = GetTerminalSourcesByEdgeKind(attribute_id, "Edge_Data", {});

        auto data_value = graphml_parser_->GetDataValue(attribute_id, "value");
        if(data_sources.size() == 1){
            
            data_value = graphml_parser_->GetDataValue(*(data_sources.begin()), "value");
        }
        attribute_value_map_[attribute_id] = data_value;
    }


    //Construct port fully qualified port ids
    auto publisher_port_ids = graphml_parser_->FindNodes("PublisherPortInstace", "");
    auto subscriber_port_ids = graphml_parser_->FindNodes("SubscriberPortInstace", "");

    auto requester_port_ids = graphml_parser_->FindNodes("RequesterPortInstace", "");
    auto replier_port_ids = graphml_parser_->FindNodes("ReplierPortInstace", "");

    for(const auto& port_id : publisher_port_ids){
        port_guid_map_[port_id] = BuildPortGuid(port_id);
    }

    for(const auto& port_id : subscriber_port_ids){
        port_guid_map_[port_id] = BuildPortGuid(port_id);
    }

    for(const auto& port_id : requester_port_ids){
        port_guid_map_[port_id] = BuildPortGuid(port_id);
    }

    for(const auto& port_id : replier_port_ids){
        port_guid_map_[port_id] = BuildPortGuid(port_id);
    }

    for(const auto& edge_id : assembly_edge_ids_){
        auto target_id = graphml_parser_->GetAttribute(edge_id, "target");
        auto source_id = graphml_parser_->GetAttribute(edge_id, "source");
        if(std::find(logging_server_ids_.begin(), logging_server_ids_.end(), target_id) != logging_server_ids_.end()){
            logging_server_client_map_[target_id].push_back(source_id);
        }
    }

    return true;
}

bool ProtobufModelParser::ParseHardwareItems(NodeManager::ControlMessage* control_message){

    for(const auto& cluster_id : hardware_cluster_ids_){
        auto cluster = control_message->add_nodes();
        cluster->set_type(GetHardwareItemKind(graphml_parser_->GetDataValue(cluster_id, "kind")));
        cluster->mutable_info()->set_id(cluster_id);
        cluster->mutable_info()->set_name(graphml_parser_->GetDataValue(cluster_id, "label"));

        node_message_map_[cluster_id] = cluster;
    }

    for(const auto& hardware_id : hardware_node_ids_){

        auto parent_id = graphml_parser_->GetParentNode(hardware_id);

        NodeManager::Node* node = 0;

        if(node_message_map_.count(parent_id)){
            node = node_message_map_[parent_id]->add_nodes();
        }
        else{
            node = control_message->add_nodes();
        }
        node->set_type(GetHardwareItemKind(graphml_parser_->GetDataValue(hardware_id, "kind")));
        node->mutable_info()->set_id(hardware_id);
        node->mutable_info()->set_name(graphml_parser_->GetDataValue(hardware_id, "label"));

        const auto& ip_address = graphml_parser_->GetDataValue(hardware_id, "ip_address");
        NodeManager::SetStringAttribute(node->mutable_attributes(), "ip_address", ip_address);
        
        node_message_map_[hardware_id] = node;
    }
    return true;
}

bool ProtobufModelParser::ParseExternalDelegates(NodeManager::ControlMessage* control_message){

    std::vector<std::string> delegate_ids;
    delegate_ids.insert(delegate_ids.end(), delegates_pubsub_ids_.begin(), delegates_pubsub_ids_.end());
    delegate_ids.insert(delegate_ids.end(), delegates_server_ids_.begin(), delegates_server_ids_.end());

    for(const auto& port_id : delegate_ids){
        auto parent_id = graphml_parser_->GetParentNode(port_id);
        auto eport_pb = control_message->add_external_ports();
        auto eport_info_pb = eport_pb->mutable_info();
        eport_info_pb->set_id(port_id);
        auto label = graphml_parser_->GetDataValue(parent_id, "label");
        eport_info_pb->set_name(label);
        eport_info_pb->set_type(graphml_parser_->GetDataValue(port_id, "type"));
        
        auto port_kind = GetExternalPortKind(graphml_parser_->GetDataValue(port_id, "kind"));
        eport_pb->set_kind(port_kind);

        bool is_blackbox = graphml_parser_->GetDataValue(parent_id, "blackbox") == "true";
        eport_pb->set_is_blackbox(is_blackbox);
        
        
        //Set Middleware
        auto middleware_str = graphml_parser_->GetDataValue(port_id, "middleware");
        auto middleware = NodeManager::NO_MIDDLEWARE;

        if(!NodeManager::Middleware_Parse(middleware_str, &middleware)){
            std::cerr << "Cannot parse middleware: " << middleware_str << std::endl;
        }
        eport_pb->set_middleware(middleware);

        auto attrs = eport_pb->mutable_attributes();

        if(is_blackbox){
            switch(middleware){
                case NodeManager::QPID:{
                    //QPID Requires Topic and Broker Addresss
                    const auto& topic_name = graphml_parser_->GetDataValue(port_id, "topic_name");
                    const auto& broker_addr = graphml_parser_->GetDataValue(port_id, "qpid_broker_address");

                    NodeManager::SetStringAttribute(attrs, "topic_name", topic_name);
                    NodeManager::SetStringAttribute(attrs, "broker_address", broker_addr);
                    break;
                }
                case NodeManager::ZMQ:{
                    //ZMQ Requires zmq_publisher_address (PUBSUB) or zmq_server_address (SERVER)
                    if(port_kind == NodeManager::ExternalPort::PUBSUB){
                        const auto& pub_addr = graphml_parser_->GetDataValue(port_id, "zmq_publisher_address");
                        NodeManager::SetStringAttribute(attrs, "publisher_address", pub_addr);
                    }else if(port_kind == NodeManager::ExternalPort::SERVER){
                        auto serv_addr = graphml_parser_->GetDataValue(port_id, "zmq_server_address");
                        NodeManager::SetStringAttribute(attrs, "server_address", serv_addr);
                    }
                    break;
                }
                case NodeManager::RTI:
                case NodeManager::OSPL:{
                    //DDS Requires domain_id and topic
                    const auto& topic_name = graphml_parser_->GetDataValue(port_id, "topic_name");
                    const auto& domain_id = std::stoi(graphml_parser_->GetDataValue(port_id, "dds_domain_id"));

                    NodeManager::SetStringAttribute(attrs, "topic_name", topic_name);
                    NodeManager::SetIntegerAttribute(attrs, "domain_id", domain_id);
                    break;
                }
                case NodeManager::TAO:{
                    //TAO Requires tao_orb_endpoint
                    const auto& naming_server_endpoint = graphml_parser_->GetDataValue(port_id, "tao_naming_service_endpoint");
                    auto server_name_list = graphml_parser_->GetDataValue(port_id, "tao_server_name");
                    auto server_kind = graphml_parser_->GetDataValue(port_id, "tao_server_kind");

                    //Split slash seperated server name into seperate strings.
                    std::transform(server_name_list.begin(), server_name_list.end(), server_name_list.begin(), [](char ch) {
                        return ch == '/' ? ' ' : ch;
                    });
                    std::istringstream iss(server_name_list);
                    std::vector<std::string> split_server_name(std::istream_iterator<std::string>{iss},
                                            std::istream_iterator<std::string>());

                    NodeManager::SetStringAttribute(attrs, "naming_server_endpoint", naming_server_endpoint);
                    NodeManager::SetStringListAttribute(attrs, "server_name", split_server_name);
                    NodeManager::SetStringAttribute(attrs, "server_kind", server_kind);
                    break;
                }
                default:
                    break;
            }
        }

        if(!external_port_id_map_.count(port_id)){
            external_port_id_map_[port_id] = eport_pb;
        }
    }
    return true;
}

std::string ProtobufModelParser::GetDeployedHardwareID(const std::string& component_id){
    if(deployed_entities_map_.count(component_id)){
       return deployed_entities_map_[component_id]; 
    }else{
        auto parent_id = graphml_parser_->GetParentNode(component_id);
        if(!parent_id.empty()){
            return GetDeployedHardwareID(parent_id);
        }else{
            return std::string();
        }
    }
};

void ProtobufModelParser::ParseLoggingClients(){
    for(const auto& client_id : logging_client_ids_){
        //Get hardware node pb message that this logger is deployed to
        auto hardware_id = deployed_entities_map_[client_id];
        NodeManager::Node* node_pb = 0;
        if(node_message_map_.count(hardware_id)){
            node_pb = node_message_map_.at(hardware_id);
        }else{
            continue;
        }

        auto logger_pb = node_pb->add_loggers();

        logger_pb->set_type(NodeManager::Logger::CLIENT);
        logger_pb->set_id(client_id);
        logger_pb->set_frequency(std::stod(graphml_parser_->GetDataValue(client_id, "frequency")));

        auto processes = graphml_parser_->GetDataValue(client_id, "processes_to_log");
        std::transform(processes.begin(), processes.end(), processes.begin(), [](char ch) {
            return ch == ',' ? ' ' : ch;
        });


        std::istringstream iss(processes);
        std::vector<std::string> split_processes(std::istream_iterator<std::string>{iss},
                                 std::istream_iterator<std::string>());

        for(const auto& process : split_processes){
            logger_pb->add_processes(process);
        }

        if(graphml_parser_->GetDataValue(client_id, "mode") == "LIVE"){
            logger_pb->set_mode(NodeManager::Logger::LIVE);
        }else if(graphml_parser_->GetDataValue(client_id, "mode") == "CACHED"){
            logger_pb->set_mode(NodeManager::Logger::CACHED);
        }
    }
}

void ProtobufModelParser::ParseLoggingServers(){
    for(const auto& server_id : logging_server_ids_){
        //Get hardware node pb message that this logger is deployed to
        auto hardware_id = deployed_entities_map_[server_id];
        NodeManager::Node* node_pb = 0;
        if(node_message_map_.count(hardware_id)){
            node_pb = node_message_map_.at(hardware_id);
        }else{
            continue;
        }

        auto logger_pb = node_pb->add_loggers();

        logger_pb->set_type(NodeManager::Logger::SERVER);
        logger_pb->set_id(server_id);
        logger_pb->set_db_file_name(graphml_parser_->GetDataValue(server_id, "database"));

        //get connected log client ids
        if(logging_server_client_map_.count(server_id)){
            for(const auto& client_id : logging_server_client_map_.at(server_id)){
                logger_pb->add_client_ids(client_id);
            }
        }
    }
}

void ProtobufModelParser::ParseComponents(){
    for(auto component_instance : component_instances_){
        const auto& component_id = component_instance->component_instance_id;
        const auto& component_def_id = GetDefinitionId(component_id);
        const auto& component_uid = component_instance->GetUniqueId();
        const auto& unique_suffix = component_instance->GetUniqueIdSuffix();
        
        //Get id of deployed node
        auto hardware_id = GetDeployedHardwareID(component_id);

        //Get hardware node pb message that this component is deployed to
        NodeManager::Node* node_pb = 0;
        if(node_message_map_.count(hardware_id)){
            node_pb = node_message_map_.at(hardware_id);
        }
        //Move to next component_instance if we're missing anything
        if(!node_pb){
            continue;
        }

        //Set Info for Component PB
        auto component_pb = node_pb->add_components();
        //Add the Component to the replication map
        component_replications_[component_id].push_back(component_pb);
        auto component_info_pb = component_pb->mutable_info();
        component_info_pb->set_id(component_uid);
        component_info_pb->set_name(graphml_parser_->GetDataValue(component_id, "label"));
        component_info_pb->set_type(graphml_parser_->GetDataValue(component_def_id, "label"));

        for(auto ns : GetNamespace(component_def_id)){
            component_info_pb->add_namespaces(ns);
        }

        const auto& replication_indices = component_instance->GetReplicationIndices();
        const auto& location = component_instance->GetReplicationLocation();

        *(component_pb->mutable_replicate_indices()) = {replication_indices.begin(), replication_indices.end()};
        *(component_pb->mutable_location()) = {location.begin(), location.end()};

        //Setup Attributes For Component PB
        FillProtobufAttributes(component_pb->mutable_attributes(), component_id, component_instance->GetUniqueIdSuffix());

        //Get All Children which we need to setup
        auto publisher_port_ids = graphml_parser_->FindImmediateChildren("PublisherPortInstance", component_id);
        auto subscriber_port_ids = graphml_parser_->FindImmediateChildren("SubscriberPortInstance", component_id);
        auto requester_port_ids = graphml_parser_->FindImmediateChildren("RequesterPortInstance", component_id);
        auto replier_port_ids = graphml_parser_->FindImmediateChildren("ReplierPortInstance", component_id);
        auto periodic_ids = graphml_parser_->FindImmediateChildren("PeriodicPortInstance", component_id);
        auto class_instance_ids = graphml_parser_->FindImmediateChildren("ClassInstance", component_id);

        std::vector<std::string> pubsub_port_ids;
        std::vector<std::string> reqrep_port_ids;

        //Combine Publisher and Subscriber Ports
        pubsub_port_ids.insert(pubsub_port_ids.end(), publisher_port_ids.begin(), publisher_port_ids.end());
        pubsub_port_ids.insert(pubsub_port_ids.end(), subscriber_port_ids.begin(), subscriber_port_ids.end());

        //Combine Requester and Replier Ports
        reqrep_port_ids.insert(reqrep_port_ids.end(), requester_port_ids.begin(), requester_port_ids.end());
        reqrep_port_ids.insert(reqrep_port_ids.end(), replier_port_ids.begin(), replier_port_ids.end());


        //Process Publisher/Subscriber Ports
        for(const auto& port_id : pubsub_port_ids){
            auto port_pb = ConstructPubSubPortPb(port_id, unique_suffix);
            if(port_pb){
                //Insert into the replicated port map
                port_replicate_id_map_[port_pb->info().id()] = port_pb;
                component_pb->mutable_ports()->AddAllocated(port_pb);
            }
        }

        //Process Requester/Replier Ports
        for(const auto& port_id : reqrep_port_ids){
            auto port_pb = ConstructReqRepPortPb(port_id, unique_suffix);
            if(port_pb){
                //Insert into the replicated port map
                port_replicate_id_map_[port_pb->info().id()] = port_pb;
                component_pb->mutable_ports()->AddAllocated(port_pb);
            }
        }

        //Process Periodic Ports
        for(const auto& periodic_id : periodic_ids){
            auto port_pb = ConstructPeriodicPb(periodic_id, unique_suffix);
            if(port_pb){
                component_pb->mutable_ports()->AddAllocated(port_pb);
            }
        }

        //Handle Class Instances
        for(const auto& class_instance_id : class_instance_ids){
            auto worker_pb = ConstructWorkerPb(class_instance_id, unique_suffix);
            if(worker_pb){
                component_pb->mutable_workers()->AddAllocated(worker_pb);
            }
        }
    }
}

bool ProtobufModelParser::Process(){
    if(!graphml_parser_){
        return false;
    }

    control_message_ = new NodeManager::ControlMessage();
    control_message_->set_experiment_id(experiment_id_);

    //populate environment message's hardware fields. Fills local node_message_map_
    ParseHardwareItems(control_message_);
    ParseExternalDelegates(control_message_);

    ParseLoggingClients();

    ParseLoggingServers();

    ParseComponents();

    CalculateReplication();
    return true;
}

void ProtobufModelParser::CalculateReplication(){
    //Calculate the connections taking into account replication!
    for(const auto& m : assembly_map_){
        for(const auto& ac : m.second){
            auto source_port_id = ac.source_id;
            auto target_port_id = ac.target_id;

            bool is_source_delegate = external_port_id_map_.count(source_port_id) > 0;
            bool is_target_delegate = external_port_id_map_.count(target_port_id) > 0;

            auto source_component_id = graphml_parser_->GetParentNode(source_port_id);
            auto target_component_id = graphml_parser_->GetParentNode(target_port_id);

            //Port to Port
            if(!is_source_delegate && !is_target_delegate){
                if(component_replications_.count(source_component_id) && component_replications_.count(target_component_id)){
                    //Get all ComponentInstances from the Replication
                    for(auto source_component_instance_proto : component_replications_[source_component_id]){
                        const auto& source_replicate_indices = source_component_instance_proto->replicate_indices();
                        auto s_unique = GetUniqueSuffix({source_replicate_indices.begin(), source_replicate_indices.end()});
                        auto s_uid = ac.source_id + s_unique;

                        NodeManager::Port* source_port_instance_proto = 0;
                        if(port_replicate_id_map_.count(s_uid)){
                            source_port_instance_proto = port_replicate_id_map_[s_uid];
                        }else{
                            std::cerr << "Cant Find Source Port: " << s_uid << std::endl;
                            continue;
                        }

                        //If we are an inter_assembly edge, we need to connect every outeventport instance to every ineventport instance
                        if(ac.inter_assembly){
                            //Connect to all!
                            for(auto target_component_instance_proto : component_replications_[target_component_id]){
                                const auto& target_replicate_indices = target_component_instance_proto->replicate_indices();
                                auto t_unique = GetUniqueSuffix({target_replicate_indices.begin(), target_replicate_indices.end()});
                                auto t_uid = ac.target_id + t_unique;

                                auto shared_parent_id = graphml_parser_->GetSharedParent(ac.source_id, ac.target_id);
                                auto source_height = graphml_parser_->GetHeightToParent(ac.source_id, shared_parent_id) - 1;
                                auto target_height = graphml_parser_->GetHeightToParent(ac.target_id, shared_parent_id) - 1;

                                auto source_index = source_replicate_indices.size() - source_height;
                                auto target_index = target_replicate_indices.size() - target_height;

                                if(source_index > 0 && target_index > 0){
                                    auto source_replicate_index = source_replicate_indices[source_index];
                                    auto target_replicate_index = target_replicate_indices[target_index];

                                    if(source_replicate_index != target_replicate_index){
                                        continue;
                                    }
                                }

                                NodeManager::Port* target_port_instance_proto = 0;
                                if(port_replicate_id_map_.count(t_uid)){
                                    target_port_instance_proto = port_replicate_id_map_[t_uid];
                                }else{
                                    std::cerr << "Cant Find Target Port: " << t_uid << std::endl;
                                    continue;
                                }

                                //Append the connection to our list
                                if(source_port_instance_proto && target_port_instance_proto){
                                    source_port_instance_proto->add_connected_ports(t_uid);
                                    target_port_instance_proto->add_connected_ports(s_uid);
                                }
                            }
                        }else{
                            //If contained in an assembly, we only need to replicate the one outeventport to the matching replication ineventport instance
                            auto t_uid = ac.target_id + s_unique;
                            
                            NodeManager::Port* target_port_instance_proto = 0;
                            
                            if(port_replicate_id_map_.count(t_uid)){
                                target_port_instance_proto = port_replicate_id_map_[t_uid];
                            }else{
                                std::cerr << "Cant Find Target Port: " << t_uid << std::endl;
                                continue;
                            }

                            if(source_port_instance_proto && target_port_instance_proto){
                                source_port_instance_proto->add_connected_ports(t_uid);
                                target_port_instance_proto->add_connected_ports(s_uid);
                            }
                        }
                    }
                }
            }else if(is_source_delegate && !is_target_delegate){
                auto source_eport_proto = external_port_id_map_[source_port_id];

                if(component_replications_.count(target_component_id)){
                    for(auto target_component_instance_proto : component_replications_[target_component_id]){
                        const auto& replicate_indices = target_component_instance_proto->replicate_indices();
                        auto t_unique = GetUniqueSuffix({replicate_indices.begin(), replicate_indices.end()});
                        auto t_uid = target_port_id + t_unique;

                        NodeManager::Port* target_port_instance_proto = 0;
                        if(port_replicate_id_map_.count(t_uid)){
                            
                            target_port_instance_proto = port_replicate_id_map_[t_uid];
                        }else{
                            std::cerr << "Cant Find Target Port: " << t_uid << std::endl;
                            continue;
                        }

                        //Append the connection to our list
                        if(source_eport_proto && target_port_instance_proto){
                            source_eport_proto->add_connected_ports(t_uid);
                            target_port_instance_proto->add_connected_external_ports(source_port_id);
                        }
                    }
                }
            }else if(!is_source_delegate && is_target_delegate){
                auto target_eport_proto = external_port_id_map_[target_port_id];

                
                if(component_replications_.count(source_component_id)){
                    for(auto source_component_instance_proto : component_replications_[source_component_id]){
                        const auto& replicate_indices = source_component_instance_proto->replicate_indices();
                        auto s_unique = GetUniqueSuffix({replicate_indices.begin(), replicate_indices.end()});
                        auto s_uid = source_port_id + s_unique;

                        NodeManager::Port* source_port_instance_proto = 0;
                        if(port_replicate_id_map_.count(s_uid)){
                            source_port_instance_proto = port_replicate_id_map_[s_uid];
                        }else{
                            std::cerr << "Cant Find Source Port: " << s_uid << std::endl;
                            continue;
                        }

                        //Append the connection to our list
                        if(source_port_instance_proto && target_eport_proto){
                            source_port_instance_proto->add_connected_external_ports(target_port_id);
                            target_eport_proto->add_connected_ports(s_uid);
                        }
                    }
                }
            }
        }
    }
}

std::string ProtobufModelParser::GetDeployedID(const std::string& id){
    if(deployed_entities_map_.count(id)){
        return deployed_entities_map_[id];
    }
    return std::string("");
}

void ProtobufModelParser::SetAttributePb(NodeManager::Attribute& attr_pb, const std::string& type, const std::string& value){
    NodeManager::Attribute::Kind kind;
    if(type == "Integer"){
        kind = NodeManager::Attribute::INTEGER;
    }else if(type == "Boolean"){
        kind = NodeManager::Attribute::BOOLEAN;
    }else if(type == "Character"){
        kind = NodeManager::Attribute::CHARACTER;
    }else if(type == "String"){
        kind = NodeManager::Attribute::STRING;
    }else if(type == "Double"){
        kind = NodeManager::Attribute::DOUBLE;
    }else if(type == "Float"){
        kind = NodeManager::Attribute::FLOAT;
    }else{
        std::cerr << "Unhandle Graphml Attribute Type: '" << type << "'" << std::endl;
        kind = NodeManager::Attribute::STRING;
    }
    attr_pb.set_kind(kind);
    switch(kind){
        case NodeManager::Attribute::FLOAT:
        case NodeManager::Attribute::DOUBLE:{
            double double_val = 0;
            try{
                double_val = std::stod(value);
            }catch(std::invalid_argument){
                double_val = 0;
            }
            attr_pb.set_d(double_val);
            break;
        }
        case NodeManager::Attribute::CHARACTER:{
            //need to trim out ' characters
            auto char_str = value;
            char_str.erase(std::remove(char_str.begin(), char_str.end(), '\''), char_str.end());
            if(char_str.length() == 1){
                attr_pb.set_i(char_str[0]);
            }else{
                std::cerr << "Character: '" << value << "' isn't length one!" << std::endl;
            }
            break;
        }
        case NodeManager::Attribute::BOOLEAN:{
            bool val = str2bool(value);
            attr_pb.set_i(val);
            break;
        }
        case NodeManager::Attribute::INTEGER:{
            int int_val = 0;
            try{
                int_val = std::stoi(value);
            }catch(std::invalid_argument){
                int_val = 0;
            }
            attr_pb.set_i(int_val);
            break;
        }
        case NodeManager::Attribute::STRING:{
            auto str = value;
            str.erase(std::remove(str.begin(), str.end(), '"'), str.end());
            attr_pb.add_s(str);
            break;
        }
        default:
            std::cerr << "Got unhandled Attribute type: " << NodeManager::Attribute_Kind_Name(attr_pb.kind()) << std::endl;
            break;
    }
}

std::string ProtobufModelParser::GetDefinitionId(const std::string& id){
    if(definition_ids_.count(id)){
        return definition_ids_[id];
    }
    std::string def_id;


    //Get the Definition ID of the 
    for(auto e_id : definition_edge_ids_){
        auto target = graphml_parser_->GetAttribute(e_id, "target");
        auto source = graphml_parser_->GetAttribute(e_id, "source");

        if(source == id){
            def_id = target;
            break;
        }
    }

    if(!def_id.empty()){
        //Save our Definition so we don't have to recalculate it
        definition_ids_[id] = def_id;
    }
    return def_id;
}

std::string ProtobufModelParser::GetRecursiveDefinitionId(const std::string& id){
    auto current_id = id;
    while(true){
        auto def_id = GetDefinitionId(current_id);

        if(def_id == current_id || def_id == ""){
            return current_id;
        }else{
            current_id = def_id;
        }
    }
}

std::string ProtobufModelParser::GetAggregateId(const std::string& id){
    if(aggregate_ids_.count(id)){
        return aggregate_ids_[id];
    }

    std::string agg_id;

    for(auto e_id : aggregate_edge_ids_){
        auto target = graphml_parser_->GetAttribute(e_id, "target");
        auto source = graphml_parser_->GetAttribute(e_id, "source");

        if(source == id){
            agg_id = target;
            break;
        }
    }

    if(!agg_id.empty()){
        aggregate_ids_[id] = agg_id;
    }
    return agg_id;
}

std::string ProtobufModelParser::GetImplId(const std::string& id){
    std::string impl_id;

    //Check for a definition first.
    std::string def_id = GetDefinitionId(id);

    if(def_id.empty()){
        def_id = id;
    }

    //Get the kind of Impl
    auto impl_kind = graphml_parser_->GetDataValue(def_id, "kind") + "Impl";

    //Get the Definition ID of the 
    for(auto e_id : definition_edge_ids_){
        auto source = graphml_parser_->GetAttribute(e_id, "source");
        auto target = graphml_parser_->GetAttribute(e_id, "target");
        auto source_kind = graphml_parser_->GetDataValue(source, "kind");

        if(target == def_id && source_kind == impl_kind){
            impl_id = source;
            break;
        }
    }
    return impl_id;

}

NodeManager::Port::Kind ProtobufModelParser::GetPortKind(const std::string& kind){
    if(kind == "PublisherPortInstance"){
        return NodeManager::Port::PUBLISHER;
    } else if(kind == "SubscriberPortInstance"){
        return NodeManager::Port::SUBSCRIBER;
    } else if(kind == "PeriodicPortInstance"){
        return NodeManager::Port::PERIODIC;
    } else if(kind == "RequesterPortInstance"){
        return NodeManager::Port::REQUESTER;
    } else if(kind == "ReplierPortInstance"){
        return NodeManager::Port::REPLIER;
    }
    std::cerr << "INVALID PORT KIND: " << kind << std::endl;
    return NodeManager::Port::NO_KIND;
}

NodeManager::ExternalPort::Kind ProtobufModelParser::GetExternalPortKind(const std::string& kind){
    if(kind == "PubSubPortDelegate"){
        return NodeManager::ExternalPort::PUBSUB;
    } else if(kind == "ServerPortDelegate"){
        return NodeManager::ExternalPort::SERVER;
    }
    else if(kind == "RequestPortDelegate"){
        return NodeManager::ExternalPort::SERVER;
    }
    else if(kind == "ReplyPortDelegate"){
        return NodeManager::ExternalPort::SERVER;
    }
    return NodeManager::ExternalPort::NO_KIND;
}

//Converts std::string to lower
std::string ProtobufModelParser::to_lower(const std::string& s){
    auto str = s;
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

//Convert unweildy strings to bool
bool ProtobufModelParser::str2bool(std::string str) {
    try{
        return std::stoi(str);
    }catch(std::invalid_argument){

    }

    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    std::istringstream is(str);
    bool b;
    is >> std::boolalpha >> b;
    int int_val = 0;
    return b;
}

//build port guid based on fully qualified path to port "experiment_id.(assembly_names)xN.component_name.port_name"
std::string ProtobufModelParser::BuildPortGuid(const std::string& port_id){
    std::string out = "." + graphml_parser_->GetDataValue(port_id, "label");

    std::string parent_id = graphml_parser_->GetParentNode(port_id);

    while(true){
        if(parent_id.empty()){
            break;
        }
        if(deployed_entities_map_.count(parent_id)){
            out.insert(0, "." + graphml_parser_->GetDataValue(parent_id, "label"));
            parent_id = graphml_parser_->GetParentNode(parent_id);
        }
        else{
            if(graphml_parser_->GetDataValue(parent_id, "kind") == "ComponentAssembly"){
                out.insert(0, "." + graphml_parser_->GetDataValue(parent_id, "label"));
            }
            parent_id = graphml_parser_->GetParentNode(parent_id);
        }
    }

    out.insert(0, experiment_id_);

    return out;
}

NodeManager::Node::NodeType ProtobufModelParser::GetHardwareItemKind(const std::string& kind){
    if(kind == "HardwareNode"){
        return NodeManager::Node::HARDWARE_NODE;
    } else if(kind == "HardwareCluster"){
        return NodeManager::Node::HARDWARE_CLUSTER;
    } else{
        std::cerr << "INVALID PORT KIND: " << kind << std::endl;
        //TODO: Throw exception??
        return NodeManager::Node::HARDWARE_NODE;
    }
}


void ProtobufModelParser::FillProtobufAttributes(::google::protobuf::Map< ::std::string, ::NodeManager::Attribute >* attrs, const std::string& parent_id, const std::string& unique_id_suffix){
    for(const auto& attribute_id : graphml_parser_->FindImmediateChildren("AttributeInstance", parent_id)){
        const auto& attr_label = graphml_parser_->GetDataValue(attribute_id, "label");
        
        auto& attr_pb = NodeManager::InsertAttribute(attrs, attr_label, NodeManager::Attribute::STRING);
        attr_pb.mutable_info()->set_id(attribute_id + unique_id_suffix);

        SetAttributePb(attr_pb, graphml_parser_->GetDataValue(attribute_id, "type"), attribute_value_map_[attribute_id]);
    }
}


std::list<std::string> ProtobufModelParser::GetNamespace(const std::string& id){
    std::list<std::string> namespace_list;
    auto current_id = id;

    while(current_id.size()){
        const auto& kind = graphml_parser_->GetDataValue(current_id, "kind");
        if(kind == "Namespace"){
            const auto& label = graphml_parser_->GetDataValue(current_id, "label");
            namespace_list.emplace_front(label);
        }
        current_id = graphml_parser_->GetParentNode(current_id);
    }
    return namespace_list;
}

std::list<std::string> ProtobufModelParser::ComponentReplication::GetReplicationLocation() const{
    std::list<std::string> locations;

    auto current = parent;
    //Ignore the Assemblies fam
    while(current && current->parent){
        locations.push_front(current->name);
        current = current->parent;
    }
    return locations;
}

std::list<int> ProtobufModelParser::ComponentReplication::GetReplicationIndices() const{
    std::list<int> replications;
    auto current = parent;
    //Ignore the Assemblies fam
    while(current && current->parent){
        replications.push_front(current->replication_index);
        current = current->parent;
    }
    return replications;
}


std::string ProtobufModelParser::ComponentReplication::GetUniqueId() const{
    std::stringstream ss;
    ss << component_instance_id << GetUniqueIdSuffix();
    return ss.str();
}

std::string ProtobufModelParser::GetUniqueSuffix(const std::vector<int>& indices){
    std::stringstream ss;
    for(const auto& id : indices){
        ss << "_" << id;
    }
    return ss.str();
}


std::string ProtobufModelParser::ComponentReplication::GetUniqueIdSuffix() const{
    const auto& indices = GetReplicationIndices();

    return GetUniqueSuffix({indices.begin(), indices.end()});
}

NodeManager::Middleware ProtobufModelParser::ParseMiddleware(const std::string& middleware_str) const{
    auto middleware = NodeManager::NO_MIDDLEWARE;
    if(!NodeManager::Middleware_Parse(middleware_str, &middleware)){
        std::cerr << "Cannot parse middleware: " << middleware_str << std::endl;
    }
    return middleware;
}

    

NodeManager::Port* ProtobufModelParser::ConstructPubSubPortPb(const std::string& port_id, const std::string& unique_id_suffix){
    auto port_pb = new NodeManager::Port();
    auto aggregate_id = GetAggregateId(GetDefinitionId(port_id));
            
    auto port_info_pb = port_pb->mutable_info();
    port_info_pb->set_id(port_id + unique_id_suffix);
    port_info_pb->set_name(graphml_parser_->GetDataValue(port_id, "label"));
    port_info_pb->set_type(graphml_parser_->GetDataValue(aggregate_id, "label"));

    //Copy in the new namespaces
    for(const auto& ns : GetNamespace(aggregate_id)){
        port_info_pb->add_namespaces(ns);
    }

    port_pb->set_kind(GetPortKind(graphml_parser_->GetDataValue(port_id, "kind")));

    //Set Middleware
    auto middleware = ParseMiddleware(graphml_parser_->GetDataValue(port_id, "middleware"));
    port_pb->set_middleware(middleware);

    
    if(middleware == NodeManager::RTI || middleware == NodeManager::OSPL || middleware == NodeManager::QPID){
        //Set the topic_name
        const auto& topic_name = graphml_parser_->GetDataValue(port_id, "topic_name");
        NodeManager::SetStringAttribute(port_pb->mutable_attributes(), "topic_name", topic_name);
    }
    return port_pb;
}


NodeManager::Port* ProtobufModelParser::ConstructReqRepPortPb(const std::string& port_id, const std::string& unique_id_suffix){
    auto port_pb = new NodeManager::Port();
    
    auto server_id = GetAggregateId(GetDefinitionId(port_id));

    auto port_info_pb = port_pb->mutable_info();

    port_info_pb->set_id(port_id + unique_id_suffix);
    port_info_pb->set_name(graphml_parser_->GetDataValue(port_id, "label"));
    port_info_pb->set_type(graphml_parser_->GetDataValue(server_id, "label"));

    //Copy in the new namespaces
    for(const auto& ns : GetNamespace(server_id)){
        port_info_pb->add_namespaces(ns);
    }

    port_pb->set_kind(GetPortKind(graphml_parser_->GetDataValue(port_id, "kind")));

    //Set Middleware
    auto middleware = ParseMiddleware(graphml_parser_->GetDataValue(port_id, "middleware"));
    port_pb->set_middleware(middleware);

    if(middleware == NodeManager::QPID){
        //Set the topic_name
        const auto& topic_name = graphml_parser_->GetDataValue(port_id, "topic_name");
        NodeManager::SetStringAttribute(port_pb->mutable_attributes(), "topic_name", topic_name);
    }
    return port_pb;
}


NodeManager::Port* ProtobufModelParser::ConstructPeriodicPb(const std::string& port_id, const std::string& unique_id_suffix){
    auto port_pb = new NodeManager::Port();
    auto port_info_pb = port_pb->mutable_info();
    
    port_info_pb->set_id(port_id + unique_id_suffix);
    port_info_pb->set_name(graphml_parser_->GetDataValue(port_id, "label"));
    port_pb->set_kind(NodeManager::Port::PERIODIC);

    //Fill the Attributes
    FillProtobufAttributes(port_pb->mutable_attributes(), port_id, unique_id_suffix);
    return port_pb;
}


NodeManager::Worker* ProtobufModelParser::ConstructWorkerPb(const std::string& worker_id, const std::string& unique_id_suffix){
    const auto& class_type = graphml_parser_->GetDataValue(worker_id, "type");
    
    //Ignore vector operations
    if(class_type == "Vector_Operations" || class_type == "Component_Info"){
        return 0;
    }
    
    auto worker_pb = new NodeManager::Worker();
    auto worker_info_pb = worker_pb->mutable_info();

    worker_info_pb->set_id(worker_id + unique_id_suffix);
    worker_info_pb->set_name(graphml_parser_->GetDataValue(worker_id, "label"));
    worker_info_pb->set_type(graphml_parser_->GetDataValue(worker_id, "type"));
    
    //Fill the Attributes
    FillProtobufAttributes(worker_pb->mutable_attributes(), worker_id, unique_id_suffix);
    return worker_pb;
}

