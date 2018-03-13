#include "protobufmodelparser.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <google/protobuf/util/json_util.h>


ProtobufModelParser::ProtobufModelParser(const std::string& filename){
    graphml_parser_ = new GraphmlParser(filename);
    is_valid_ = graphml_parser_->IsValid();
    pre_process_success_ = PreProcess();
    process_success_ = Process();
}

std::string ProtobufModelParser::GetDeploymentJSON(){

    std::string output;
    google::protobuf::util::JsonPrintOptions options;
    options.add_whitespace = true;
    options.always_print_primitive_fields = false;

    google::protobuf::util::MessageToJsonString(*environment_message_, &output, options);
    return output;
}

NodeManager::EnvironmentMessage* ProtobufModelParser::EnvironmentMessage(){
    return environment_message_;
}

void ProtobufModelParser::RecurseEdge(const std::string& source_id, const std::string& current_id){
    for(auto edge_id : entity_edge_ids_[current_id]){
        auto edge_source = graphml_parser_->GetAttribute(edge_id, "source");
        auto edge_target = graphml_parser_->GetAttribute(edge_id, "target");
        auto edge_kind = graphml_parser_->GetDataValue(edge_id, "kind");

        if(edge_kind == "Edge_Assembly" && edge_source == current_id){
            auto target_kind = graphml_parser_->GetDataValue(edge_target, "kind");

            if(target_kind == "InEventPortInstance"){
                AssemblyConnection edge;
                edge.source_id = source_id;
                edge.target_id = edge_target;
                edge.inter_assembly = source_id != current_id;
                assembly_map_[source_id].push_back(edge);
            }
            else if(target_kind == "OutEventPortDelegate" || target_kind == "InEventPortDelegate"){
                RecurseEdge(source_id, edge_target);
            }
        }
    }
}

bool ProtobufModelParser::PreProcess(){
    if(!graphml_parser_){
        return false;
    }

    //Get all ids
    hardware_node_ids_ = graphml_parser_->FindNodes("HardwareNode");
    hardware_cluster_ids_ = graphml_parser_->FindNodes("HardwareCluster");
    component_ids_ = graphml_parser_->FindNodes("Component");
    component_instance_ids_ = graphml_parser_->FindNodes("ComponentInstance");
    component_impl_ids_ = graphml_parser_->FindNodes("ComponentImpl");
    component_assembly_ids_ = graphml_parser_->FindNodes("ComponentAssembly");
    model_id_ = graphml_parser_->FindNodes("Model")[0];
    model_name_ = graphml_parser_->GetDataValue(model_id_, "label");

    //Get the ID's of the edges
    deployment_edge_ids_ = graphml_parser_->FindEdges("Edge_Deployment");
    assembly_edge_ids_ = graphml_parser_->FindEdges("Edge_Assembly");
    definition_edge_ids_ = graphml_parser_->FindEdges("Edge_Definition");
    aggregate_edge_ids_ = graphml_parser_->FindEdges("Edge_Aggregate");
    qos_edge_ids_ = graphml_parser_->FindEdges("Edge_QOS");

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
        while(true){
            if(parent_id.empty()){
                break;
            }
            if(deployed_entities_map_.count(parent_id)){
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
    for(const auto& c_id : graphml_parser_->FindNodes("OutEventPortInstance")){
        RecurseEdge(c_id, c_id);
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

bool ProtobufModelParser::ParseHardwareItems(NodeManager::EnvironmentMessage* environment_message){

    for(const auto& cluster_id : hardware_cluster_ids_){
        auto cluster = environment_message->add_hardware_items();
        cluster->set_type(GetHardwareItemKind(graphml_parser_->GetDataValue(cluster_id, "kind")));
        cluster->mutable_info()->set_id(cluster_id);
        cluster->mutable_info()->set_name(graphml_parser_->GetDataValue(cluster_id, "label"));

        node_message_map_[cluster_id] = cluster;
    }

    for(const auto& hardware_id : hardware_node_ids_){

        auto parent_id = graphml_parser_->GetParentNode(hardware_id);

        NodeManager::HardwareItem* node = 0;

        if(node_message_map_.count(parent_id)){
            node = node_message_map_[parent_id]->add_hardware_items();
        }
        else{
            node = environment_message->add_hardware_items();
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

    environment_message_ = new NodeManager::EnvironmentMessage();

    //populate environment message's hardware fields. Fills local node_message_map_
    ParseHardwareItems(environment_message_);

    std::string model_name = graphml_parser_->GetDataValue(model_id_, "label");

    //Construct and fill component instances
    for(const auto& component_id : component_instance_ids_){
        //Get id of deployed node
        auto parent_id = graphml_parser_->GetParentNode(component_id);
        auto hardware_id = deployed_entities_map_[component_id];

        //Get hardware node pb message that this component is deployed to
        NodeManager::HardwareItem* node_pb = 0;
        if(node_message_map_.count(hardware_id)){
            node_pb = node_message_map_.at(hardware_id);
        }

        //Move to next component_instance if we're missing anything
        if(hardware_id.empty() || component_id.empty() || !node_pb){
            continue;
        }

        //Set component info
        auto component_pb = node_pb->add_components();
        component_pb->set_replication(replication_map_[component_id]);
        auto component_info_pb = component_pb->mutable_info();
        component_info_pb->set_id(graphml_parser_->GetAttribute(component_id, "id"));
        std::string component_name = graphml_parser_->GetDataValue(component_id, "label");
        component_info_pb->set_name(component_name);
        component_info_pb->set_type(graphml_parser_->GetDataValue(component_id, "type"));

        //Set component attributes
        auto attribute_ids = graphml_parser_->FindNodes("AttributeInstance", component_id);
        for(const auto& attribute_id : attribute_ids){
            auto attr_pb = component_pb->add_attributes();
            auto attr_info_pb = attr_pb->mutable_info();

            attr_info_pb->set_id(attribute_id);
            attr_info_pb->set_name(graphml_parser_->GetDataValue(attribute_id, "label"));

            SetAttributePb(attr_pb, graphml_parser_->GetDataValue(attribute_id, "type"), 
                                    graphml_parser_->GetDataValue(attribute_id, "value"));

        }

        //Find all ports in/out of component instance
        auto port_ids = graphml_parser_->FindNodes("OutEventPortInstance", component_id);
        auto in_port_ids = graphml_parser_->FindNodes("InEventPortInstance", component_id);
        auto periodic_ids = graphml_parser_->FindNodes("PeriodicEvent", component_id);
        port_ids.insert(port_ids.end(), in_port_ids.begin(), in_port_ids.end());
        port_ids.insert(port_ids.end(), periodic_ids.begin(), periodic_ids.end());

        //Set port info
        for(const auto& port_id : port_ids){
            auto aggregate_id = GetAggregateId(GetDefinitionId(port_id));
            auto port_pb = component_pb->add_ports();
            auto port_info_pb = port_pb->mutable_info();

            port_info_pb->set_id(port_id);
            std::string port_name = graphml_parser_->GetDataValue(port_id, "label");
            port_info_pb->set_name(port_name);
            port_info_pb->set_type(graphml_parser_->GetDataValue(aggregate_id, "label"));
            port_pb->set_namespace_name(graphml_parser_->GetDataValue(aggregate_id, "namespace"));

            if(graphml_parser_->GetDataValue(aggregate_id, "port_visibility") == "public"){
                port_pb->set_visibility(NodeManager::EventPort::PUBLIC);
            }
            else{
                port_pb->set_visibility(NodeManager::EventPort::PRIVATE);
            }

            port_pb->set_port_guid(port_guid_map_[port_id]);

            port_pb->set_kind(GetPortKind(graphml_parser_->GetDataValue(port_id, "kind")));

            //Middleware ports
            if(port_pb->kind() != NodeManager::EventPort::PERIODIC_PORT){

                //Fill connected port mapping
                if(assembly_map_.count(port_id)){
                    for(const auto& assembly_connection : assembly_map_[port_id]){
                        std::string connected_id = assembly_connection.target_id;

                        std::string connected_guid;
                        if(graphml_parser_->GetDataValue(connected_id, "port_visibility") == "public"){
                            connected_guid = port_guid_map_[connected_id];
                        }
                        else{
                            connected_guid = connected_id;
                        }
                        port_pb->add_connected_ports(connected_guid);
                    }
                }

                //Set Middleware
                std::string mw_string = graphml_parser_->GetDataValue(port_id, "middleware");
                NodeManager::EventPort::Middleware mw;
                if(!NodeManager::EventPort_Middleware_Parse(mw_string, &mw)){
                    std::cout << "Cannot parse middleware: " << mw_string << std::endl;
                }
                port_pb->set_middleware(mw);

                //Set middleware attributes
                bool is_rti = mw == NodeManager::EventPort::RTI;
                bool is_ospl = mw == NodeManager::EventPort::OSPL;
                bool is_qpid = mw == NodeManager::EventPort::QPID;
                bool is_zmq = mw == NodeManager::EventPort::ZMQ;

                bool is_outport = port_pb->kind() == NodeManager::EventPort::OUT_PORT;
                bool is_inport = port_pb->kind() == NodeManager::EventPort::IN_PORT;


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
            //Periodic ports
            else{
                try{
                    double freq = std::stod(graphml_parser_->GetDataValue(port_id, "frequency"));
                    auto freq_pb = port_pb->add_attributes();
                    auto freq_info_pb = freq_pb->mutable_info();

                    freq_info_pb->set_name("frequency");
                    freq_pb->set_kind(NodeManager::Attribute::DOUBLE);
                    freq_pb->set_d(freq);
                }
                catch(std::exception& ex){
                    std::cout << "Could not parse periodic event port frequency " << ex.what() << std::endl;
                }
            }
        }
    }
}

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
    } else if(kind == "PeriodicEvent"){
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

//build port guid based on fully qualified path to port "model_name.(assembly_names)xN.component_name.port_name"
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

    out.insert(0, model_name_);

    return out;
}

NodeManager::HardwareItem::HardwareType ProtobufModelParser::GetHardwareItemKind(const std::string& kind){
    if(kind == "HardwareNode"){
        return NodeManager::HardwareItem::HARDWARE_NODE;
    } else if(kind == "HardwareCluster"){
        return NodeManager::HardwareItem::HARDWARE_CLUSTER;
    } else{
        std::cerr << "INVALID PORT KIND: " << kind << std::endl;
        //TODO: Throw exception??
        return NodeManager::HardwareItem::HARDWARE_NODE;
    }
}
