#ifndef GRAPHMLPARSER_H
#define GRAPHMLPARSER_H

#include <pugixml.hpp>
#include <string>
#include <vector>
#include <map>


class GraphmlParser{

    public:
        GraphmlParser(const std::string filename);
        std::vector<std::string> find_nodes(std::string kind, std::string parent_id = "");
        std::vector<std::string> find_edges(std::string kind);

        std::string get_attribute(std::string id, std::string attribute_name);
        std::string get_data_value(std::string id, std::string key_name);

    private:
        std::map<std::string, std::string> attribute_map_;

        pugi::xml_document doc;


};
#endif //"GRAPHMLPARSER_H"