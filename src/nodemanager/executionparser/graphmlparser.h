#ifndef GRAPHMLPARSER_H
#define GRAPHMLPARSER_H

#include <pugixml.hpp>
#include <string>
#include <vector>
#include <unordered_map>


class GraphmlParser{

    public:
        GraphmlParser(std::string filename);
        bool IsValid();
        std::vector<std::string> FindNodes(std::string kind, std::string parent_id = "");
        std::vector<std::string> FindEdges(std::string kind ="");

        std::string GetAttribute(std::string id, std::string attribute_name);
        std::string GetDataValue(std::string id, std::string key_name);

        std::string GetParentNode(std::string id);
        

    private:
        bool legal_parse = false;
        std::unordered_map<std::string, std::string> attribute_map_;
        pugi::xml_document doc;
        std::unordered_map<std::string, std::string> data_lookup_;
        std::unordered_map<std::string, std::string> attr_lookup_;
        std::unordered_map<std::string, std::string> parent_id_lookup_;


};
#endif //"GRAPHMLPARSER_H"