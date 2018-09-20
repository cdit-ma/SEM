#ifndef PROTOBUFMODELPARSER_H
#define PROTOBUFMODELPARSER_H
#include "graphmlparserint.h"
#include <unordered_map>
#include <list>

#include <proto/controlmessage/controlmessage.pb.h>

class ProtobufModelParser{
    public:
        ProtobufModelParser(const std::string& filename, const std::string& experiment_id);
        ~ProtobufModelParser(){
            delete top_level_assembly_;
        }
        bool IsValid();

        std::string GetDeploymentJSON();
        NodeManager::ControlMessage* ControlMessage();

    private:

        struct ComponentReplication;
        struct Assembly{
            Assembly(const std::string& name, const std::string& assembly_id, Assembly* parent = 0){
                this->name = name;
                this->assembly_id = assembly_id;
                this->parent = parent;
                if(parent){
                    parent->children.push_back(this);
                }
            };

            Assembly(Assembly* assembly):
            Assembly(assembly->name, assembly->assembly_id, assembly->parent){
                this->replication_count = assembly->replication_count;
            };
            ~Assembly(){
                for(auto child : children){
                    delete child;
                }
                for(auto replication : replications){
                    delete replication;
                }
            };

            std::string name;
            std::string assembly_id;

            int replication_count = 1;
            
            int replication_index; //Our replication as an assembly



            Assembly* parent = 0;

            std::vector<Assembly*> children;
            std::vector<ComponentReplication*> replications;

        };

        //Replication struct, represents one instance of a component instance that has been replicated
        struct ComponentReplication{
            std::string component_instance_id;
            std::string id;

            Assembly* parent;
            std::list<std::string> GetReplicationLocation() const;
            std::list<int> GetReplicationIndices() const;
            std::string GetUniqueId() const;
            std::string GetUniqueIdSuffix() const;
        };

        std::list<std::string> GetNamespace(const std::string& id);
        std::unique_ptr<GraphmlParserInt> graphml_parser_;
        bool is_valid_;
        bool pre_process_success_;
        bool process_success_;
        bool PreProcess();

        Assembly* top_level_assembly_ = 0;

        void GenerateReplications(Assembly* parent);

        bool ParseHardwareItems(NodeManager::ControlMessage* environment_message);
        bool ParseExternalDelegates(NodeManager::ControlMessage* control_message);

        void ParseLoggingClients();
        void ParseLoggingServers();
        void ParseComponents();

        bool Process();

        void RecurseEdge(const std::string& source_id, const std::string& current_id);
        
        void FillProtobufAttributes(::google::protobuf::Map< ::std::string, ::NodeManager::Attribute >* attrs, const std::string& parent_id, const std::string& unique_id_suffix);

        std::set<std::string> GetTerminalSourcesByEdgeKind(const std::string& node_id, const std::string& edge_kind, std::set<std::string> prev_id);

        void CalculateReplication();
        std::string GetDeployedHardwareID(const std::string& component_id);

        //Parse helpers
        std::string GetDeployedID(const std::string& id);
        void SetAttributePb(NodeManager::Attribute&, const std::string& type, const std::string& value);
        std::string GetDefinitionId(const std::string& id);
        std::string GetRecursiveDefinitionId(const std::string& id);
        std::string GetAggregateId(const std::string& id);
        std::string GetImplId(const std::string& id);

        int GetReplicationId(const std::string& id);
        NodeManager::Middleware ParseMiddleware(const std::string& middleware_str) const;


        std::string BuildPortGuid(const std::string& port_id);

        NodeManager::Port::Kind GetPortKind(const std::string& kind);
        NodeManager::ExternalPort::Kind GetExternalPortKind(const std::string& kind);
        NodeManager::Node::NodeType GetHardwareItemKind(const std::string& kind);

        std::string to_lower(const std::string& s);
        bool str2bool(std::string str);
        static std::string GetUniqueSuffix(const std::vector<int>& indices);
        

        std::vector<std::string> GetComponentInstanceLocation(ComponentReplication* component);
        std::vector<int> GetComponentInstanceReplication(ComponentReplication* component);

        NodeManager::Port* ConstructPubSubPortPb(const std::string& port_id, const std::string& unique_id_suffix);
        NodeManager::Port* ConstructReqRepPortPb(const std::string& port_id, const std::string& unique_id_suffix);
        NodeManager::Port* ConstructPeriodicPb(const std::string& port_id, const std::string& unique_id_suffix);
        NodeManager::Worker* ConstructWorkerPb(const std::string& worker_id, const std::string& unique_id_suffix);

        NodeManager::ControlMessage* control_message_;

        std::string model_id_;
        std::string experiment_id_;

        std::vector<std::string> deployment_edge_ids_;
        std::vector<std::string> data_edge_ids_;
        std::vector<std::string> assembly_edge_ids_;
        std::vector<std::string> definition_edge_ids_;
        std::vector<std::string> aggregate_edge_ids_;
        std::vector<std::string> hardware_cluster_ids_;
        std::vector<std::string> component_assembly_ids_;
        std::vector<std::string> qos_edge_ids_;
        std::vector<std::string> logging_server_ids_;
        std::vector<std::string> logging_client_ids_;

        //source/target id -> set of all edge id's attached to source/target
        std::unordered_map<std::string, std::set<std::string> > entity_edge_ids_;

        //ComponentInstance id - > HardwareNode id
        std::unordered_map<std::string, std::string> deployed_entities_map_;

        //component instance id -> definition id
        std::unordered_map<std::string, std::string> definition_ids_;

        //
        std::unordered_map<std::string, std::string> aggregate_ids_;

        //
        std::unordered_map<std::string, std::string> entity_qos_map_;

        //Component id -> replication count
        std::unordered_map<std::string, int> replication_map_;

        //attribute_instance id -> Value
        std::unordered_map<std::string, std::string> attribute_value_map_;

        //port id -> nested assembly string
        std::unordered_map<std::string, std::string> full_assembly_name_map_;

        //port replicate id -> eventport proto
        std::unordered_map<std::string, NodeManager::Port*> port_replicate_id_map_;
        //external port id -> ExternalPort Proto
        std::unordered_map<std::string, NodeManager::ExternalPort*> external_port_id_map_;

        //component id -> vector of that component's replications
        std::unordered_map<std::string, std::vector<NodeManager::Component*> > component_replications_;

        std::vector<ComponentReplication*> component_instances_;

        std::vector<std::string> hardware_node_ids_;
        std::vector<std::string> component_ids_;
        std::vector<std::string> component_impl_ids_;
        std::vector<std::string> component_instance_ids_;

        std::vector<std::string> delegates_pubsub_ids_;
        std::vector<std::string> delegates_server_ids_;

        //event port id -> fully qualified event port guid
        std::unordered_map<std::string, std::string> port_guid_map_;

        //node id -> pointer to proto message for that node
        std::unordered_map<std::string, NodeManager::Node *> node_message_map_;

        class AssemblyConnection{
            public:
                std::string source_id;
                std::string target_id;
                bool inter_assembly = false;
        };

        std::unordered_map<std::string, std::vector<AssemblyConnection> > assembly_map_;

        std::unordered_map<std::string, std::vector<std::string> > logging_server_client_map_;
};
#endif //PROTOBUFMODELPARSER_H
