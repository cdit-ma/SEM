#ifndef EXECUTIONPARSER_DATATYPES_H
#define EXECUTIONPARSER_DATATYPES_H

#include <string>
#include <vector>

namespace Graphml{
    //Forward Declare
    class HardwareNode{
        public:
            std::string id;
            std::string name;
            std::string ip_address;
            std::string parent_id;
            int port_count = 6000;
            int node_manager_port = 7001;
            int logger_port_number = 6000;
            std::vector<std::string> component_ids;
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
            std::string topic_name;

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
};

#endif //EXECUTIONPARSER_EXECUTIONPARSER_H