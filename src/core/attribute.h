#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

enum class ATTRIBUTE_TYPE{
    STRING = 0,
    INTEGER = 1,
    CHARACTER = 2,
    BOOLEAN = 3,
    DOUBLE = 4,
    FLOAT = 5,
    STRINGLIST = 6
};

class Attribute{
    public:
        Attribute(const ATTRIBUTE_TYPE type, const std::string name);

        ATTRIBUTE_TYPE get_type() const;
        std::string get_name() const;

        void set_String(const std::string& val);
        std::string get_String() const;
        std::string& String();
        
        void set_Integer(const int val);
        int get_Integer() const;
        int& Integer();

        void set_Character(const char val);
        char get_Character() const;
        char& Character();

        void set_Double(const double val);
        double get_Double() const;
        double& Double();

        void set_Float(const float val);
        float get_Float() const;
        float& Float();

        void set_Boolean(const bool val);
        bool get_Boolean() const;
        bool& Boolean();

        void set_StringList(const std::vector<std::string>& val);
        std::vector<std::string> get_StringList() const;
        std::vector<std::string>& StringList();

        template<class PrimitiveType>
        void SetValue(const PrimitiveType& type);

        template<class PrimitiveType>
        PrimitiveType& Value();
        
    private:
        std::string name_;
        ATTRIBUTE_TYPE type_;
        std::string string_;
        int integer_ = 0;
        double double_ = 0;
        bool boolean_ = false;
        char character_ = 0;
        float float_ = 0;
        std::vector<std::string> stringlist_;
};

//Template Specialisation
template<> void Attribute::SetValue<std::string>(const std::string& value);
template<> void Attribute::SetValue<int>(const int& value);
template<> void Attribute::SetValue<char>(const char& value);
template<> void Attribute::SetValue<bool>(const bool& value);
template<> void Attribute::SetValue<double>(const double& value);
template<> void Attribute::SetValue<float>(const float& value);
template<> void Attribute::SetValue<std::vector<std::string>>(const std::vector<std::string>& value);

//Template Specialisation
template<> std::string& Attribute::Value<std::string>();
template<> int& Attribute::Value<int>();
template<> char& Attribute::Value<char>();
template<> bool& Attribute::Value<bool>();
template<> double& Attribute::Value<double>();
template<> float& Attribute::Value<float>();
template<> std::vector<std::string>& Attribute::Value<std::vector<std::string>>();

#endif //ACTIVATABLE_H