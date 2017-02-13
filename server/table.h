#ifndef LOGAN_TABLE_H
#define LOGAN_TABLE_H

#include <iostream>
#include <sstream>
#include "sqlitedatabase.h"

class TableInsert;

struct TableColumn{
    public:
        TableColumn(int column, std::string name, std::string type){
            column_number_ = column;
            column_name_ = name;
            column_type_ = type;
        }
        std::string column_name_;
        std::string column_type_;
        int column_number_;
};

class Table{
    public:
        Table(SQLiteDatabase* database, std::string name);
        void AddColumn(std::string name, std::string type);
        TableInsert* get_insert_statement();

        //OPTIMIZE PRIME FOR MITCH
        int get_field_id(std::string field);
        sqlite3_stmt* get_table_construct_statement();
        sqlite3_stmt* get_table_insert_statement();

    private:
        std::string table_name_;
        int size_;
        std::string table_insert_;

        SQLiteDatabase* database_;

        std::vector<TableColumn*> columns_;
        std::string table_create_;
        std::stringstream insert_ss;
        void ConstructTableStatement();
        void InsertTableStatement();
        sqlite3_stmt* GetSqlStatement(std::string query);



};

#endif //LOGAN_TABLE_H
