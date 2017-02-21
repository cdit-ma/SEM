#include "attribute.h"

Attribute::Attribute(ATTRIBUTE_TYPE type, std::string name){
    name_ = name;
    type_ = type;
}

ATTRIBUTE_TYPE Attribute::get_type() const{
    return type_;
}

std::string Attribute::get_name() const{
    return name_;
}


void Attribute::set_String(std::string val){
    if(type_ == ATTRIBUTE_TYPE::STRING){
        string_ = val;
    }
}

std::string Attribute::get_String() const{
    return string_;
}

std::string& Attribute::String(){
    return string_;
}


void Attribute::set_Integer(int val){
    if(type_ == ATTRIBUTE_TYPE::INTEGER){
        integer_ = val;
    }
}

int Attribute::get_Integer() const{
    return integer_;
}

int& Attribute::Integer(){
    return integer_;
}

void Attribute::set_Double(double val){
    if(type_ == ATTRIBUTE_TYPE::DOUBLE){
        double_ = val;
    }
}

double Attribute::get_Double() const{
    return double_;
}

double& Attribute::Double(){
    return double_;
}

void Attribute::set_Boolean(bool val){
    if(type_ == ATTRIBUTE_TYPE::BOOLEAN){
        boolean_ = val;
    }
}

bool Attribute::get_Boolean() const{
    return boolean_;
}

bool& Attribute::Boolean(){
    return boolean_;
}


void Attribute::set_StringList(std::vector<std::string> val){
    if(type_ == ATTRIBUTE_TYPE::STRINGLIST){
        stringlist_ = val;
    }
}
std::vector<std::string> Attribute::get_StringList() const{
    return stringlist_;
}

std::vector<std::string>& Attribute::StringList(){
    return stringlist_;
}