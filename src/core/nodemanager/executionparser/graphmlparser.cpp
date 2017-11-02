#include "graphmlparser.h"
#include <pugixml.hpp>

#include <iostream>

GraphmlParser::GraphmlParser(std::string filename){
    auto result = doc.load_file(filename.c_str());

    auto keys = doc.select_nodes("/graphml/key");
    for(auto key : keys){
        std::string name = key.node().attribute("attr.name").value();
        std::string id = key.node().attribute("id").value();

        attribute_map_[name] = id;
    }
}

std::vector<std::string> GraphmlParser::FindNodes(std::string kind, std::string parent_id){

    std::string search = ".//node";

    if(parent_id.length() > 0){
        search += "[@id='"+ parent_id + "']/";
    }
    search += "/data[@key='" + attribute_map_["kind"] + "' and .='" + kind +"']/..";
    std::vector<std::string> out;

    for(auto n : doc.select_nodes(search.c_str())){
        out.push_back(n.node().attribute("id").value());
    }
    return out;
}

std::vector<std::string> GraphmlParser::FindEdges(std::string kind){
    std::string search = ".//edge";
    
    if(kind != ""){
        search += "[data[@key='" + attribute_map_["kind"] + "' and .='" + kind +"']]";
    }

    std::vector<std::string> out;

    for(auto n : doc.select_nodes(search.c_str())){
        out.push_back(n.node().attribute("id").value());
    }
    return out;
}

std::string GraphmlParser::GetAttribute(std::string id, std::string attribute_name){

    std::string key = id + "|" + attribute_name;

    if(attr_lookup_.count(key)){
        return attr_lookup_[key];
    }else{
        std::string search = ".//*[@id='" + id + "']";
        std::string out;
        try{
            auto res = doc.select_node(search.c_str());
            out = res.node().attribute(attribute_name.c_str()).value();
        } catch(...){}
        attr_lookup_[key] = out;
        return out;
    }
}

std::string GraphmlParser::GetDataValue(std::string id, std::string key_name){
    std::string key = id + "|" + key_name;

    if(data_lookup_.count(key)){
        return data_lookup_[key];
    }else{
        std::string search = ".//*[@id='" + id + "']/data[@key='" + attribute_map_[key_name] + "']";
        std::string out;
        try{
            auto res = doc.select_node(search.c_str());
            out = res.node().child_value();
        } catch(...){}
        data_lookup_[key] = out;
        return out;
    }
}

std::string GraphmlParser::GetParentNode(std::string id){
    if(parent_id_lookup_.count(id)){
        return parent_id_lookup_[id];
    }else{
        std::string search = ".//*[@id='" + id + "']/../..";
        std::string out;
        bool success = false;
        try{
            auto res = doc.select_node(search.c_str());
            out = res.node().attribute("id").value();
            success = true;
        }catch(...){
            std::cout << "ERROR" << std::endl;
        }

        if(success){
            parent_id_lookup_[id] = out;
        }
        return out;
    }
}
