#ifndef EXECUTIONPARSER_DATATYPES_H
#define EXECUTIONPARSER_DATATYPES_H

#include <string>
#include <vector>

namespace Graphml{
    //Forward Declare

    class LoggingProfile{
        public:
        std::string id;
        std::string mode;
        std::string processes;
        std::string frequency;
    };

    class LoggingServer{
        public:
        std::string id;
        std::string database_name;
        std::vector<std::string> connected_hardware_ids;

    };

    class HardwareNode{
        public:
            std::string id;
            std::string name;
            std::string ip_address;
            std::string parent_id;

            std::string logging_profile_id;
            std::string logging_server_id;
            int port_count = 6000;
            int node_manager_master_port = 7000;
            int node_manager_slave_port = 7001;
            
            int model_logger_port_number;
            int logan_client_port_number;

            bool is_re_master = false;

            std::vector<std::string> component_ids;
            std::vector<std::string> logged_processes = {"re_node_manager"};
            std::vector<std::string> logan_clients;

            bool is_deployed(){
                return !component_ids.empty();
            }

            std::string TCPify(std::string ip, int port_number){
                return "tcp://" + ip + ":" + std::to_string(port_number);
            }
            std::string GetLoganClientAddress(){
                return TCPify(ip_address, logan_client_port_number);
            }
            std::string GetModelLoggerAddress(){
                return TCPify(ip_address, model_logger_port_number);
            }
            
            std::string GetNodeManagerSlaveAddress(){
                return TCPify(ip_address, node_manager_slave_port);
            }
            std::string GetNodeManagerMasterAddress(){
                return TCPify(ip_address, node_manager_master_port);
            }
    };

    class HardwareCluster{
        public:
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
            };
    };

    class ComponentReplication{
        public:
            std::string id;
            std::vector<std::string> component_instance_ids;
    };

    class Component{
        public:
            std::string id;
            std::string implementation_id;
            std::string name;
            std::vector<std::string> instance_ids;
    };

    class ComponentInstance{
        public:
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

    class EventPort{
        public:
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
            std::string named_port_address;
            std::string topic_name;

            std::string qos_profile_name;
            std::string qos_profile_path;

            std::vector<std::string> connected_port_ids;
    };

    class Attribute{
        public:
            std::string component_id;
            std::string id;
            std::string type;
            std::string value;
            std::string name;
    };

    class ComponentAssembly{
        public:
            std::string parent_id;
            std::string id;
            std::string name;
            int replicate_count;
    };

    class Edge{
        public:
            std::string kind;
            std::string src_id;
            std::string dst_id;
    };

    class AssemblyConnection{
        public:
            std::string source_id;
            std::string target_id;
            bool inter_assembly = false;
    };

    class Model{
        public:
            std::string id;
            std::string name;
            std::string uuid;
            std::string description;
            std::vector<std::string> middlewares;
    };
};

#endif //EXECUTIONPARSER_EXECUTIONPARSER_H