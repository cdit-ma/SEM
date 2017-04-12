#ifndef EXECUTIONMANAGER_H
#define EXECUTIONMANAGER_H

#include <mutex>
#include <thread>
#include <condition_variable>
#include <string>
#include "graphmlparser.h"

#include <iostream>
#include <map>
#include <vector>

namespace google{
    namespace protobuf{
        class MessageLite;
    };
};

namespace NodeManager{
    class ControlMessage;
};

namespace zmq{class ProtoWriter;};

class Execution;

class ExecutionManager{
    public:
    struct AssemblyConnection{
        std::string source_id;
        std::string target_id;
        bool inter_assembly = false;
    };

    struct HardwareNode{
        std::string id;
        std::string name;
        std::string ip_address;
        std::string parent_id;
        int port_count = 6000;
        int node_manager_port = 7001;
        int logger_port_number = 6000;
        std::vector<std::string> component_ids;
    };

    struct HardwareCluster{
        std::string id;
        std::string name;
        std::vector<std::string> node_ids;

        std::string GetHardwareNode(int position){
            int index = 0;
            if(position > 0){
                index = node_ids.size() % position;
            }

            if(index >= 0 && index < node_ids.size()){
                return node_ids[index];
            }
            return "";
        }
    };

    struct ComponentReplication{
        std::string id;
        std::vector<std::string> component_instance_ids;
    };

    struct ComponentInstance{
        std::string parent_id;
        std::string node_id;
        int replicate_id;

        std::string definition_id;
        std::string implementation_id;
        std::string deployed_node_id;
        
        std::string original_id;
        std::string id;
        std::string name;
        std::string type_name;
        
        std::vector<std::string> event_port_ids;
        std::vector<std::string> attribute_ids;
    };

    struct Component{
        std::string id;
        std::string implementation_id;
        std::string name;
        std::vector<std::string> instance_ids;
    };

    struct ComponentAssembly{
        std::string parent_id;
        std::string id;
        std::string name;
        int replicate_count;
    };


   
    struct EventPort{
        std::string component_id;
        std::string id;
        std::string name;
        std::string kind;
        std::string middleware;

        std::string namespace_name;

        std::string message_type;

        std::string frequency;

        int port_number = -1;
        std::string port_address;
        std::string topic_name;

        std::vector<std::string> connected_port_ids;
    };

    struct Attribute{
        std::string component_id;
        std::string id;
        std::string type;
        std::string value;
        std::string name;
    };

    public:
        ExecutionManager(std::string endpoint, std::string graphml_path, double execution_duration, Execution* execution);

        std::vector<std::string> GetRequiredSlaveEndpoints();
        std::string GetHostNameFromAddress(std::string address);
        std::string GetLoggerAddressFromHostName(std::string host_name);

        void ExecutionLoop(double duration_sec);

        void ActivateExecution();
        void TerminateExecution();

        void PushMessage(std::string topic, google::protobuf::MessageLite* message);
        void SlaveOnline(std::string response, std::string endpoint, std::string host_name);

        bool Finished();
    private:
        std::string GetAttribute(std::string id, std::string attr_name);
        std::string GetDataValue(std::string id, std::string key_name);
        std::string GetTCPAddress(const std::string ip, const unsigned int port_number);
        std::string GetDefinitionId(std::string id);
        std::string GetAggregateID(std::string id);
        std::string GetDeployedID(std::string id);
        std::string GetImplId(std::string id);
        void RecurseEdge(std::string source_id, std::string current_id);
        std::string GetUniquePrefix(int count);

        void HandleSlaveOnline(std::string endpoint);

        

        ExecutionManager::HardwareNode* GetHardwareNode(std::string id);
        ExecutionManager::HardwareCluster* GetHardwareCluster(std::string id);
        ExecutionManager::ComponentInstance* GetComponentInst(std::string id);
        ExecutionManager::Component* GetComponent(std::string id);
        ExecutionManager::ComponentAssembly* GetComponentAssembly(std::string id);
        ExecutionManager::EventPort* GetEventPort(std::string id);
        ExecutionManager::Attribute* GetAttribute(std::string id);
        ExecutionManager::ComponentReplication* GetComponentReplication(std::string id);

        

        bool ScrapeDocument();

        std::mutex mutex_;
        std::map<std::string, NodeManager::ControlMessage*> deployment_map_;

        std::vector<std::string> required_slaves_;
        std::vector<std::string> inactive_slaves_;
        std::thread* execution_thread_ = 0;


        std::mutex activate_mutex_;
        std::condition_variable activate_lock_condition_;

        std::mutex terminate_mutex_;
        std::condition_variable terminate_lock_condition_;
        bool terminate_flag_ = false;
        bool finished_ = false;

        Execution* execution_;
        

        zmq::ProtoWriter* proto_writer_;
        GraphmlParser* graphml_parser_;

        //IDs to Entities
        std::map<std::string, HardwareNode*> hardware_nodes_;
        std::map<std::string, HardwareCluster*> hardware_clusters_;
        std::map<std::string, Component*> components_;
        std::map<std::string, ComponentInstance*> component_instances_;
        std::map<std::string, EventPort*> event_ports_;
        std::map<std::string, Attribute*> attributes_;
        std::map<std::string, ComponentAssembly*> component_assemblies_;

        std::map<std::string, ComponentReplication*> component_replications_;

        std::map<std::string, std::vector<AssemblyConnection *> > assembly_map_;
        
        std::vector<std::string> deployment_edge_ids_;
        std::vector<std::string> assembly_edge_ids_;
        std::vector<std::string> definition_edge_ids_;
        std::vector<std::string> aggregate_edge_ids_;

        std::map<std::string, std::string> deployed_entities_map_;
        std::map<std::string, std::string> definition_ids_;
        std::map<std::string, std::string> aggregate_ids_;
        
};

#endif //EXECUTIONMANAGER_H