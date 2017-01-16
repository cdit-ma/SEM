#include <pugixml.hpp>

#include <string>
#include <iostream>

struct graphml{
    void load(const std::string &filename);
};

void graphml::load(const std::string &filename){
    pugi::xml_document doc;

    auto result = doc.load_file(filename.c_str());

    std::cout << result.description() << std::endl;

    auto keys = doc.select_nodes("/graphml/key");

    for(auto key : keys){
        std::string name = key.node().attribute("attr.name").value();
        std::string id = key.node().attribute("id").value();

        std::cout << id << " : " << name << std::endl;

    }

    auto component_instances = doc.select_nodes(".//node[data[@key='2' and .='ComponentInstance']]");

    for(auto i : component_instances){
        std::cout << i.node().attribute("id").value() << std::endl;
    }
};

int main(int argc, char** argv){

    graphml g;
    std::cout << argv[1]<<std::endl;
    g.load(argv[1]);

}