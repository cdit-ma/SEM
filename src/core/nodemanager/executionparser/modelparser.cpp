#include "modelparser.h"
#include "graphmlparser.h"
#include "datatypes.h"

#include <iostream>
#include <chrono>
#include <algorithm>


//Converts std::string to lower
std::string Graphml::ModelParser::to_lower(std::string s){
    auto str = s;
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}


Graphml::ModelParser::ModelParser(const std::string filename){
    //Setup the parser
    graphml_parser_ = new GraphmlParser(filename);
    
    auto start = std::chrono::steady_clock::now();
    success = Process();
    auto end = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    //std::cout << "* Deployment Parsed In: " << ms.count() << " us" << std::endl;
}
bool Graphml::ModelParser::Process(){
    if(!graphml_parser_){
        return false;
    }

    model_ = new Model();
    std::vector<std::string> modelIdVec = graphml_parser_->FindNodes("Model");
    if(modelIdVec.size() > 0){
        model_->id = modelIdVec[0];
        model_->name = graphml_parser_->GetDataValue(model_->id, "label");
        model_->uuid = graphml_parser_->GetDataValue(model_->id, "projectUUID");
    }

    //Get the ID's of the edges
    deployment_edge_ids_ = graphml_parser_->FindEdges("Edge_Deployment");
    assembly_edge_ids_ = graphml_parser_->FindEdges("Edge_Assembly");
    definition_edge_ids_ = graphml_parser_->FindEdges("Edge_Definition");
    aggregate_edge_ids_ = graphml_parser_->FindEdges("Edge_Aggregate");
    qos_edge_ids_ = graphml_parser_->FindEdges("Edge_QOS");

    for(auto c_id : graphml_parser_->FindNodes("OutEventPortInstance")){
        RecurseEdge(c_id, c_id);
    }
        
    //Construct a Deployment Map which points ComponentInstance - > HardwareNodes
    for(auto e_id: deployment_edge_ids_){
        auto source_id = GetAttribute(e_id, "source");
        auto target_id = GetAttribute(e_id, "target");
        deployed_entities_map_[source_id] = target_id;
    }

    //Construct a Deployment Map which points EventPort - > QOSProfile
    for(auto e_id: qos_edge_ids_){
        auto source_id = GetAttribute(e_id, "source");
        auto target_id = GetAttribute(e_id, "target");
        entity_qos_map_[source_id] = target_id;
        std::cout << "port:" << GetDataValue(source_id, "label") << std::endl;
        std::cout << "qos:" << GetDataValue(target_id, "label") << std::endl;
        std::cout << std::endl;
    }


    

    //Parse HardwareClusters
    for(auto c_id : graphml_parser_->FindNodes("HardwareCluster")){
        if(!GetHardwareCluster(c_id)){
            auto cluster = new HardwareCluster();
            cluster->id = c_id;
            cluster->name = GetDataValue(c_id, "label");
            hardware_clusters_[c_id] = cluster;
        }
    }

    //Parse HardwareNodes
    for(auto n_id : graphml_parser_->FindNodes("HardwareNode")){
        if(!GetHardwareNode(n_id)){
            auto node = new HardwareNode();
            node->id = n_id;
            node->name = GetDataValue(n_id, "label");
            node->ip_address = GetDataValue(n_id, "ip_address"); 
            //Set Port Number for ZMQ Logger
            node->logan_client_port_number = node->port_count++;
            node->model_logger_port_number = node->port_count++;

            //Get the parent
            auto parent_id = graphml_parser_->GetParentNode(n_id);
            //Check if the parent is a cluster
            auto parent_cluster = GetHardwareCluster(parent_id);
            if(parent_cluster){
                node->parent_id = parent_id;
                parent_cluster->node_ids.push_back(n_id);
            }
            hardware_nodes_[n_id] = node;
        }
    }

    //Parse LoggingServers
    for(auto l_id : graphml_parser_->FindNodes("LoggingServer")){
        if(!GetLoggingServer(l_id)){
            auto server = new LoggingServer();
            auto hardware_id = GetDeployedID(l_id);
            server->database_name = GetDataValue(l_id, "database");
            
            auto hardware = GetHardwareNode(hardware_id);
            if(hardware){
                hardware->logging_server_id = l_id;
            }

            logging_servers_[l_id] = server;
        }
    }

    //Parse LoggingProfiles
    for(auto l_id : graphml_parser_->FindNodes("LoggingProfile")){
        if(!GetLoggingProfile(l_id)){
            auto profile = new LoggingProfile();
            profile->id = l_id;
            profile->mode = GetDataValue(l_id, "mode");
            profile->frequency = GetDataValue(l_id, "frequency");

            std::string processes = GetDataValue(l_id, "processes_to_log");
            std::replace(processes.begin(), processes.end(), ',', ' ');
            profile->processes = processes;

            LoggingServer* loggingServer = 0;
            
            //Find the Server this profile is connected to
            for(auto e_id : assembly_edge_ids_){
                auto src_id = graphml_parser_->GetAttribute(e_id, "source");
                if(src_id == l_id){
                    //Get the nodes this is deployed on
                    auto dst_id = graphml_parser_->GetAttribute(e_id, "target");
                    auto server = GetLoggingServer(dst_id);
                    if(server){
                        loggingServer = server;
                        break;
                    }
                }
            }
            
            //Find the profile attached to this node
            for(auto e_id : deployment_edge_ids_){
                auto src_id = graphml_parser_->GetAttribute(e_id, "source");
                if(src_id == l_id){
                    //Get the nodes this is deployed on
                    auto dst_id = graphml_parser_->GetAttribute(e_id, "target");
                    auto hardware = GetHardwareNode(dst_id);
                    if(hardware){
                        hardware->logging_profile_id = l_id;
                        if(loggingServer){
                            loggingServer->connected_hardware_ids.push_back(dst_id);
                        }
                    }
                }
            }

            logging_profiles_[l_id] = profile;
        }
    }
    

    //Parse ComponentAssembly
    for(auto c_id : graphml_parser_->FindNodes("ComponentAssembly")){
        if(!GetComponentAssembly(c_id)){
            auto assembly = new ComponentAssembly();
            assembly->id = c_id;
            
            assembly->replicate_count = std::stoi(GetDataValue(c_id, "replicate_count"));
            
            auto parent_id = graphml_parser_->GetParentNode(c_id);
            auto parent_assembly = GetComponentAssembly(parent_id);
            
            if(parent_assembly){
                //Set the parent_id if we have a parent assembly
                assembly->parent_id = parent_id;
            }
            component_assemblies_[c_id] = assembly;
        }
    }

    //Parse Component
    for(auto c_id : graphml_parser_->FindNodes("Component")){
        if(!GetComponentDefinition(c_id)){
            auto component = new Component();
            component->id = c_id;
            component->name = GetDataValue(c_id, "label");
            components_[c_id] = component;
        }
    }

    //Parse ComponentImpl
    for(auto c_id : graphml_parser_->FindNodes("ComponentImpl")){
        auto def_id = GetDefinitionId(c_id);
        auto component = GetComponentDefinition(def_id);
        
        if(component){
            //Set the Implemenation for the component
            component->implementation_id = c_id;
        }
    }

    
    //Parse ComponentInstances
    for(auto c_id : graphml_parser_->FindNodes("ComponentInstance")){
        //Default replication count of 1
        int replication_count = 1;
        
        auto parent_id = graphml_parser_->GetParentNode(c_id);
        auto c_parent_id = parent_id;

        auto def_id = GetDefinitionId(c_id);
        auto component = GetComponentDefinition(def_id);
        auto impl_id = component->implementation_id;
        

        //Get the deployed ID of this component, if it's been deployed
        std::string hardware_id = GetDeployedID(c_id);

        std::string definition_id = GetDefinitionId(c_id);
        
        while(true){
            auto component_assembly = GetComponentAssembly(c_parent_id);
            if(component_assembly){
                if(hardware_id.empty()){
                    //If we haven't been deployed yet, Check if the ComponentAssembly has been deployed
                    hardware_id = GetDeployedID(c_parent_id);
                }

                if(component_assembly->replicate_count > 0){
                    //Calculate the number of these components
                    replication_count *= component_assembly->replicate_count;
                }
                //Get next parent
                c_parent_id = component_assembly->parent_id;
            }else{
                //Terminate
                break;
            }
        }

        //Get the ids of the attributes and ports
        auto attribute_ids = graphml_parser_->FindNodes("AttributeInstance", c_id);
        auto port_ids = graphml_parser_->FindNodes("OutEventPortInstance", c_id);
        {
            auto in_port_ids = graphml_parser_->FindNodes("InEventPortInstance", c_id);
            //Combine into one list to reduce code duplication            
            port_ids.insert(port_ids.end(), in_port_ids.begin(), in_port_ids.end());
        }

        //Get the periodic events from the impl
        auto periodic_event_ids = graphml_parser_->FindNodes("PeriodicEvent", impl_id);


        HardwareCluster* cluster = GetHardwareCluster(hardware_id);
        HardwareNode* node = GetHardwareNode(hardware_id);

        auto component_replication = new ComponentReplication(); 
        component_replication->id = c_id;

        //Construct the number of nodes
        for(int i = 0; i < replication_count; i++){
            HardwareNode* deployed_node = 0;

            
            
            //Get the deployed node
            if(node){
                deployed_node = node;
            }else if(cluster){
                auto n_id = cluster->GetHardwareNode(i);
                deployed_node = GetHardwareNode(n_id);
            }

            if(!deployed_node){
                std::cout << "Component '" << c_id << "' Not Deployed" << std::endl;
                break;
            }

            //Construct a unique_id
            std::string unique_id;
            if(i > 0){
                unique_id = "_" + std::to_string(i);
            }

            auto c_uid = c_id + unique_id;
            if(!GetComponentInstance(c_uid)){
                //Construct a Component
                auto component_inst = new ComponentInstance();
                component_inst->deployed_node_id = deployed_node->id;
                component_inst->original_id = c_id;
                component_inst->id = c_uid;
                component_inst->replicate_id = i;
                //Append uniquity to label
                component_inst->name = GetDataValue(c_id, "label") + unique_id;

                

                //Attach our definitions
                if(component){
                    component_inst->type_name = component->name;
                    component->instance_ids.push_back(c_uid);
                    component_inst->definition_id = component->id;
                }

                //Parse Attributes
                for(auto a_id : attribute_ids){
                    auto a_uid = a_id + unique_id;
                    if(!GetAttribute(a_uid)){
                        auto attribute = new Attribute();
                        attribute->id = a_uid;
                        attribute->component_id = c_uid;
                        attribute->name = GetDataValue(a_id, "label");
                        attribute->type = GetDataValue(a_id, "type");
                        attribute->value = GetDataValue(a_id, "value");
                        attributes_[a_uid] = attribute;
                        
                        //Add the Attribute to the Component
                        component_inst->attribute_ids.push_back(a_uid);
                    }
                }

                //Parse In/OutEventPortInstance
                for(auto p_id: port_ids){
                    auto p_uid = p_id + unique_id;
                    if(!GetEventPort(p_uid)){
                        auto port = new EventPort();
                        port->id = p_uid;
                        port->component_id = c_uid;
                        port->name = GetDataValue(p_id, "label");
                        //TODO uniquify topic_name if it's not set
                        port->topic_name = GetDataValue(p_id, "topicName");
                        port->kind = GetDataValue(p_id, "kind");
                        port->middleware = GetDataValue(p_id, "middleware");

                        if(entity_qos_map_.count(p_id)){
                            //Set QOS to port
                            auto qos_profile_id = entity_qos_map_[p_id];
                            port->qos_profile_name = GetDataValue(qos_profile_id, "label");
                            //Relative to the library directory
                            port->qos_profile_path = "file://qos/" + to_lower(port->middleware) + "/" + to_lower(port->qos_profile_name) + ".xml";
                        }

                        auto aggregate_id = GetAggregateID(GetDefinitionId(p_id));

                        port->message_type = GetDataValue(aggregate_id, "label");
                        port->namespace_name = GetDataValue(aggregate_id, "namespace");
                        

                        //Register Only OutEventPortInstances
                        if(port->kind == "OutEventPortInstance" && deployed_node && port->middleware == "ZMQ"){
                            //Set Port Number only for ZMQ
                            port->port_number = ++deployed_node->port_count;
                            
                            //Construct a TCP Address
                            port->port_address = GetTCPAddress(deployed_node->ip_address, port->port_number);
                        }
                        event_ports_[p_uid] = port;
                        //Add the Attribute to the Component
                        component_inst->event_port_ids.push_back(p_uid);
                    }
                }


                //Parse Periodic_Events
                for(auto p_id : periodic_event_ids){
                    auto p_uid = p_id + unique_id;
                    if(!GetEventPort(p_uid)){
                        auto port = new EventPort();
                        //setup the port
                        port->id = p_uid;
                        port->component_id = c_uid;
                        //Get the original values
                        port->name = GetDataValue(p_id, "label");
                        port->kind = GetDataValue(p_id, "kind");
                        port->frequency = GetDataValue(p_id, "frequency");

                        event_ports_[p_uid] = port;
                        //Add the Attribute to the Component
                        component_inst->event_port_ids.push_back(p_uid);
                    }
                }


                if(deployed_node){
                    deployed_node->component_ids.push_back(c_uid);

                    if(first_node_){
                        first_node_ = false;
                        deployed_node->is_re_master = true;
                    }
                }
                component_instances_[c_uid] = component_inst;

                //Append to the component_replication object this nodes id
                component_replication->component_instance_ids.push_back(c_uid);
            }
        }

        component_replications_[c_id] = component_replication;
    }
    

    //Calculate the connections taking into account replication!
    for(auto m : assembly_map_){
        for(auto ac : m.second){
            auto source_port = GetEventPort(ac->source_id);
            auto target_port = GetEventPort(ac->target_id);

            ComponentReplication* source_replication = 0;
            ComponentReplication* target_replication = 0;
            if(source_port && target_port){
                source_replication = GetComponentReplication(source_port->component_id);
                target_replication = GetComponentReplication(target_port->component_id);
            }

            if(source_replication && target_replication){
                //Get all ComponentInstances from the Replication
                for(auto s_id : source_replication->component_instance_ids){
                    auto s_component = GetComponentInstance(s_id);
                    auto s_unique = GetUniquePrefix(s_component->replicate_id);
                    auto s_uid = ac->source_id + s_unique;
                    auto source_port_inst = GetEventPort(s_uid);

                    
                    //If we are an inter_assembly edge, we need to connect every outeventport instance to every ineventport instance
                    if(ac->inter_assembly){
                        //Connect to all!
                        for(auto t_id : target_replication->component_instance_ids){
                            auto t_component = GetComponentInstance(t_id);
                            auto t_unique = GetUniquePrefix(t_component->replicate_id);
                            auto t_uid = ac->target_id + t_unique;
                            auto target_port_inst = GetEventPort(t_uid);


                            //Append the connection to our list
                            if(source_port_inst && target_port_inst){
                                source_port_inst->connected_port_ids.push_back(t_uid);
                                target_port_inst->connected_port_ids.push_back(s_uid);
                            }
                        }
                    }else{
                        //If contained in an assembly, we only need to replicate the one outeventport to the matching replication ineventport instance
                        auto t_uid = ac->target_id + s_unique;
                        auto target_port_inst = GetEventPort(t_uid);


                        if(source_port_inst && target_port_inst){
                            source_port_inst->connected_port_ids.push_back(t_uid);
                            target_port_inst->connected_port_ids.push_back(s_uid);
                        }
                    }
                }
            }
        }
    }
    
    return true;

}

Graphml::HardwareNode* Graphml::ModelParser::GetHardwareNode(std::string id){
    if(hardware_nodes_.count(id)){
        return hardware_nodes_[id];
    }else{
        return 0;
    }
}

Graphml::HardwareCluster* Graphml::ModelParser::GetHardwareCluster(std::string id){
    if(hardware_clusters_.count(id)){
        return hardware_clusters_[id];
    }else{
        return 0;
    }
}

Graphml::LoggingProfile* Graphml::ModelParser::GetLoggingProfile(std::string id){
    if(logging_profiles_.count(id)){
        return logging_profiles_[id];
    }else{
        return 0;
    }
}

Graphml::LoggingServer* Graphml::ModelParser::GetLoggingServer(std::string id){
    if(logging_servers_.count(id)){
        return logging_servers_[id];
    }else{
        return 0;
    }
}

Graphml::Component* Graphml::ModelParser::GetComponentDefinition(std::string id){
    if(components_.count(id)){
        return components_[id];
    }else{
        return 0;
    }
}

Graphml::ComponentInstance* Graphml::ModelParser::GetComponentInstance(std::string id){
    if(component_instances_.count(id)){
        return component_instances_[id];
    }else{
        return 0;
    }
}

Graphml::ComponentAssembly* Graphml::ModelParser::GetComponentAssembly(std::string id){
    if(component_assemblies_.count(id)){
        return component_assemblies_[id];
    }else{
        return 0;
    }
}

Graphml::EventPort* Graphml::ModelParser::GetEventPort(std::string id){
    if(event_ports_.count(id)){
        return event_ports_[id];
    }else{
        return 0;
    }
}

Graphml::Attribute* Graphml::ModelParser::GetAttribute(std::string id){
    if(attributes_.count(id)){
        return attributes_[id];
    }else{
        return 0;
    }
}

Graphml::Edge* Graphml::ModelParser::GetEdge(std::string id){
    if(edges_.count(id)){
        return edges_[id];
    }else{
        return 0;
    }
}

Graphml::ComponentReplication* Graphml::ModelParser::GetComponentReplication(std::string id){
    if(component_replications_.count(id)){
        return component_replications_[id];
    }else{
        return 0;
    }
}

Graphml::ComponentReplication* GetComponentReplication(std::string id);


Graphml::HardwareNode* Graphml::ModelParser::GetHardwareNodeByName(std::string host_name){
    Graphml::HardwareNode* node = 0;
    for(auto n : hardware_nodes_){
        if(n.second->name == host_name){
            node = n.second;
            break;
        }
    }
    return node;
}

Graphml::HardwareNode* Graphml::ModelParser::GetHardwareNodeByIPAddress(std::string ip_address){
    Graphml::HardwareNode* node = 0;
    for(auto n : hardware_nodes_){
        if(n.second->ip_address == ip_address){
            node = n.second;
            break;
        }
    }
    return node;
}

std::string Graphml::ModelParser::GetDeployedID(std::string id){
    std::string d_id;

    if(deployed_entities_map_.count(id)){
        d_id = deployed_entities_map_[id];
    }
    return d_id;
}



std::string Graphml::ModelParser::GetDefinitionId(std::string id){
    if(definition_ids_.count(id)){
        return definition_ids_[id];
    }
    std::string def_id;

    //Get the Definition ID of the 
    for(auto e_id : definition_edge_ids_){
        auto target = GetAttribute(e_id, "target");
        auto source = GetAttribute(e_id, "source");
        
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

std::string Graphml::ModelParser::GetAggregateID(std::string id){
    if(aggregate_ids_.count(id)){
        return aggregate_ids_[id];
    }

    std::string agg_id;

    for(auto e_id : aggregate_edge_ids_){
        auto target = GetAttribute(e_id, "target");
        auto source = GetAttribute(e_id, "source");

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

std::string Graphml::ModelParser::GetImplId(std::string id){
    std::string impl_id;
    
    //Check for a definition first.
    std::string def_id = GetDefinitionId(id);
    
    if(def_id.empty()){
        def_id = id;
    }

    //Get the kind of Impl
    auto impl_kind = GetDataValue(def_id, "kind") + "Impl";

    //Get the Definition ID of the 
    for(auto e_id : definition_edge_ids_){
        auto source = GetAttribute(e_id, "source");
        auto target = GetAttribute(e_id, "target");
        auto source_kind = GetDataValue(source, "kind");

        if(target == def_id && source_kind == impl_kind){
            impl_id = source;
            break;
        }
    }
    return impl_id;

}

std::string Graphml::ModelParser::GetTCPAddress(const std::string ip, const unsigned int port_number){
    std::string addr;
    if(!ip.empty()){
        addr += "tcp://" + ip + ":" + std::to_string(port_number);
    }
    return addr;
}

void Graphml::ModelParser::RecurseEdge(std::string source_id, std::string current_id){
    for(auto e_id: assembly_edge_ids_){
        auto s_id = GetAttribute(e_id, "source");

        if(s_id == current_id){
            auto t_id = GetAttribute(e_id, "target");
            auto t_kind = GetDataValue(t_id, "kind");

            if(t_kind == "InEventPortInstance"){
                auto edge = new AssemblyConnection();
                edge->source_id = source_id;
                edge->target_id = t_id;
                edge->inter_assembly = source_id != current_id;
                assembly_map_[source_id].push_back(edge);
            }else{
                RecurseEdge(source_id, t_id);
            }
        }
    }
}

std::string Graphml::ModelParser::GetAttribute(std::string id, std::string attr_name){
    if(graphml_parser_){
        return graphml_parser_->GetAttribute(id, attr_name);
    }
    return "";
}

std::string Graphml::ModelParser::GetDataValue(std::string id, std::string key_name){
    if(graphml_parser_){
        return graphml_parser_->GetDataValue(id, key_name);
    }
    return "";
}

std::vector<Graphml::HardwareNode*> Graphml::ModelParser::GetHardwareNodes(){
    std::vector<Graphml::HardwareNode*> v;
    for(auto n : hardware_nodes_){
        v.push_back(n.second);
    }
    return v;
}

std::vector<Graphml::ComponentInstance*> Graphml::ModelParser::GetComponentInstances(){
    std::vector<Graphml::ComponentInstance*> v;
    for(auto n : component_instances_){
        v.push_back(n.second);
    }
    return v;
}


std::string Graphml::ModelParser::GetUniquePrefix(int count){
    std::string str;
    if(count > 0){

        str += "_" + std::to_string(count);
    }
    return str;
}

std::string dblquotewrap(std::string val){
    std::string quote("\"");
    return quote + val + quote;
}
std::string tab(int i = 1){
    return std::string(i, '\t');
}

std::string json_pair(std::string key, std::string val){
    return dblquotewrap(key) + ": " + dblquotewrap(val);
}

std::string json_bool_pair(std::string key, bool val){
    return dblquotewrap(key) + ": " + (val ? "true" : "false");
}

std::string json_export_list(std::vector<std::string> vals){
    std::string str;
    for(auto val : vals){
        bool is_last = val == vals.back();
        std::string comma = (is_last ? "" : ",\n");
        str += val + comma;
    }
    return str;
}

std::string json_list_pair(std::string key, std::vector<std::string> vals){
    std::string str = dblquotewrap(key) + ": [";
    for(auto val : vals){
        bool is_last = val == vals.back();
        std::string comma = (is_last ? "" : ", ");
        str += dblquotewrap(val) + comma;
    }
    str+= "]";
    return str;
}

std::string Graphml::ModelParser::GetDeploymentJSON(){
    std::string newline("\n");

    std::vector<Graphml::HardwareNode*> nodes;

    for(auto n : hardware_nodes_){
        nodes.push_back(n.second);
    }

    //Jsonify model metadata
    std::string model_str;
    model_str += tab() + dblquotewrap("model") + ":{" + newline + tab(2) + json_pair("name", model_->name) + "," + 
                                                        newline + tab(2) + json_pair("id", model_->id) + "," + 
                                                        newline + tab(2) + json_pair("projectUUID", model_->uuid) +
                                                        newline + tab() + "},";


    //Jsonify node information
    std::vector<std::string> node_strs;

    for(auto node : nodes){

        bool is_deployed = node->is_deployed();
        auto logging_profile = GetLoggingProfile(node->logging_profile_id);
        auto logging_server = GetLoggingServer(node->logging_server_id);
        bool is_master = node->is_re_master; 

        if(is_deployed || is_master){
            std::vector<std::string> node_strings;
            //node_str += tab() + dblquotewrap(node->name) + ":{" + newline;
            
            //Output ComponentInstances
            if(is_deployed){
                std::vector<std::string> component_strings;
                
                for(auto c_id : node->component_ids){
                    auto component = GetComponentInstance(c_id);
                    if(component){
                        std::string c_str;
                        c_str += tab(4) + "{" + newline;
                        c_str += tab(5) + json_pair("id", component->id) + "," + newline; 
                        c_str += tab(5) + json_pair("name", component->name) + "," + newline; 
                        c_str += tab(5) + json_pair("type", component->type_name) + newline; 
                        c_str += tab(4) + "}";

                        component_strings.push_back(c_str);
                    }
                }
                std::string comp_string = tab(3) + dblquotewrap("component_instances") + ":[" + newline + json_export_list(component_strings) + newline + tab(3) +"]";
                node_strings.push_back(comp_string);
            }

            if(logging_profile){
                if(logging_profile->mode != "OFF"){
                    std::string logcl_str;
                    //Output Logan Client
                    logcl_str += tab(3) + dblquotewrap("logan_client") + ":{" + newline;
                    logcl_str += tab(4) + json_pair("publisher", node->GetLoganClientAddress()) + "," + newline;
                    logcl_str += tab(4) + json_pair("frequency", logging_profile->frequency) + "," + newline;
                    if(!logging_profile->processes.empty()){
                        logcl_str += tab(4) + json_pair("process", logging_profile->processes) + "," + newline;
                    }
                    logcl_str += tab(4) + json_bool_pair("live_mode", logging_profile->mode == "LIVE") + newline;
                    logcl_str += tab(3) + "}";
                    node_strings.push_back(logcl_str);
                }
            }

            if(logging_server){
                //Get the list of clients
                std::string logsv_str;
                //Output Logan Client
                logsv_str += tab(3) + dblquotewrap("logan_server") + ":{" + newline;

                for(auto n_id : logging_server->connected_hardware_ids){
                    auto n = GetHardwareNode(n_id);
                    if(n){
                        logsv_str += tab(4) + json_pair("clients", n->GetLoganClientAddress()) + "," + newline;
                        logsv_str += tab(4) + json_pair("clients", n->GetModelLoggerAddress()) + "," + newline;
                    }
                }

                logsv_str += tab(4) + json_pair("database", logging_server->database_name) + newline;
                logsv_str += tab(3) + "}";
                node_strings.push_back(logsv_str);
            }
            if(is_deployed){
                std::string renm_str;
                renm_str += tab(3) + dblquotewrap("re_node_manager") + ":{" + newline;
                if(is_master){
                    renm_str += tab(3) + json_pair("master", node->GetNodeManagerMasterAddress()) + "," + newline;
                }
                renm_str += tab(4) + json_pair("slave", node->GetNodeManagerSlaveAddress()) + newline;
                renm_str += tab(3) + "}";
                node_strings.push_back(renm_str);
            }

            std::string node_str = tab(2) + dblquotewrap(node->name) + ":{" + newline + json_export_list(node_strings) + newline + tab(2) + "}";
            node_strs.push_back(node_str);
        }
    }

    std::string node_list_str;
    node_list_str += tab() + dblquotewrap("nodes") + ":{" + newline + json_export_list(node_strs) + newline + tab() + "}";

    std::string str = "{" + newline + model_str + newline + node_list_str + newline + "}";
    return str;
}
