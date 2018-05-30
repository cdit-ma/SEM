#ifndef EXECUTIONPARSER_EXECUTIONPARSER_H
#define EXECUTIONPARSER_EXECUTIONPARSER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <set>

class GraphmlParserInt;
namespace Graphml{
    //Forward Declare
    class HardwareNode;
    class HardwareCluster;
    class Component;
    class ComponentInstance;
    class EventPort;
    class Attribute;
    class ComponentAssembly;
    class ComponentReplication;
    class Edge;
    class AssemblyConnection;
    class Model;

    class LoggingProfile;
    class LoggingServer;
    
    class ModelParser{
        public:
            ModelParser(const std::string filename);
            bool IsValid();
        public:
            Graphml::HardwareNode* GetHardwareNodeByName(const std::string& host_name);
            Graphml::HardwareNode* GetHardwareNodeByIPAddress(const std::string& ip_address);

            //Get List functions        
            std::vector<HardwareNode*> GetHardwareNodes();
            std::vector<ComponentInstance*> GetComponentInstances();

            //Getter Functions from ID
            Graphml::HardwareNode* GetHardwareNode(const std::string& id);
            Graphml::HardwareCluster* GetHardwareCluster(const std::string& id);
            Graphml::LoggingProfile* GetLoggingProfile(const std::string& id);
            Graphml::LoggingServer* GetLoggingServer(const std::string& id);
            Graphml::Component* GetComponentDefinition(const std::string& id);
            Graphml::ComponentInstance* GetComponentInstance(const std::string& id);
            Graphml::ComponentAssembly* GetComponentAssembly(const std::string& id);
            Graphml::ComponentReplication* GetComponentReplication(const std::string& id);
            Graphml::EventPort* GetEventPort(const std::string& id);
            Graphml::Attribute* GetAttribute(const std::string& id);
            Graphml::Edge* GetEdge(const std::string& id);
            std::string GetAttribute(const std::string& id, const std::string& attr_name);
            std::string GetDataValue(const std::string& id, std::string key_name);
            
            std::string GetDeploymentJSON();
        private:
            std::string to_lower(const std::string& str);
            bool Process();
            GraphmlParserInt* graphml_parser_;

            std::string GetTCPAddress(const std::string ip, const unsigned int port_number);
            std::string GetDefinitionId(const std::string& id);
            std::string GetAggregateID(const std::string& id);
            std::string GetDeployedID(const std::string& id);
            std::string GetImplId(const std::string& id);
            void RecurseEdge(const std::string& source_id, std::string current_id);
            std::string GetUniquePrefix(int count);


            std::vector<std::string> GetRequiredSlaveEndpoints();

        private:
            bool first_node_ = true;

            bool success = false;
            //Lookup Maps

            Graphml::Model* model_ = 0;
            std::unordered_map<std::string, Graphml::HardwareNode*> hardware_nodes_;
            std::unordered_map<std::string, Graphml::HardwareCluster*> hardware_clusters_;

            std::unordered_map<std::string, Graphml::LoggingProfile*> logging_profiles_;
            std::unordered_map<std::string, Graphml::LoggingServer*> logging_servers_;

            std::unordered_map<std::string, Graphml::Component*> components_;
            std::unordered_map<std::string, Graphml::ComponentInstance*> component_instances_;
            std::unordered_map<std::string, Graphml::ComponentAssembly*> component_assemblies_;

            std::unordered_map<std::string, Graphml::EventPort*> event_ports_;
            std::unordered_map<std::string, Graphml::Attribute*> attributes_;
            std::unordered_map<std::string, Graphml::Edge*> edges_;

            std::vector<std::string> deployment_edge_ids_;
            std::vector<std::string> assembly_edge_ids_;
            std::vector<std::string> definition_edge_ids_;
            std::vector<std::string> aggregate_edge_ids_;
            std::vector<std::string> qos_edge_ids_;

            std::unordered_map<std::string, std::set<std::string> > entity_edge_ids_;

            std::unordered_map<std::string, std::string> deployed_entities_map_;
            std::unordered_map<std::string, std::string> definition_ids_;
            std::unordered_map<std::string, std::string> aggregate_ids_;
            std::unordered_map<std::string, std::string> entity_qos_map_;


            std::vector<std::string> required_slaves_;
            std::vector<std::string> inactive_slaves_;


            std::unordered_map<std::string, ComponentReplication*> component_replications_;

            std::unordered_map<std::string, std::vector<AssemblyConnection *> > assembly_map_;
    };
};

#endif //EXECUTIONPARSER_EXECUTIONPARSER_H