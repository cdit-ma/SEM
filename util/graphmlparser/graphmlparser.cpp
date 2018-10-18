#include "graphmlparser.h"
#include <pugixml.hpp>

#include <iostream>

GraphmlParser::GraphmlParser(const std::string& filename) : GraphmlParserInt(filename){
    auto result = doc.load_file(filename.c_str());
    legal_parse = result.status == pugi::status_ok;;
    if(!legal_parse){
        throw std::runtime_error("GraphmlParser:Parse(" + filename + ") Error: " + result.description());
    }

    //Pre fill attribute map
    for(auto& key : doc.select_nodes("/graphml/key")){
        const auto &name = key.node().attribute("attr.name").value();
        const auto &id = key.node().attribute("id").value();
        attribute_map_[name] = id;
    }

    //Pre fill id map
    for(auto& xpath_node : doc.select_nodes("//*[@id]")){
        auto node = xpath_node.node();
        auto id = node.attribute("id").value();
        id_lookup_[id] = node;
    }
}

std::vector<std::string> GraphmlParser::FindNodes(const std::string& kind, const std::string& parent_id){
    auto search_node = doc.document_element();

    if(parent_id.length() > 0 && id_lookup_.count(parent_id)){
        search_node = id_lookup_[parent_id];
    }
    //Infinite depth
    std::string search = ".//node/data[@key='" + attribute_map_["kind"] + "' and .='" + kind +"']/..";
    std::vector<std::string> out;

    for(auto& n : search_node.select_nodes(search.c_str())){
        out.emplace_back(n.node().attribute("id").value());
    }
    return out;
}

std::vector<std::string> GraphmlParser::FindImmediateChildren(const std::string& kind, const std::string& parent_id){
    std::vector<std::string> out;

    if(parent_id.length() > 0 && id_lookup_.count(parent_id)){
        auto search_node = id_lookup_[parent_id];
        std::string search = "./graph/node/data[@key='" + attribute_map_["kind"] + "' and .='" + kind +"']/..";

        for(auto& n : search_node.select_nodes(search.c_str())){
            out.emplace_back(n.node().attribute("id").value());
        }
    }
    return out;
}


std::vector<std::string> GraphmlParser::FindEdges(const std::string& kind){
    std::string search = ".//edge";
    
    if(!kind.empty()){
        search += "[data[@key='" + attribute_map_["kind"] + "' and .='" + kind +"']]";
    }

    std::vector<std::string> out;

    for(auto& n : doc.select_nodes(search.c_str())){
        out.emplace_back(n.node().attribute("id").value());
    }
    return out;
}

std::string GraphmlParser::GetAttribute(const std::string& id, const std::string& attribute_name){

    std::string key = id + "|" + attribute_name;

    if(attr_lookup_.count(key)){
        return attr_lookup_[key];
    }else{
        std::string out;
        
        if(id_lookup_.count(id)){
            auto& entity = id_lookup_[id];
            if(entity){
                auto attribute = entity.attribute(attribute_name.c_str());
                if(attribute){
                    out = attribute.value();
                    attr_lookup_[key] = out;
                }else{
                    std::cerr << "GraphmlParser: Entity with ID: '" << id << "' doesn't have an attribute: '" << attribute_name << "'" << std::endl;
                }
            }
        }else{
            std::cerr << "GraphmlParser: No entity with ID: '" << id << "'" << std::endl;
        }
        return out;
    }
}

std::string GraphmlParser::GetDataValue(const std::string& id, const std::string& key_name){
    std::string key = id + "|" + key_name;

    if(data_lookup_.count(key)){
        return data_lookup_[key];
    }else{
        std::string out;
        
        if(id_lookup_.count(id)){
            auto& entity = id_lookup_[id];
            if(entity){
                auto search = "data[@key='" + attribute_map_[key_name] + "']";

                auto res = entity.select_node(search.c_str());
                auto node = res.node();

                if(node){
                    out = node.child_value();
                    data_lookup_[id] = out;
                }
            }
        }else{
            std::cerr << "GraphmlParser: No entity with ID: '" << id << "'" << std::endl;
        }
        return out;
    }
}


std::string GraphmlParser::GetParentNode(const std::string& id, int depth){
    std::string parent_id = id;
    
    while(depth != 0){
        parent_id = GetParentNode(parent_id);
        depth--;
    }
    
    return parent_id;
}

std::string GraphmlParser::GetParentNode(const std::string& id){
    if(parent_id_lookup_.count(id)){
        return parent_id_lookup_[id];
    }else{
        std::string out;
        if(id_lookup_.count(id)){
            auto search_node = id_lookup_[id];
            auto res = search_node.select_node("../..");
            out = res.node().attribute("id").value();
            parent_id_lookup_[id] = out;
        }else{
            std::cerr << "GraphmlParser: No entity with ID: '" << id << "'" << std::endl;
        }
        return out;
    }
}


std::string GraphmlParser::GetSharedParent(const std::string& left_child_id, const std::string& right_child_id){
    std::string key = left_child_id + "|" + right_child_id;

    if(!shared_parent_map_.count(key)){
        std::string shared_parent_id;
        std::string expr(".//node[@id=\"" + left_child_id + "\"]/ancestor::* [count(. | .//node[@id=\""  + right_child_id + "\"]/ancestor::*) = count(.//node[@id=\""  + right_child_id + "\"]/ancestor::*)][1]/..");

        auto result = doc.select_node(expr.c_str());
        if(result && result.node()){
            shared_parent_map_.insert({key, result.node().attribute("id").value()});
        }
    }
    return shared_parent_map_[key];
}

int GraphmlParser::GetHeightToParent(const std::string& child_id, const std::string& parent_id){
    std::string key = child_id + "|" + parent_id;
    int depth = 0;

    if(!parent_height_depth_map_.count(key)){
        auto current_id = child_id;
        while(!current_id.empty()){
            depth ++;
            current_id = GetParentNode(current_id);
            if(current_id == parent_id){
                break;
            }
        }

        if(current_id.empty()){
            depth = -1;
        }
        parent_height_depth_map_[key] = depth;
    }
    return parent_height_depth_map_[key];
}


