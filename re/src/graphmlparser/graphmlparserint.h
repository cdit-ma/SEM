#ifndef GRAPHMLPARSERINT_H
#define GRAPHMLPARSERINT_H

#include <string>
#include <vector>

class GraphmlParserInt{
    public:
        explicit GraphmlParserInt(std::istream& model_stream){};

        virtual std::vector<std::string> FindNodesOfKind(const std::string& kind, const std::string& parent_id = "") = 0;
        virtual std::vector<std::string> FindNodesOfKinds(const std::vector<std::string>& kinds, const std::string& parent_id = "") = 0;
        virtual std::vector<std::string> FindEdges(const std::string& kind = "") = 0;
        virtual std::vector<std::string> FindImmediateChildren(const std::string& kind, const std::string& parent_id) = 0;
        virtual std::vector<std::string> FindImmediateChildren(const std::vector<std::string>& kinds, const std::string& parent_id) = 0;

        virtual std::string GetAttribute(const std::string& id, const std::string& attribute_name) = 0;
        virtual std::string GetDataValue(const std::string& id, const std::string& key_name) = 0;
        virtual std::string GetParentNode(const std::string& id, int depth) = 0;
        virtual std::string GetParentNode(const std::string& id) = 0;

        virtual std::string GetSharedParent(const std::string& left_child_id, const std::string& right_child_id) = 0;
        virtual int GetHeightToParent(const std::string& child_id, const std::string& parent_id) = 0;

};
#endif //GRAPHMLPARSERINT_H