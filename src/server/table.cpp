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
    return TableInsert(this);
}

int Table::get_field_id(const std::string& field){
    int field_id = -1;
    for(auto i = 1; i < columns_.size(); i++){
        if(columns_[i]->column_name_ == field){
            field_id = columns_[i]->column_number_;
            break;
        }
    }
    return field_id;
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

sqlite3_stmt* Table::get_table_construct_statement(){
    size_ = columns_.size();
    if(table_create_.empty()){
        ConstructTableStatement();
    }
    return GetSqlStatement(table_create_);
}

sqlite3_stmt* Table::get_table_insert_statement(){
    return GetSqlStatement(table_insert_);
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
