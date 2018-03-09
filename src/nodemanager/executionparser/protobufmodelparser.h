#ifndef PROTOBUFMODELPARSER_H
#define PROTOBUFMODELPARSER_H
#include "graphmlparser.h"
#include <unordered_map>

#include <google/protobuf/message_lite.h>
#include "controlmessage.pb.h"

class ProtobufModelParser{
    public:
        ProtobufModelParser(const std::string& filename);
        bool IsValid();

        std::string GetDeploymentJSON();
        NodeManager::ControlMessage* GetControlMessage();

    private:
        GraphmlParser* graphml_parser_;
        bool success_;
        bool PreProcess();
        bool ParseHardwareItems(NodeManager::ControlMessage* control_message);
        bool Process();
        void RecurseEdge(const std::string& source_id, const std::string& current_id);

        //Parse helpers
        std::string GetDeployedID(const std::string& id);
        void SetAttributePb(NodeManager::Attribute*, const std::string& type, const std::string& value);
        std::string GetDefinitionId(const std::string& id);
        std::string GetAggregateId(const std::string& id);
        std::string GetImplId(const std::string& id);

        std::string BuildPortGuid(const std::string& port_id);

        NodeManager::EventPort::Kind GetPortKind(const std::string& kind);
        NodeManager::HardwareItem::HardwareType GetHardwareItemKind(const std::string& kind);

        std::string to_lower(const std::string& s);
        bool str2bool(std::string str);

        NodeManager::ControlMessage* control_message_;

        std::string model_id_;
        std::string model_name_;

        std::vector<std::string> deployment_edge_ids_;
        std::vector<std::string> assembly_edge_ids_;
        std::vector<std::string> definition_edge_ids_;
        std::vector<std::string> aggregate_edge_ids_;
        std::vector<std::string> hardware_cluster_ids_;
        std::vector<std::string> component_assembly_ids_;
        std::vector<std::string> qos_edge_ids_;

        std::unordered_map<std::string, std::set<std::string> > entity_edge_ids_;

        std::unordered_map<std::string, std::string> deployed_entities_map_;
        std::unordered_map<std::string, std::string> definition_ids_;
        std::unordered_map<std::string, std::string> aggregate_ids_;
        std::unordered_map<std::string, std::string> entity_qos_map_;

        std::unordered_map<std::string, int> replication_map_;
        std::unordered_map<std::string, std::string> full_assembly_name_map_;


        std::vector<std::string> hardware_node_ids_;
        std::vector<std::string> component_ids_;
        std::vector<std::string> component_impl_ids_;
        std::vector<std::string> component_instance_ids_;

        std::unordered_map<std::string, std::string> port_guid_map_;


        std::unordered_map<std::string, NodeManager::HardwareItem *> node_message_map_;

        class AssemblyConnection{
            public:
                std::string source_id;
                std::string target_id;
                bool inter_assembly = false;
        };
        std::unordered_map<std::string, std::vector<AssemblyConnection> > assembly_map_;
};
#endif //PROTOBUFMODELPARSER_H
