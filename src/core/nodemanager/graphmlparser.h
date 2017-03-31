#ifndef GRAPHMLPARSER_H
#define GRAPHMLPARSER_H

#include <pugixml.hpp>
#include <string>
#include <vector>
#include <map>


class GraphmlParser{

    public:
        GraphmlParser(const std::string filename);
        std::vector<std::string> FindNodes(std::string kind, std::string parent_id = "");
        std::vector<std::string> FindEdges(std::string kind);

        std::string GetAttribute(std::string id, std::string attribute_name);
        std::string GetDataValue(std::string id, std::string key_name);

        std::string GetParentNode(std::string id);
        

    private:
        std::map<std::string, std::string> attribute_map_;

        pugi::xml_document doc;


};
#endif //"GRAPHMLPARSER_H"