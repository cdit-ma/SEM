#ifndef LOGAN_TABLE_H
#define LOGAN_TABLE_H

#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include "sqlitedatabase.h"

class TableInsert;

struct TableColumn{
    public:
        TableColumn(int column, const std::string& name, const std::string& type){
            column_number_ = column;
            column_name_ = name;
            column_type_ = type;
        }
        std::string column_name_;
        std::string column_type_;
        int column_number_;
};

class Table{
    friend TableInsert;
    public:
        Table(SQLiteDatabase& database, const std::string& name);
        ~Table();
        bool AddColumn(const std::string& name, const std::string& type);
        TableInsert get_insert_statement();

        int get_field_id(const std::string& field);
        sqlite3_stmt& get_table_construct_statement();

        void Finalize();
    protected:
        void free_table_insert_statement(sqlite3_stmt* stmnt);
        sqlite3_stmt* get_table_insert_statement();
    private:
        SQLiteDatabase& database_;

        int size_;
        bool finalized_ = false;

        std::string table_name_;
        std::string table_insert_;

        std::unordered_map<std::string, int> column_lookup_;
        std::vector<TableColumn*> columns_;
        std::string table_create_;
        
        void ConstructTableStatement();
        sqlite3_stmt* GetSqlStatement(const std::string& query);

        std::mutex insert_mutex_;
        std::queue<sqlite3_stmt*> insert_pool_;

        
        sqlite3_stmt* table_construct_ = 0;
};
#endif //LOGAN_TABLE_H
