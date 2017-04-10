#include "modelparser.h"
#include <string>
#include <iostream>
#include "datatypes.h"
int main(int argc, char **argv){
    std::string str("HelloWorld.graphml");
    Graphml::ModelParser p(str);

    std::string node_name("Dan");
    auto node = p.GetHardwareNodeByName(node_name);
    if(node && node->component_ids.size() > 0){
        std::cout << "Node: " << node_name << " Deploys Components!" << std::endl;
    }
};