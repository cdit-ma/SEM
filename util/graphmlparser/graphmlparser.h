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
        std::vector<std::string> FindNodesOfKind(const std::string& kind, const std::string& parent_id = "");

        std::vector<std::string> FindNodesOfKinds(const std::vector<std::string>& kinds, const std::string& parent_id = "");

        std::vector<std::string> FindEdges(const std::string& kind = "");

        std::vector<std::string> FindImmediateChildren(const std::string& kind, const std::string& parent_id);

        std::string GetAttribute(const std::string& id, const std::string& attribute_name);
        std::string GetDataValue(const std::string& id, const std::string& key_name);
        std::string GetParentNode(const std::string& id, int depth);
        std::string GetParentNode(const std::string& id);

        std::string GetSharedParent(const std::string& left_child_id, const std::string& right_child_id);
        int GetHeightToParent(const std::string& child_id, const std::string& parent_id);

    private:
        bool legal_parse = false;
        std::unordered_map<std::string, std::string> attribute_map_;
        std::unordered_map<std::string, pugi::xml_node> id_lookup_;
        pugi::xml_document doc;
        std::unordered_map<std::string, std::string> data_lookup_;
        std::unordered_map<std::string, std::string> attr_lookup_;
        std::unordered_map<std::string, std::string> parent_id_lookup_;

        std::unordered_map<std::string, std::string> shared_parent_map_;
        std::unordered_map<std::string, int> parent_height_depth_map_;


};
#endif //GRAPHMLPARSER_H