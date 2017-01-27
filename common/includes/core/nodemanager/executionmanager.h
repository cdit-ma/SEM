#ifndef EXECUTIONMANAGER_H
#define EXECUTIONMANAGER_H


#include <thread>
#include <string>
#include "graphmlparser.h"

#include <map>
#include <vector>

class ZMQMaster;
class ExecutionManager{
    struct HardwareNode{
        std::string id;
        std::string name;
        std::string ip_address;
        int port_count = 6000;
        int node_manager_port = 7001;
        std::vector<std::string> component_ids;
    };

    struct ComponentInstance{
        std::string node_id;

        std::string definition_id;
        std::string implementation_id;
        
        std::string id;
        std::string name;
        std::string type_name;
        
        std::vector<std::string> event_port_ids;
        std::vector<std::string> attribute_ids;
    };

    struct EventPort{
        std::string component_id;
        std::string id;
        std::string name;
        std::string kind;
        std::string middleware;

        std::string message_type;

        std::string frequency;

        int port_number = -1;
        std::string port_address;
        std::string topic_name;
    };

    struct Attribute{
        std::string component_id;
        std::string id;
        std::string name;
    };

    public:
        ExecutionManager(ZMQMaster *zmqmaster, std::string graphml_path);

        std::vector<std::string> get_slave_endpoints();
        std::string get_host_name_from_address(std::string address);

        void execution_loop();
    private:
        std::string get_attribute(std::string id, std::string attr_name);
        std::string get_data_value(std::string id, std::string key_name);
        

        ExecutionManager::HardwareNode* get_hardware_node(std::string id);
        ExecutionManager::ComponentInstance* get_component(std::string id);
        ExecutionManager::EventPort* get_event_port(std::string id);
        ExecutionManager::Attribute* get_attribute(std::string id);

        bool scrape_document();

        
        std::thread* execution_thread_;
        ZMQMaster* zmq_master_;
        GraphmlParser* graphml_parser_;

        std::map<std::string, HardwareNode*> hardware_nodes_;
        std::map<std::string, ComponentInstance*> component_instances_;
        std::map<std::string, EventPort*> event_ports_;
        std::map<std::string, Attribute*> attributes_;

        std::vector<std::string> deployment_edge_ids_;
        std::vector<std::string> assembly_edge_ids_;

        std::map<std::string, std::string> deployed_instance_map_;
        
};

#endif //EXECUTIONMANAGER_H