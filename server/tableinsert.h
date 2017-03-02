#ifndef LOGAN_TABLEINSERT_H
#define LOGAN_TABLEINSERT_H

#include <string>
#include "sqlite3.h"
class Table;

class TableInsert{
    public:   
        TableInsert(Table* table);

        int BindString(std::string field, std::string val);
        int BindInt(std::string field, int val);
        int BindDouble(std::string field, double val);

        sqlite3_stmt* get_statement();
    private:
        sqlite3_stmt* stmt_;
        Table* table_;        
};
#endif //LOGAN_TABLEINSERT_H
