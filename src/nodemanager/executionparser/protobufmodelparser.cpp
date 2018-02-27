#include "protobufmodelparser.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <google/protobuf/util/json_util.h>

ProtobufModelParser::ProtobufModelParser(const std::string& filename){
    graphml_parser_ = new GraphmlParser(filename);
    success_ = graphml_parser_->IsValid() && PreProcess() && Process();
}

std::string ProtobufModelParser::GetDeploymentJSON(){

    std::string output;

    MessageToJsonString(control_message_, &output, JsonPrintOptions(){true, true});

    return output;
}


NodeManager::ControlMessage* ProtobufModelParser::GetControlMessage(){
    return control_message_;
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

    //Construct a Deployment Map which points EventPort - > QOSProfile
    for(const auto& e_id: qos_edge_ids_){
        auto source_id = graphml_parser_->GetAttribute(e_id, "source");
        auto target_id = graphml_parser_->GetAttribute(e_id, "target");
        entity_qos_map_[source_id] = target_id;
    }

    for(const auto& c_id : graphml_parser_->FindNodes("OutEventPortInstance")){
        RecurseEdge(c_id, c_id);
    }

    hardware_node_ids_ = graphml_parser_->FindNodes("HardwareNode");
    hardware_cluster_ids_ = graphml_parser_->FindNodes("HardwareCluster");
    component_ids_ = graphml_parser_->FindNodes("Component");
    component_instance_ids_ = graphml_parser_->FindNodes("ComponentInstance");
    component_impl_ids_ = graphml_parser_->FindNodes("ComponentImpl");
    component_assembly_ids_ = graphml_parser_->FindNodes("ComponentAssembly");


}

bool ProtobufModelParser::Process(){
    if(!graphml_parser_){
        return false;
    }
    //Construct proto to fill

    control_message_ = new NodeManager::ControlMessage();

    //Construct and fill hardware node fields
    for(const auto& h_id : hardware_node_ids_){
        auto node = control_message_->add_nodes();
        node->mutable_info()->set_id(h_id);
        node->mutable_info()->set_name(graphml_parser_->GetAttribute(h_id, "name"));
        node_message_map_[h_id] = node;
    }


    //TODO:replication
    //TODO:replication
    //TODO:replication
    //TODO:replication
    //TODO:replication
    //TODO:replication
    //TODO:replication

    //Construct and fill component instances
    for(const auto& component_id : component_instance_ids_){
        //Get id of deployed node
        auto hardware_id = deployed_entities_map_[component_id];

        //Get hardware node pb message that this component is deployed to
        NodeManager::Node* node_pb = 0;
        if(node_message_map_.count(hardware_id)){
            node_pb = node_message_map_.at(hardware_id);
        }

        //Continue if we're missing anything
        if(hardware_id.empty() || component_id.empty() || !node_pb){
            continue;
        }

        //Set component info
        auto component_pb = node_pb->add_components();
        auto component_info_pb = component_pb->mutable_info();
        //TODO: deal with this re. replication
        component_info_pb->set_id(graphml_parser_->GetDataValue(component_id, "id"));
        std::string component_name = graphml_parser_->GetDataValue(component_id, "label"));
        component_info_pb->set_name(component_name);
        component_info_pb->set_type(graphml_parser_->GetDataValue(component_id, "type"));

        //Set component attributes
        auto attribute_ids = graphml_parser_->FindNodes("AttributeInstance", component_id);
        for(const auto& attribute_id : attribute_ids){
            auto attr_pb = component_pb->add_attributes();
            auto attr_info_pb = attr_pb->mutable_info();

            //TODO: deal with this re. replication
            attr_info_pb->set_id(attribute_id);
            attr_info_pb->set_name(graphml_parser_->GetDataValue(attribute_id, "label"));

            SetAttributePb(attr_pb, graphml_parser_->GetDataValue(attribute_id, "type"), 
                                    graphml_parser_->GetDataValue(attribute_id, "value"));

        }

        //Find all ports in/out of component instance
        auto port_ids = graphml_parser_->FindNodes("OutEventPortInstance", component_id);
        auto in_port_ids = graphml_parser_->FindNodes("InEventPortInstance", component_id);
        port_ids.insert(port_ids.end(), in_port_ids.begin(), in_port_ids.end());

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

            NodeManager::EventPort::Kind kind;
            std::string kind_str = graphml_parser_->GetDataValue(port_id, "kind");
            port_pb->set_kind(GetPortKind(graphml_parser_->GetDataValue(port_id, "kind")));

            //Middleware ports
            if(port_pb->kind() != NodeManager::EventPort::PERIODIC_PORT){
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


                if(is_rti || is_ospl || is_qpid){
                    //Set the topic_name
                    auto topic_pb = port_pb->add_attributes();
                    auto topic_info_pb = topic_pb->mutable_info();
                    topic_info_pb->set_name("topic_name");
                    topic_pb->set_kind(NodeManager::Attribute::STRING);
                    std::string topic_name = graphml_parser_->GetDataValue(port_id, "topicName");
                    //Only set if we actually have a topic name
                    if(!topic_name.empty()){
                        topic_pb->add_s(); 
                    }
                }
                if(is_rti || is_ospl){
                    //Set the domain_id
                    auto domain_pb = port_pb->add_attributes();
                    auto domain_info_pb = domain_pb->mutable_info();
                    domain_info_pb->set_name("domain_id");
                    domain_pb->set_kind(NodeManager::Attribute::INTEGER);
                    domain_pb->set_i(0);
                    
                    //QOS Profile + profile Path
                    auto qos_name_pb = port_pb->add_attributes();
                    auto qos_name_info_pb = qos_name_pb->mutable_info();
                    
                    auto qos_path_pb = port_pb->add_attributes();
                    auto qos_path_info_pb = qos_path_pb->mutable_info();

                    qos_name_info_pb->set_name("qos_profile_name");
                    qos_name_pb->set_kind(NodeManager::Attribute::STRING);

                    qos_path_info_pb->set_name("qos_profile_path");
                    qos_path_pb->set_kind(NodeManager::Attribute::STRING);

                    if(entity_qos_map_.count(port_id)){
                        auto qos_id = entity_qos_map_[port_id];
                        std::string qos_profile_name_str = graphml_parser_->GetDataValue(qos_id, "label");
                        qos_name_pb->add_s(qos_profile_name_str);
                        qos_path_pb->add_s("file://qos/" + to_lower(mw_string) + "/" + to_lower(qos_profile_name_str) + ".xml");
                    }

                    if(is_outport){
                        //Set publisher name
                        auto pub_name_pb = port_pb->add_attributes();
                        auto pub_name_info_pb = pub_name_pb->mutable_info();
                        pub_name_info_pb->set_name("publisher_name");
                        pub_name_pb->set_kind(NodeManager::Attribute::STRING);
                        pub_name_pb->add_s(component_name + port_name);
                    }
                    if(is_inport){
                        //Set subscriber name
                        //TODO: Allow modelling of this
                        auto sub_name_pb = port_pb->add_attributes();
                        auto sub_name_info_pb = sub_name_pb->mutable_info();
                        sub_name_info_pb->set_name("subscriber_name");
                        sub_name_pb->set_kind(NodeManager::Attribute::STRING);
                        sub_name_pb->add_s(component_name + port_name);
                    }
                }
                if(is_qpid){
                    auto broker_pb = port_pb->add_attributes();
                    auto broker_info_pb = broker_pb->mutable_info();
                    broker_info_pb->set_name("broker");
                    broker_pb->set_kind(NodeManager::Attribute::STRING);
                }

                if(is_zmq){
                    if(is_outport){
                        auto pub_addr_pb = port_pb->add_attributes();
                        auto pub_addr_info_pb = pub_addr_pb->mutable_info();
                        pub_addr_info_pb->set_name("publisher_address");

                        bool endpoint_set_flag;
                        //TODO: Check for manually set endpoint
                        if(endpoint_set_flag){
                            //set endpoint
                        }
                        pub_addr_pb->add_s();
                    }
                    if(is_inport){
                        //Construct a publisher_address list
                        auto pub_addr_pb = port_pb->add_attributes();
                        auto pub_addr_info_pb = pub_addr_pb->mutable_info();
                        pub_addr_info_pb->set_name("publisher_address");

                        bool endpoint_set_flag;
                        //TODO: check for manually set endpoint
                        if(endpoint_set_flag){

                        }

                    }
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
bool ProtobufModelParser::str2bool(const std::string& str) {
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
