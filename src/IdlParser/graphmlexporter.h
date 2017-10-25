#ifndef GRAPHML_EXPORTER_H
#define GRAPHML_EXPORTER_H

#include <sstream>
#include <stack>
#include <map>

namespace Graphml{
    class Exporter{
        public:
            Exporter(bool pretty = true);
            void begin();
            
            int export_key(std::string key_name, std::string key_type);
            int export_edge(int edge_src, int edge_dst);
            int export_node();
            void export_graph();

            void export_data(std::string key_name, std::string key_value);

            bool close_element();

            int get_key_id(std::string key_name);

            void close();
            
            std::string ToGraphml();
        private:
            bool close_element_(bool needs_tab = true);

            int get_tab();

            int export_element(int id, std::string element, std::string attributes = "", bool one_line = false);
            void export_tab();
            void export_new_line();

            bool pretty = true;

            int id_count_ = 0;
            std::map<std::string, int> key_ids_;

            std::stack<std::string> element_stack_;
            
            std::stringstream ss;
    };
};

#endif //GRAPHML_EXPORTER_H
