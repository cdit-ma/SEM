#ifndef GRAPHMLPARSERINT_H
#define GRAPHMLPARSERINT_H

#include <string>
#include <vector>

class GraphmlParserInt{
    public:
        GraphmlParserInt(const std::string& filename){};
        virtual bool IsValid() = 0;
        
        virtual std::vector<std::string> FindNodes(const std::string& kind, const std::string& parent_id = "") = 0;
        virtual std::vector<std::string> FindEdges(const std::string& kind = "") = 0;
        virtual std::vector<std::string> FindImmediateChildren(const std::string& kind, const std::string& parent_id) = 0;

        virtual std::string GetAttribute(const std::string& id, const std::string& attribute_name) = 0;
        virtual std::string GetDataValue(const std::string& id, const std::string& key_name) = 0;
        virtual std::string GetParentNode(const std::string& id, int depth) = 0;
        virtual std::string GetParentNode(const std::string& id) = 0;
};
#endif //GRAPHMLPARSERINT_H