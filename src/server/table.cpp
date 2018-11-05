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
#include "sqlite3.h"

#define CREATE_TABLE_PREFIX "CREATE TABLE IF NOT EXISTS"
#define INSERT_TABLE_PREFIX "INSERT INTO"
#define LID_INSERT "lid INTEGER PRIMARY KEY AUTOINCREMENT,"

Table::Table(SQLiteDatabase& database, const std::string& name):
    database_(database)
{
    table_name_ = name;
    AddColumn("lid", "INTEGER");
}

Table::~Table(){
    for(const auto& i : columns_){
        delete i;
    }

    sqlite3_finalize(table_construct_);

    std::lock_guard<std::mutex> lock(insert_mutex_);
    
    while(insert_pool_.size()){
        sqlite3_finalize(insert_pool_.front());
        insert_pool_.pop();
    }
}

bool Table::AddColumn(const std::string& name, const std::string& type){
    bool success = false;
    if(!finalized_){
        auto t = new TableColumn(columns_.size(), name, type);
        if(!column_lookup_.count(name)){
            column_lookup_[name] = t->column_number_;
            columns_.push_back(t);
            success = true;
        }else{
            std::cerr << table_name_ << " has duplicate column '" << name << "'" << std::endl;
        }
    }
    return success;
}

TableInsert Table::get_insert_statement(){
    //Prepare an object which allows setting and bind of values.
    return TableInsert(*this);
}

int Table::get_field_id(const std::string& field){
    try{
        return column_lookup_.at(field);
    }catch(const std::exception& ex){
        return -1;
    }
}

void Table::ConstructTableStatement(){
    if(table_create_.empty()){
        std::stringstream ss;
        //Create table
        ss << CREATE_TABLE_PREFIX << " " << table_name_ << " (";
        ss << LID_INSERT << " ";
        //Ignore the LID_INSERT
        for(auto i = 1; i < columns_.size(); i++){
            ss << columns_[i]->column_name_ << " " << columns_[i]->column_type_;
            if(i + 1 != columns_.size()){
                ss << ", ";
            }
        }
        ss << ");";
        table_create_ = ss.str();
    }
}

sqlite3_stmt& Table::get_table_construct_statement(){
    if(!table_construct_){
        size_ = columns_.size();
        ConstructTableStatement();
        table_construct_ = GetSqlStatement(table_create_);
    }
    return *table_construct_;
}

sqlite3_stmt* Table::GetSqlStatement(const std::string& query){
    return database_.GetSqlStatement(query);
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

        for(auto i = 1; i < columns_.size(); i++){
            top_ss << columns_[i]->column_name_;
            //bottom_ss << "?" << std::to_string(i);
            bottom_ss << ":" << columns_[i]->column_name_;
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

void Table::free_table_insert_statement(sqlite3_stmt* stmt){
    std::lock_guard<std::mutex> lock(insert_mutex_);
    insert_pool_.emplace(stmt);
}

sqlite3_stmt* Table::get_table_insert_statement(){
    sqlite3_stmt* stmt = 0;
    std::lock_guard<std::mutex> lock(insert_mutex_);

    if(insert_pool_.size() == 0){
        stmt = GetSqlStatement(table_insert_);
    }else{
        stmt = insert_pool_.front();
        insert_pool_.pop();
    }

    return stmt;
}