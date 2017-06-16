#ifndef EXECUTIONPARSER_EXECUTIONPARSER_H
#define EXECUTIONPARSER_EXECUTIONPARSER_H

#include <string>
#include <vector>
#include <map>

class GraphmlParser;
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
        public:
            Graphml::HardwareNode* GetHardwareNodeByName(std::string host_name);
            Graphml::HardwareNode* GetHardwareNodeByIPAddress(std::string ip_address);

            //Get List functions        
            std::vector<HardwareNode*> GetHardwareNodes();
            std::vector<ComponentInstance*> GetComponentInstances();

            //Getter Functions from ID
            Graphml::HardwareNode* GetHardwareNode(std::string id);
            Graphml::HardwareCluster* GetHardwareCluster(std::string id);
            Graphml::LoggingProfile* GetLoggingProfile(std::string id);
            Graphml::LoggingServer* GetLoggingServer(std::string id);
            Graphml::Component* GetComponentDefinition(std::string id);
            Graphml::ComponentInstance* GetComponentInstance(std::string id);
            Graphml::ComponentAssembly* GetComponentAssembly(std::string id);
            Graphml::ComponentReplication* GetComponentReplication(std::string id);
            Graphml::EventPort* GetEventPort(std::string id);
            Graphml::Attribute* GetAttribute(std::string id);
            Graphml::Edge* GetEdge(std::string id);
            std::string GetAttribute(std::string id, std::string attr_name);
            std::string GetDataValue(std::string id, std::string key_name);

            std::string GetDeploymentJSON();
        private:
            std::string to_lower(std::string str);
            bool Process();
            GraphmlParser* graphml_parser_;

            std::string GetTCPAddress(const std::string ip, const unsigned int port_number);
            std::string GetIiopAddress(const std::string ip, const unsigned int port_number);
            std::string GetNamedIiopAddress(const std::string ip, const unsigned int port_number, const std::string name);
            
            std::string GetDefinitionId(std::string id);
            std::string GetAggregateID(std::string id);
            std::string GetDeployedID(std::string id);
            std::string GetImplId(std::string id);
            void RecurseEdge(std::string source_id, std::string current_id);
            std::string GetUniquePrefix(int count);


            std::vector<std::string> GetRequiredSlaveEndpoints();

        private:
            bool first_node_ = true;

            bool success = false;
            //Lookup Maps

            Graphml::Model* model_;
            std::map<std::string, Graphml::HardwareNode*> hardware_nodes_;
            std::map<std::string, Graphml::HardwareCluster*> hardware_clusters_;

            std::map<std::string, Graphml::LoggingProfile*> logging_profiles_;
            std::map<std::string, Graphml::LoggingServer*> logging_servers_;

            std::map<std::string, Graphml::Component*> components_;
            std::map<std::string, Graphml::ComponentInstance*> component_instances_;
            std::map<std::string, Graphml::ComponentAssembly*> component_assemblies_;

            std::map<std::string, Graphml::EventPort*> event_ports_;
            std::map<std::string, Graphml::Attribute*> attributes_;
            std::map<std::string, Graphml::Edge*> edges_;

            std::vector<std::string> deployment_edge_ids_;
            std::vector<std::string> assembly_edge_ids_;
            std::vector<std::string> definition_edge_ids_;
            std::vector<std::string> aggregate_edge_ids_;
            std::vector<std::string> qos_edge_ids_;

            std::map<std::string, std::string> deployed_entities_map_;
            std::map<std::string, std::string> definition_ids_;
            std::map<std::string, std::string> aggregate_ids_;
            std::map<std::string, std::string> entity_qos_map_;


            std::vector<std::string> required_slaves_;
            std::vector<std::string> inactive_slaves_;


            std::map<std::string, ComponentReplication*> component_replications_;

            std::map<std::string, std::vector<AssemblyConnection *> > assembly_map_;
    };
};

#endif //EXECUTIONPARSER_EXECUTIONPARSER_H