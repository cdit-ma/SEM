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
 
#include "sqlitedatabase.h"

#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include "sqlite3.h"

#define SQL_BATCH_SIZE 1000

const std::string BEGIN_TRANSACTION = "BEGIN TRANSACTION;";
const std::string END_TRANSACTION = "END TRANSACTION;";

SQLiteDatabase::SQLiteDatabase(const std::string& dbFilepath){
    //Open Database, create it if it's not there
    int result = sqlite3_open(dbFilepath.c_str(), &database_);

    if(result != SQLITE_OK){
        throw std::runtime_error("SQLite Failed to Open Database");
    }
    result = sqlite3_exec(database_, "PRAGMA journal_mode = MEMORY;PRAGMA synchronous = OFF;", NULL, NULL, NULL);

    if(result != SQLITE_OK){
        throw std::runtime_error("SQLite Failed to optimize");
    }
}

SQLiteDatabase::~SQLiteDatabase(){
    std::unique_lock<std::mutex> lock(mutex_);
    //Flush any messages still in the queue
    Flush_();

    int result = sqlite3_close(database_);
    if(result != SQLITE_OK){
        std::cerr << "SQLite failed to close database" << std::endl;
    }
}

sqlite3_stmt* SQLiteDatabase::GetSqlStatement(const std::string& query){
    sqlite3_stmt* statement;

    int result = sqlite3_prepare_v2(database_, query.c_str(), -1, &statement, NULL);
    if(result == SQLITE_OK){
        return statement;
    }
    return nullptr;
}

void SQLiteDatabase::ExecuteSqlStatement(sqlite3_stmt& statement, bool flush){
    //Gain the conditional lock
    std::unique_lock<std::mutex> lock(mutex_);

    if(transaction_count_ == 0){
        auto result = sqlite3_exec(database_, BEGIN_TRANSACTION.c_str(), NULL, NULL, NULL);
        if(result != SQLITE_OK){
            std::cerr << "SQLite failed to BEGIN_TRANSACTION" << std::endl;
        }
    }

    {
        auto result = sqlite3_step(&statement);
        if(result != SQLITE_DONE){
            std::cerr << "SQLite failed to step statement" << std::endl;
            std::cerr << sqlite3_sql(&statement) << std::endl;
        }

        result = sqlite3_reset(&statement);
        if(result != SQLITE_OK){
            std::cerr << "SQLite failed to reset statement" << std::endl;
        }
        //result = sqlite3_clear_bindings(&statement);
        //if(result != SQLITE_OK){
            //std::cerr << "SQLite failed to clear bindings on statement" << std::endl;
        //}
        transaction_count_ ++;
    }

    if(flush || transaction_count_ > SQL_BATCH_SIZE){
        Flush_();
    }
}

void SQLiteDatabase::Flush(){
    //Gain the mutex and flush
    std::unique_lock<std::mutex> lock(mutex_);
    Flush_();
}

void SQLiteDatabase::Flush_(){
    if(transaction_count_){
        auto result = sqlite3_exec(database_, END_TRANSACTION.c_str(), NULL, NULL, NULL);
        if(result != SQLITE_OK){
            std::cerr << "SQLite failed to END_TRANSACTION" << std::endl;
        }
        transaction_count_ = 0;
    }
}