#ifndef LOGAN_TABLEINSERT_H
#define LOGAN_TABLEINSERT_H

#include <string>

class Table;
class sqlite3_stmt;

class TableInsert{
    public:   
        TableInsert(Table& table);
        ~TableInsert();

        int BindString(const std::string& field, const std::string& val);
        int BindInt(const std::string& field, const int64_t& val);
        int BindDouble(const std::string& field, const double& val);
        sqlite3_stmt& get_statement();
    private:
        int GetFieldIndex(const std::string& field);
        sqlite3_stmt* stmt_ = 0;
        Table& table_;        
};
#endif //LOGAN_TABLEINSERT_H
