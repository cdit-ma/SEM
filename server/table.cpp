#include "table.h"
#include "tableinsert.h"

#include "sqlitedatabase.h"

#define CREATE_TABLE_PREFIX "CREATE TABLE IF NOT EXISTS"
#define INSERT_TABLE_PREFIX "INSERT INTO"
#define LID_INSERT "lid INTEGER PRIMARY KEY AUTOINCREMENT,"

Table::Table(sqlite3* database, std::string name){
    table_name_ = name;
    database_ = database;
    AddColumn("lid", "INTEGER");
}

void Table::AddColumn(std::string name, std::string type){
    int pos = columns_.size();
    auto t = new TableColumn(pos, name, type);
    columns_.push_back(t);
}

TableInsert* Table::get_insert_statement(){
    //Prepare an object which allows setting and bind of values.
    return new TableInsert(this);
}

int Table::get_field_id(std::string field){

    auto j = 100000;
    auto t1 = std::chrono::system_clock::now();

    for(int f = 0; f < j; f++){
        for(int i = 0; i < columns_.size(); i++){
            if(field == columns_.at(i)->column_name_){
                //return columns_.at(i)->column_number_;
            }
        }
    }
    auto t2 = std::chrono::system_clock::now();
    for(int f = 0; f < j; f++){
        for(int i = 0; i < columns_.size(); i++){
            if(field == columns_[i]->column_name_){
                //return columns_[i]->column_number_;
            }
        }
    }

    auto t3 = std::chrono::system_clock::now();
    for(int f = 0; f < j; f++){
        for(auto c: columns_){
        if(field == c->column_name_){

            }
        }
    }
    auto t4 = std::chrono::system_clock::now();

    int size = columns_.size();
    for(int f = 0; f < j; f++){
        for(int i = 0; i < size; i++){
            if(field == columns_[i]->column_name_){
                //return columns_[i]->column_number_;
            }
        }
    }
    auto t5 = std::chrono::system_clock::now();


    auto t_1 = (t2 - t1);
    auto t_2 = (t3 - t2);
    auto t_3 = (t4 - t3);
    auto t_4 = (t5 - t4);


    std::cout << "\t FOR AT:" << t_1.count()/j << " μs" << std::endl;
    std::cout << "\t FOR []:" << t_2.count()/j << " μs" << std::endl;
    std::cout << "\t FOREACH:" << t_3.count()/j << " μs" << std::endl;
    std::cout << "\t FOR [] CACHED SIZE:" << t_4.count()/j << " μs" << std::endl;
    



    for(int i = 0; i < columns_.size(); i++){
        if(field == columns_[i]->column_name_){
            return columns_[i]->column_number_;
        }
    }






    std::cerr << "Can't find field '" << field << "' in table: '" << table_name_ << std::endl;
    return -1;
}

void Table::ConstructTableStatement(){
    if(table_create_.empty()){
        std::stringstream ss;
        //Create table
        ss << CREATE_TABLE_PREFIX << " " << table_name_ << " (";
        ss << LID_INSERT << " ";
        for(int i = 1; i < columns_.size(); i++){
            auto c = columns_[i];
            ss << c->column_name_ << " " << c->column_type_;

            if(i + 1 != columns_.size()){
                ss << ", ";
            }
        }
        ss << ");";
        table_create_ = ss.str();
    }
}

void Table::InsertTableStatement(){
    if(table_insert_.empty()){
        std::stringstream top_ss;
        std::stringstream bottom_ss;
        //Create table
        top_ss << INSERT_TABLE_PREFIX << " " << table_name_ << " (";
        bottom_ss << " VALUES (";
        for(int i = 1; i < columns_.size(); i++){
            auto c = columns_[i];
            top_ss << c->column_name_;
            bottom_ss << "?" << std::to_string(i);
            if(i + 1 != columns_.size()){
                top_ss << ", ";
                bottom_ss << ", ";
            }
        }
        top_ss << ") ";
        bottom_ss << ");";
        table_insert_ = top_ss.str() + bottom_ss.str();
    }
}

sqlite3_stmt* Table::get_table_construct_statement(){
    if(table_create_.empty()){
        ConstructTableStatement();
    }
    return GetSqlStatement(table_create_);
}

sqlite3_stmt* Table::get_table_insert_statement(){
    if(table_insert_.empty()){
        InsertTableStatement();
    }
    return GetSqlStatement(table_insert_);
}

sqlite3_stmt* Table::GetSqlStatement(std::string query){
    sqlite3_stmt *statement;
    int result = sqlite3_prepare_v2(database_, query.c_str(), -1, &statement, NULL);
    if(result == SQLITE_OK){
        return statement;
    }
    return 0;
}