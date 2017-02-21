#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <string>
#include <vector>
#include <map>

enum class ATTRIBUTE_TYPE{
    UNKNOWN = 0,
    STRING = 1,
    INTEGER = 2,
    BOOLEAN = 3,
    DOUBLE = 4,
    STRINGLIST = 5
};

class Attribute{
    public:
        Attribute(ATTRIBUTE_TYPE type, std::string name);

        ATTRIBUTE_TYPE get_type() const;
        std::string get_name() const;

        void set_String(std::string val);
        std::string get_String() const;
        std::string& String();
        
        void set_Integer(int val);
        int get_Integer() const;
        int& Integer();

        void set_Double(double val);
        double get_Double() const;
        double& Double();

        void set_Boolean(bool val);
        bool get_Boolean() const;
        bool& Boolean();

        void set_StringList(std::vector<std::string> val);
        std::vector<std::string> get_StringList() const;
        std::vector<std::string>& StringList();
    private:
        std::string name_;
        ATTRIBUTE_TYPE type_;
        std::string string_;
        int integer_;
        double double_;
        bool boolean_;
        std::vector<std::string> stringlist_;
};


#endif //ACTIVATABLE_H