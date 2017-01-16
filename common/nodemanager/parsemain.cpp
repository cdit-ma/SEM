#include "graphmlparser.h"


#include <string>
#include <iostream>

#include <map>

/*struct graphml{
    void load(const std::string &filename);
};

pugi::xpath_node_set get_nodes(std::string attr_name, std::string attr_value){

}

void graphml::load(const std::string &filename){
    pugi::xml_document doc;

    auto result = doc.load_file(filename.c_str());

    std::cout << result.description() << std::endl;


    //Build map of keys
    std::map<std::string, std::string> attribute_map;
    auto keys = doc.select_nodes("/graphml/key");
    for(auto key : keys){
        std::string name = key.node().attribute("attr.name").value();
        std::string id = key.node().attribute("id").value();

        attribute_map[name] = id;
    }

    //Find all component instances
    std::string component_inst_search = ".//node[data[@key='" + attribute_map["kind"] + "' and .='ComponentInstance']]";
    auto component_instances = doc.select_nodes(component_inst_search.c_str());

    for(auto i : component_instances){
        std::cout << i.node().attribute("id").value() << std::endl;
    }

    //Find all hardware
    std::string hardware_search = ".//node[data[@key='" + attribute_map["kind"] + "' and .='HardwareNode']]";
    auto hardware_nodes = doc.select_nodes(hardware_search.c_str());

    for(auto i : hardware_nodes){
        std::cout << i.node().attribute("id").value() << std::endl;
    }


    //Map all deployments
    std::string deployment_search = ".//edge[data[@key='" + attribute_map["kind"] + "' and .='Edge_Deployment']]";
    auto deployment_edges = doc.select_nodes(deployment_search.c_str());

    std::multimap<std::string, std::string> deployment_map;
    for(auto i : deployment_edges){
        std::string target = i.node().attribute("target").value();
        std::string source = i.node().attribute("source").value();
        std::pair<std::string, std::string> p(target, source);
        deployment_map.insert(p);
    }

    auto ret = deployment_map.equal_range("287");

    for(auto it=ret.first; it!=ret.second; it++){
        std::cout << it->second << std::endl;
    }

};*/

int main(int argc, char** argv){

    GraphmlParser g;
    std::cout << argv[1] << std::endl;
    g.load(argv[1]);

    for(auto i : g.find_nodes("ComponentInstance")){
        std::cout << i << std::endl;
    }

    for(auto i : g.find_edges("Edge_Deployment")){
        std::cout << i << std::endl;
    }

    std::cout << g.get_attribute(g.find_edges("Edge_Deployment")[0], "source") << std::endl;;
    std::cout << g.get_data_value(g.find_edges("Edge_Deployment")[0], "kind") << std::endl;;

}