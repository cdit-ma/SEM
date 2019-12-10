#ifndef PROTOBUFMODELPARSER_H
#define PROTOBUFMODELPARSER_H
#include "graphmlparserint.h"
#include "assembly.h"
#include <unordered_map>
#include <list>

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4251)
#endif
#include <proto/controlmessage/controlmessage.pb.h>
#ifdef _WIN32
#pragma warning(pop)
#endif

class ProtobufModelParser{
    public:
    // REVIEW (Mitch): These should be free functions.
        static std::unique_ptr<NodeManager::Experiment>
        ParseModel(const std::string& filename, const std::string& experiment_id);

        static std::unique_ptr<NodeManager::Experiment>
        ParseModelString(const std::string& filename, const std::string& experiment_id);

        static std::string GetDeploymentJSON(const NodeManager::Experiment& experiment);
    private:
        ProtobufModelParser(std::istream& model_stream, const std::string& experiment_id);
        // Parser
        std::unique_ptr<GraphmlParserInt> graphml_parser_;

        // Getter
        NodeManager::Experiment& GetExperiment();

        // Processing functions
        void PreProcess();
        void ParseHardwareItems();
        void ParseExternalDelegates();
        void ParseLoggingClients();
        void ParseLoggingServers();
        void ParseComponents();
        void Process();

        // Internal state helper functions
        void GenerateReplications(Assembly& parent);
        void GenerateChildAssemblies(Assembly& assembly, const std::vector<std::string>& assembly_children);
        void GenerateComponentReplications(Assembly& assembly, const std::vector<std::string>& component_children);

        void CalculateReplication();

        // Graphml helper functions
        std::set<std::string> GetTerminalSourcesByEdgeKind(const std::string& node_id, const std::string& edge_kind, std::set<std::string> prev_id);
        std::string GetDeployedID(const std::string& id);
        std::string GetDeployedContainerID(const std::string& component_id);
        std::string GetDefinitionId(const std::string& id);
        std::string GetAggregateId(const std::string& id);
        std::list<std::string> GetNamespaceList(const std::string& id) const;
        unsigned int GetReplicationId(const std::string& id);

        static std::string GetUniqueSuffix(const std::vector<int>& indices);

        // Protobuf helpers
        void SetAttributePb(NodeManager::Attribute&, const std::string& type, const std::string& value);
        void FillProtobufAttributes(::google::protobuf::Map< ::std::string, ::NodeManager::Attribute >* attrs, const std::string& parent_id, const std::string& unique_id_suffix);

        std::unique_ptr<NodeManager::Port> ConstructPubSubPortPb(const std::string& port_id, const std::string& unique_id_suffix);
        std::unique_ptr<NodeManager::Port> ConstructReqRepPortPb(const std::string& port_id, const std::string& unique_id_suffix);
        std::unique_ptr<NodeManager::Port> ConstructPeriodicPb(const std::string& port_id, const std::string& unique_id_suffix);
        std::unique_ptr<NodeManager::Worker> ConstructWorkerPb(const std::string& worker_id, const std::string& unique_id_suffix);

        // Protobuf converters
        NodeManager::Middleware ParseMiddleware(const std::string& middleware_str) const;
        NodeManager::Port::Kind GetPortKind(const std::string& kind) const;
        NodeManager::ExternalPort::Kind GetExternalPortKind(const std::string& kind) const;
        bool str2bool(std::string str) const;


        // Internal state members
        std::unique_ptr<Assembly> top_level_assembly_;

        // Output message
        std::unique_ptr<NodeManager::Experiment> experiment_;

        std::string experiment_id_;

        // Entity id lists
        std::vector<std::string> aggregate_edge_ids_;
        std::vector<std::string> assembly_edge_ids_;
        std::vector<std::string> definition_edge_ids_;
        std::vector<std::string> deployment_edge_ids_;
        std::vector<std::string> qos_edge_ids_;

        std::vector<std::string> hardware_cluster_ids_;
        std::vector<std::string> hardware_node_ids_;
        std::vector<std::string> deployment_container_ids_;
        std::vector<std::string> logging_server_ids_;
        std::vector<std::string> logging_client_ids_;

        std::vector<std::string> delegates_pubsub_ids_;
        std::vector<std::string> delegates_server_ids_;

        // Source/target id -> set of all edge id's attached to source/target
        std::unordered_map<std::string, std::set<std::string> > entity_edge_ids_;

        // Component instance id - > hardware node id
        std::unordered_map<std::string, std::vector<std::string> > deployed_entities_map_;

        // Component instance id -> component definition id
        std::unordered_map<std::string, std::string> definition_ids_;

        // Aggregate ->
        std::unordered_map<std::string, std::string> aggregate_ids_;

        // Port id -> QoS Profile id
        std::unordered_map<std::string, std::string> entity_qos_map_;

        // Attribute instance id -> attribute value
        std::unordered_map<std::string, std::string> attribute_value_map_;

        // Logging server id -> logging client ids
        std::unordered_map<std::string, std::vector<std::string> > logging_server_client_map_;

        // Owning vector of component replications
        std::vector<std::unique_ptr<ComponentReplication> > component_instances_;

        // Acceleration structures
        // "experiment_" owns these messages
        std::unordered_map<std::string, NodeManager::Cluster*> cluster_message_map_;
        std::unordered_map<std::string, NodeManager::Node* > node_message_map_;
        std::unordered_map<std::string, NodeManager::Container* > container_message_map_;
        // Component id -> vector of that component's replications
        std::unordered_map<std::string, std::vector<NodeManager::Component*> > component_replications_;
        // Port replicate id -> event port proto
        std::unordered_map<std::string, NodeManager::Port*> port_replicate_id_map_;
        //external port id -> ExternalPort Proto
        std::unordered_map<std::string, NodeManager::ExternalPort*> external_port_id_map_;

        // Internal data structure used for calculating inter assembly port connections
        class AssemblyConnection{
            public:
                std::string source_id;
                std::string target_id;
                bool inter_assembly = false;
        };
        std::unordered_map<std::string, std::vector<AssemblyConnection> > assembly_map_;

};
#endif //PROTOBUFMODELPARSER_H
