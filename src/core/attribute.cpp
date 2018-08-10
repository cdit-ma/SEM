#include "attribute.h"

Attribute::Attribute(const ATTRIBUTE_TYPE type, const std::string name){
    name_ = name;
    type_ = type;
}

ATTRIBUTE_TYPE Attribute::get_type() const{
    return type_;
}

std::string Attribute::get_name() const{
    return name_;
}


void Attribute::set_String(const std::string val){
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

void Attribute::set_Integer(const int val){
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

void Attribute::set_Character(const char val){
    if(type_ == ATTRIBUTE_TYPE::CHARACTER){
        character_ = val;
    }
}
char Attribute::get_Character() const{
    return character_;
}
char& Attribute::Character(){
    return character_;
}

void Attribute::set_Double(double val){
    if(type_ == ATTRIBUTE_TYPE::DOUBLE){
        double_ = val;
    }
}

void Attribute::set_Float(const float val){
    if(type_ == ATTRIBUTE_TYPE::FLOAT){
        float_ = val;
    }
}
float Attribute::get_Float() const{
    return float_;
}
float& Attribute::Float(){
    return float_;
}

double Attribute::get_Double() const{
    return double_;
}

double& Attribute::Double(){
    return double_;
}

void Attribute::set_Boolean(const bool val){
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


void Attribute::set_StringList(const std::vector<std::string> val){
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