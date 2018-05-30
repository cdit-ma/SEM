#ifndef GRAPHMLPARSER_H
#define GRAPHMLPARSER_H

#include <pugixml.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include "graphmlparserint.h"

class GraphmlParser : public GraphmlParserInt{

    public:
        GraphmlParser(const std::string& filename);
        bool IsValid();
        std::vector<std::string> FindNodes(const std::string& kind, const std::string& parent_id = "");
        std::vector<std::string> FindEdges(const std::string& kind = "");

        std::vector<std::string> FindImmediateChildren(const std::string& kind, const std::string& parent_id);

        std::string GetAttribute(const std::string& id, const std::string& attribute_name);
        std::string GetDataValue(const std::string& id, const std::string& key_name);
        std::string GetParentNode(const std::string& id, int depth);
        std::string GetParentNode(const std::string& id);
    private:
        bool legal_parse = false;
        std::unordered_map<std::string, std::string> attribute_map_;
        std::unordered_map<std::string, pugi::xml_node> id_lookup_;
        pugi::xml_document doc;
        std::unordered_map<std::string, std::string> data_lookup_;
        std::unordered_map<std::string, std::string> attr_lookup_;
        std::unordered_map<std::string, std::string> parent_id_lookup_;


};
#endif //"GRAPHMLPARSER_H"