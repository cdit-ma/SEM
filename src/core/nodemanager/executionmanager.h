#ifndef EXECUTIONMANAGER_H
#define EXECUTIONMANAGER_H

#include <mutex>
#include <thread>
#include <condition_variable>
#include <string>
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
namespace Graphml{class ModelParser;};

class Execution;

class ExecutionManager{
    public:
<<<<<<< HEAD
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
=======
        ExecutionManager(std::string endpoint, std::string graphml_path, double execution_duration);
>>>>>>> master

        std::vector<std::string> GetNodeManagerSlaveAddresses();
        
        std::string GetNodeNameFromNodeManagerAddress(std::string address);
        std::string GetModelLoggerAddressFromNodeName(std::string host_name);

        void ExecutionLoop(double duration_sec);

        void ActivateExecution();
        void TerminateExecution();

        void PushMessage(std::string topic, google::protobuf::MessageLite* message);
        void SlaveOnline(std::string response, std::string endpoint, std::string host_name);

        bool Finished();
    private:
        void HandleSlaveOnline(std::string endpoint);
        bool ConstructControlMessages();

        std::mutex mutex_;

        std::thread* execution_thread_ = 0;

        std::map<std::string, NodeManager::ControlMessage*> deployment_map_;

        std::mutex activate_mutex_;
        std::condition_variable activate_lock_condition_;

        std::mutex terminate_mutex_;
        std::condition_variable terminate_lock_condition_;
        bool terminate_flag_ = false;
        bool finished_ = false;

        Execution* execution_;
        
        zmq::ProtoWriter* proto_writer_;
        Graphml::ModelParser* model_parser_;

        std::vector<std::string> required_slave_addresses_;
        std::vector<std::string> inactive_slave_addresses_;
};

#endif //EXECUTIONMANAGER_H