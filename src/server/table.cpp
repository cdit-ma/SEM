/* logan
 * Copyright (C) 2016-2017 The University of Adelaide
 *
 * This file is part of "logan"
 *
 * "logan" is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * "logan" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
 
#include "table.h"

#include "tableinsert.h"

#define CREATE_TABLE_PREFIX "CREATE TABLE IF NOT EXISTS"
#define INSERT_TABLE_PREFIX "INSERT INTO"
#define LID_INSERT "lid INTEGER PRIMARY KEY AUTOINCREMENT,"

Table::Table(SQLiteDatabase* database, std::string name){
    table_name_ = name;
    database_ = database;
    AddColumn("lid", "INTEGER");
}

Table::~Table(){
    for(auto i : columns_){
        delete i;
    }
}

bool Table::AddColumn(std::string name, std::string type){
    if(!finalized_){
        int pos = columns_.size();
        auto t = new TableColumn(pos, name, type);
        columns_.push_back(t);
        return true;
    }
    return false;
}

TableInsert Table::get_insert_statement(){
    //Prepare an object which allows setting and bind of values.
    return TableInsert(this);
}

int Table::get_field_id(std::string field){
    for(int i = 0; i < size_; i++){
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
        for(size_t i = 1; i < columns_.size(); i++){
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

sqlite3_stmt* Table::get_table_construct_statement(){
    size_ = columns_.size();
    if(table_create_.empty()){
        ConstructTableStatement();
    }
    return GetSqlStatement(table_create_);
}

sqlite3_stmt* Table::get_table_insert_statement(){
    auto s =GetSqlStatement(table_insert_);
    if(!s){
        if(!database_){
            std::cout << "NULL DATABSE?!: " << database_ << std::endl;    
        }
        std::cout << "NULL TABLE: " << table_insert_ << std::endl;
    }
    return s;
}

sqlite3_stmt* Table::GetSqlStatement(std::string query){
    return database_->GetSqlStatement(query);
}

void Table::Finalize(){
    if(!finalized_){
        finalized_ = true;
        if(table_insert_.empty()){
        std::stringstream top_ss;
        std::stringstream bottom_ss;
        //Create table
        top_ss << INSERT_TABLE_PREFIX << " " << table_name_ << " (";
        bottom_ss << " VALUES (";
        for(size_t i = 1; i < columns_.size(); i++){
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
}
