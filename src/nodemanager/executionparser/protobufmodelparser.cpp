#include "protobufmodelparser.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <google/protobuf/util/json_util.h>

#include "graphmlparser.h"



ProtobufModelParser::ProtobufModelParser(const std::string& filename, const std::string& experiment_id){
    experiment_id_ = experiment_id;
    graphml_parser_ = std::unique_ptr<GraphmlParser>(new GraphmlParser(filename));
    is_valid_ = graphml_parser_->IsValid();
    if(is_valid_){
        pre_process_success_ = PreProcess();
        process_success_ = Process();
    }
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
std::set<std::string> ProtobufModelParser::GetTerminalSourcesByEdgeKind(const std::string& node_id, const std::string& edge_kind){
    std::set<std::string> source_ids;
    
    //Get all edges connected to node_id
    for(const auto& edge_id : entity_edge_ids_[node_id]){
        if(edge_kind == graphml_parser_->GetDataValue(edge_id, "kind")){
            //If node provided is the target of this edge
            if(node_id == graphml_parser_->GetAttribute(edge_id, "target")){
                //Get the source of this edge
                const auto& edge_source = graphml_parser_->GetAttribute(edge_id, "source");
                
                //Get the edges originating from the source
                const auto& connected_sources = GetTerminalSourcesByEdgeKind(edge_source, edge_kind);

                //If we have edges originating from the source, append them
                if(connected_sources.size()){
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

    component_ids_ = graphml_parser_->FindNodes("Component");

    component_instance_ids_ = graphml_parser_->FindNodes("ComponentInstance");
    component_assembly_ids_ = graphml_parser_->FindNodes("ComponentAssembly");
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

    //Calculate replication
    for(const auto& component_instance_id: component_instance_ids_){

        int replication = 1;
        auto parent_id = graphml_parser_->GetParentNode(component_instance_id);

        bool deployed = false;
        int count = 0;
        while(true){
            count++;
            if(parent_id.empty()){
                break;
            }
            if(deployed_entities_map_.count(component_instance_id)){
                break;
            }
            else if(deployed_entities_map_.count(parent_id)){
                if(!deployed){
                    replication *= std::stoi(graphml_parser_->GetDataValue(parent_id, "replicate_count"));
                    deployed_entities_map_[component_instance_id] = deployed_entities_map_[parent_id];
                }
                //set deployed flag s.t. we stop calculating replication and updating deployment location
                deployed = true;
                parent_id = graphml_parser_->GetParentNode(parent_id);
            }
            else{
                if(graphml_parser_->GetDataValue(parent_id, "kind") == "ComponentAssembly"){
                    if(!deployed){
                        replication *= std::stoi(graphml_parser_->GetDataValue(parent_id, "replicate_count"));
                    }
                }
                parent_id = graphml_parser_->GetParentNode(parent_id);
            }
        }
        replication_map_[component_instance_id] = replication;
    }

    //Construct a Deployment Map which points EventPort - > QOSProfile
    for(const auto& e_id: qos_edge_ids_){
        auto source_id = graphml_parser_->GetAttribute(e_id, "source");
        auto target_id = graphml_parser_->GetAttribute(e_id, "target");
        entity_qos_map_[source_id] = target_id;
    }


    //Populate port connection map using recurse edge function to follow port delegates through
    for(const auto& port_id : graphml_parser_->FindNodes("InEventPortInstance")){
        const auto& target_id = port_id;
        for(auto source_id : GetTerminalSourcesByEdgeKind(target_id, "Edge_Assembly")){
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
        auto data_sources = GetTerminalSourcesByEdgeKind(attribute_id, "Edge_Data");

        auto data_value = graphml_parser_->GetDataValue(attribute_id, "value");
        if(data_sources.size() == 1){
            
            data_value = graphml_parser_->GetDataValue(*(data_sources.begin()), "value");
        }
        attribute_value_map_[attribute_id] = data_value;
    }


    //Construct port fully qualified port ids
    auto out_event_port_ids = graphml_parser_->FindNodes("OutEventPortInstance", "");
    auto in_event_port_ids = graphml_parser_->FindNodes("InEventPortInstance", "");

    for(const auto& port_id : out_event_port_ids){
        port_guid_map_[port_id] = BuildPortGuid(port_id);
    }

    for(const auto& port_id : in_event_port_ids){
        port_guid_map_[port_id] = BuildPortGuid(port_id);
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

        auto ip_attr = node->add_attributes();
        auto ip_attr_info = ip_attr->mutable_info();
        ip_attr_info->set_name("ip_address");
        ip_attr->set_kind(NodeManager::Attribute::STRING);
        ip_attr->add_s(graphml_parser_->GetDataValue(hardware_id, "ip_address"));
        node_message_map_[hardware_id] = node;
    }
    return true;
}

bool ProtobufModelParser::Process(){
    if(!graphml_parser_){
        return false;
    }

    control_message_ = new NodeManager::ControlMessage();
    control_message_->set_experiment_id(experiment_id_);

    //populate environment message's hardware fields. Fills local node_message_map_
    ParseHardwareItems(control_message_);

    //Construct and fill component instances
    for(const auto& component_id : component_instance_ids_){
        for(int i = 0; i < replication_map_[component_id]; i++){
            std::string unique_id;

            if(i > 0){
                unique_id = "_" + std::to_string(i);
            }

            auto component_uid = graphml_parser_->GetAttribute(component_id, "id") + unique_id;

            //Get id of deployed node
            auto parent_id = graphml_parser_->GetParentNode(component_id);
            auto hardware_id = deployed_entities_map_[component_id];

            //Get hardware node pb message that this component is deployed to
            NodeManager::Node* node_pb = 0;
            if(node_message_map_.count(hardware_id)){
                node_pb = node_message_map_.at(hardware_id);
            }

            //Move to next component_instance if we're missing anything
            if(hardware_id.empty() || component_id.empty() || !node_pb){
                continue;
            }

            //Set component info
            auto component_pb = node_pb->add_components();
            component_pb->set_replicate_id(i);
            auto component_info_pb = component_pb->mutable_info();
            component_info_pb->set_id(component_uid);
            std::string component_name = graphml_parser_->GetDataValue(component_id, "label") + unique_id;
            component_info_pb->set_name(component_name);
            component_info_pb->set_type(graphml_parser_->GetDataValue(component_id, "type"));

            //Fill the Attributes
            FillProtobufAttributes(component_pb->mutable_attributes(), component_id, unique_id);

            //Find all ports in/out of component instance
            auto port_ids = graphml_parser_->FindImmediateChildren("OutEventPortInstance", component_id);
            auto in_port_ids = graphml_parser_->FindImmediateChildren("InEventPortInstance", component_id);
            auto periodic_ids = graphml_parser_->FindImmediateChildren("PeriodicEventInstance", component_id);
            auto class_instance_ids = graphml_parser_->FindImmediateChildren("ClassInstance", component_id);
            port_ids.insert(port_ids.end(), in_port_ids.begin(), in_port_ids.end());

            //Set port info
            for(const auto& port_id : port_ids){
                auto aggregate_id = GetAggregateId(GetDefinitionId(port_id));
                auto port_pb = component_pb->add_ports();
                auto port_info_pb = port_pb->mutable_info();

                port_info_pb->set_id(port_id + unique_id);
                std::string port_name = graphml_parser_->GetDataValue(port_id, "label");
                port_info_pb->set_name(port_name);
                port_info_pb->set_type(graphml_parser_->GetDataValue(aggregate_id, "label"));

                //Copy in the new namespaces
                for(auto ns : GetNamespace(aggregate_id)){
                    port_pb->add_namespaces(ns);
                }

                if(graphml_parser_->GetDataValue(aggregate_id, "port_visibility") == "public"){
                    port_pb->set_visibility(NodeManager::EventPort::PUBLIC);
                }
                else{
                    port_pb->set_visibility(NodeManager::EventPort::PRIVATE);
                }

                port_pb->set_port_guid(port_guid_map_[port_id]);

                port_pb->set_kind(GetPortKind(graphml_parser_->GetDataValue(port_id, "kind")));

                port_replicate_id_map_[port_id + unique_id] = port_pb;

                //Set Middleware
                std::string mw_string = graphml_parser_->GetDataValue(port_id, "middleware");
                NodeManager::EventPort::Middleware mw;
                if(!NodeManager::EventPort_Middleware_Parse(mw_string, &mw)){
                    std::cerr << "Cannot parse middleware: " << mw_string << std::endl;
                }
                port_pb->set_middleware(mw);

                //Set the topic_name
                std::string topic_name;
                topic_name = graphml_parser_->GetDataValue(port_id, "topicName");

                if(!topic_name.empty()){
                    auto topic_pb = port_pb->add_attributes();
                    auto topic_info_pb = topic_pb->mutable_info();
                    topic_info_pb->set_name("topic_name");
                    topic_pb->set_kind(NodeManager::Attribute::STRING);
                    //Only set if we actually have a topic name
                    topic_pb->add_s(topic_name);
                }
            }

            //Handle Periodic Events
            for(const auto& periodic_id : periodic_ids){
                std::string p_uid = component_uid + "_" + periodic_id;
                auto port_pb = component_pb->add_ports();
                auto port_info_pb = port_pb->mutable_info();

                port_info_pb->set_id(p_uid);
                port_info_pb->set_name(graphml_parser_->GetDataValue(periodic_id, "label"));
                port_pb->set_kind(GetPortKind(graphml_parser_->GetDataValue(periodic_id, "kind")));

                //Fill the Attributes
                FillProtobufAttributes(port_pb->mutable_attributes(), periodic_id, unique_id);
            }

            //Handle Class Instances
            for(const auto& class_instance_id : class_instance_ids){
                std::string class_uid = component_uid + "_" + class_instance_id;
                auto class_pb = component_pb->add_workers();
                auto class_info_pb = class_pb->mutable_info();

                class_info_pb->set_id(class_uid);
                class_info_pb->set_name(graphml_parser_->GetDataValue(class_instance_id, "label"));
                class_info_pb->set_type(graphml_parser_->GetDataValue(class_instance_id, "type"));
                
                //Fill the Attributes
                FillProtobufAttributes(class_pb->mutable_attributes(), class_instance_id, unique_id);
            }

            component_replications_[component_id].push_back(component_pb);
        }
    }
    CalculateReplication();
    return true;
}

void ProtobufModelParser::CalculateReplication(){
    //Calculate the connections taking into account replication!
    for(const auto& m : assembly_map_){
        for(const auto& ac : m.second){
            auto source_port_id = ac.source_id;
            auto target_port_id = ac.target_id;

            auto source_component_id = graphml_parser_->GetParentNode(source_port_id);
            auto target_component_id = graphml_parser_->GetParentNode(target_port_id);


            if(component_replications_.count(source_component_id) && component_replications_.count(source_component_id)){
                //Get all ComponentInstances from the Replication
                for(auto source_component_instance_proto : component_replications_[source_component_id]){
                    auto s_unique = GetUniquePrefix(source_component_instance_proto->replicate_id());
                    auto s_uid = ac.source_id + s_unique;

                    NodeManager::EventPort* source_port_instance_proto = 0;
                    if(port_replicate_id_map_.count(s_uid)){
                        source_port_instance_proto = port_replicate_id_map_[s_uid];
                    }else{

                    }

                    //If we are an inter_assembly edge, we need to connect every outeventport instance to every ineventport instance
                    if(ac.inter_assembly){
                        //Connect to all!
                        for(auto target_component_instance_proto : component_replications_[target_component_id]){
                            auto t_unique = GetUniquePrefix(target_component_instance_proto->replicate_id());
                            auto t_uid = ac.target_id + t_unique;

                            NodeManager::EventPort* target_port_instance_proto = 0;
                            if(port_replicate_id_map_.count(t_uid)){
                                target_port_instance_proto = port_replicate_id_map_[t_uid];
                            }else{

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
                        NodeManager::EventPort* target_port_instance_proto = 0;
                        if(port_replicate_id_map_.count(t_uid)){
                            target_port_instance_proto = port_replicate_id_map_[t_uid];
                        }else{

                        }

                        if(source_port_instance_proto && target_port_instance_proto){
                            source_port_instance_proto->add_connected_ports(t_uid);
                            target_port_instance_proto->add_connected_ports(s_uid);
                        }
                    }
                }
            }
        }
    }
}


// std::string connected_guid;
// if(graphml_parser_->GetDataValue(connected_id, "port_visibility") == "public"){
//     connected_guid = port_guid_map_[connected_id];
// }
// else{
//     connected_guid = connected_id;
// }
// port_pb->add_connected_ports(connected_guid);

std::string ProtobufModelParser::GetDeployedID(const std::string& id){
    std::string d_id;

    if(deployed_entities_map_.count(id)){
        d_id = deployed_entities_map_[id];
    }
    return d_id;
}

void ProtobufModelParser::SetAttributePb(NodeManager::Attribute* attr_pb, const std::string& type, const std::string& value){
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
    attr_pb->set_kind(kind);
    switch(kind){
        case NodeManager::Attribute::FLOAT:
        case NodeManager::Attribute::DOUBLE:{
            double double_val = 0;
            try{
                double_val = std::stod(value);
            }catch(std::invalid_argument){
                double_val = 0;
            }
            attr_pb->set_d(double_val);
            break;
        }
        case NodeManager::Attribute::CHARACTER:{
            //need to trim out ' characters
            auto char_str = value;
            char_str.erase(std::remove(char_str.begin(), char_str.end(), '\''), char_str.end());
            if(char_str.length() == 1){
                attr_pb->set_i(char_str[0]);
            }else{
                std::cerr << "Character: '" << value << "' isn't length one!" << std::endl;
            }
            break;
        }
        case NodeManager::Attribute::BOOLEAN:{
            bool val = str2bool(value);
            attr_pb->set_i(val);
            break;
        }
        case NodeManager::Attribute::INTEGER:{
            int int_val = 0;
            try{
                int_val = std::stoi(value);
            }catch(std::invalid_argument){
                int_val = 0;
            }
            attr_pb->set_i(int_val);
            break;
        }
        case NodeManager::Attribute::STRING:{
            auto str = value;
            str.erase(std::remove(str.begin(), str.end(), '"'), str.end());
            attr_pb->add_s(str);
            break;
        }
        default:
            std::cerr << "Got unhandled Attribute type: " << NodeManager::Attribute_Kind_Name(attr_pb->kind()) << std::endl;
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

NodeManager::EventPort::Kind ProtobufModelParser::GetPortKind(const std::string& kind){
    if(kind == "OutEventPortInstance"){
        return NodeManager::EventPort::OUT_PORT;
    } else if(kind == "InEventPortInstance"){
        return NodeManager::EventPort::IN_PORT;
    } else if(kind == "PeriodicEventInstance"){
        return NodeManager::EventPort::PERIODIC_PORT;
    } else{
        std::cerr << "INVALID PORT KIND: " << kind << std::endl;
        //TODO: Throw exception??
        return NodeManager::EventPort::PERIODIC_PORT;
    }
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

std::string ProtobufModelParser::GetUniquePrefix(int count){
    std::string str;
    if(count > 0){

        str += "_" + std::to_string(count);
    }
    return str;
}

void ProtobufModelParser::FillProtobufAttributes(google::protobuf::RepeatedPtrField<NodeManager::Attribute>* entity, const std::string& parent_id, const std::string& unique_id_suffix){
    for(const auto& attribute_id : graphml_parser_->FindImmediateChildren("AttributeInstance", parent_id)){
        auto attr_pb = entity->Add();
        auto attr_info_pb = attr_pb->mutable_info();

        attr_info_pb->set_id(attribute_id + unique_id_suffix);
        attr_info_pb->set_name(graphml_parser_->GetDataValue(attribute_id, "label"));

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