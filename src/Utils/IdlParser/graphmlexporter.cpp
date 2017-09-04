#include "graphmlexporter.h"


Graphml::Exporter::Exporter(bool pretty){
    this->pretty = pretty;
    begin();
};

void Graphml::Exporter::begin(){
    ss.clear();
    ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    export_new_line();

    std::stringstream attr_ss;
    attr_ss << "xmlns=\"http://graphml.graphdrawing.org/xmlns\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns  http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd\"";
    export_element(0, "graphml", attr_ss.str());
    export_new_line();
    
    export_key("kind", "string");
    export_key("index", "int");
    export_key("label", "string");
    export_key("type", "string");
    export_key("namespace", "string");
    export_key("key", "boolean");

    export_graph();
};


int Graphml::Exporter::export_edge(int edge_src, int edge_dst){
    std::stringstream attr_ss;
    attr_ss << "source=\"" << edge_src << "\" target=\"" << edge_dst << "\"";
    auto id = export_element(++id_count_, "edge", attr_ss.str());
    export_new_line(); 
    return id;
}

int Graphml::Exporter::export_key(std::string key_name, std::string key_type){
    if(!key_ids_.count(key_name)){
        std::stringstream attr_ss;
        attr_ss << "attr.name=\"" << key_name << "\" attr.type=\"" << key_type << "\"";

        auto id = export_element(++id_count_, "key", attr_ss.str(), true);
        export_new_line();
        key_ids_[key_name] = id;
        return id;
    }
    return -1;
};

int Graphml::Exporter::export_node(){
    auto id = export_element(++id_count_, "node");
    export_new_line(); 
    return id;
};

void Graphml::Exporter::export_data(std::string key_name, std::string key_value){
    auto key_id = get_key_id(key_name);

    std::stringstream attr_ss;
    attr_ss << "key=\"" << key_id << "\"";

    export_element(0, "data", attr_ss.str());
    //Attach data
    ss << key_value;
    //Close the data
    close_element_(false);
};

bool Graphml::Exporter::close_element(){
    return close_element_();
}
bool Graphml::Exporter::close_element_(bool tab){
    if(element_stack_.size()){
        auto element = element_stack_.top();
        element_stack_.pop();
        if(tab){
            export_tab();
        }
        ss << "</" << element << ">";
        export_new_line();
        return true;
    }
    return false;
};

int Graphml::Exporter::get_key_id(std::string key_name){
    auto key_id = -1;
    if(key_ids_.count(key_name)){
        key_id = key_ids_[key_name];
    }
    return key_id;
};

void Graphml::Exporter::close(){
    while(close_element());
};

std::string Graphml::Exporter::ToGraphml(){
    return ss.str();
}

void Graphml::Exporter::export_graph(){
    
    export_element(++id_count_, "graph", "edgedefault=\"directed\"");
    export_new_line();
};

int Graphml::Exporter::export_element(int id, std::string element, std::string attributes, bool one_line){
    export_tab();
    ss << "<" << element;
    
    //Export attributes
    if(attributes.length()){
        ss << " " << attributes;
    }

    if(id > 0){
        ss << " id=\"" << id << "\"";
    }

    if(one_line){
        ss << " /";
    }else{
        element_stack_.push(element);
    }

    ss << ">";
    return id;
};

void Graphml::Exporter::export_tab(){
    if(pretty){
        auto tab = get_tab();
        if(tab > 0){
            ss << std::string(tab, '\t');
        }
    }
};

void Graphml::Exporter::export_new_line(){
    if(pretty){
        ss << '\n';
    }
};

int Graphml::Exporter::get_tab(){
    return element_stack_.size();
}