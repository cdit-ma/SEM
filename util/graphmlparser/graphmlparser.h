#ifndef GRAPHMLPARSER_H
#define GRAPHMLPARSER_H

#include <pugixml.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include "graphmlparserint.h"

class GraphmlParser : public GraphmlParserInt{

    public:
        explicit GraphmlParser(std::istream& model_stream);

        std::vector<std::string> FindNodesOfKind(const std::string& kind, const std::string& parent_id = "") override;

        std::vector<std::string> FindNodesOfKinds(const std::vector<std::string>& kinds, const std::string& parent_id = "") override;

        std::vector<std::string> FindEdges(const std::string& kind = "") override;

        std::vector<std::string> FindImmediateChildren(const std::string& kind, const std::string& parent_id) override;

        std::string GetAttribute(const std::string& id, const std::string& attribute_name) override;
        std::string GetDataValue(const std::string& id, const std::string& key_name) override;
        std::string GetParentNode(const std::string& id, int depth) override;
        std::string GetParentNode(const std::string& id) override;

        std::string GetSharedParent(const std::string& left_child_id, const std::string& right_child_id) override;
        int GetHeightToParent(const std::string& child_id, const std::string& parent_id) override;

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