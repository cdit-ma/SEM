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

#define SQL_BATCH_SIZE 100

const std::string BEGIN_TRANSACTION = "BEGIN TRANSACTION";
const std::string END_TRANSACTION = "END TRANSACTION";

SQLiteDatabase::SQLiteDatabase(const std::string& dbFilepath){
    //Open Database, create it if it's not there
    int result = sqlite3_open_v2(dbFilepath.c_str(), &database_, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);

    if(result != SQLITE_OK){
        throw std::runtime_error("SQLite Failed to Open Database");
    }

    writer_future = std::async(std::launch::async, &SQLiteDatabase::ProcessQueue, this);
}

SQLiteDatabase::~SQLiteDatabase(){
    {
        //Gain the lock so we can notify and set our terminate flag.
        std::unique_lock<std::mutex> lock(queue_mutex_);
        terminate_ = true;
        queue_lock_condition_.notify_all();
    }

    if(writer_future.valid()){
        writer_future.get();
    }
    
    int result = sqlite3_close_v2(database_);
    if(result != SQLITE_OK){
        std::cerr << "SQLite failed to close database" << std::endl;
    }
}

sqlite3_stmt* SQLiteDatabase::GetSqlStatement(const std::string& query){
    sqlite3_stmt *statement;
    int result = sqlite3_prepare_v2(database_, query.c_str(), -1, &statement, NULL);
    if(result == SQLITE_OK){
        return statement;
    }else{
        return 0;
    }
}

void SQLiteDatabase::QueueSqlStatement(sqlite3_stmt *sql){
    if(!sql){
        std::cerr << "Sqlite encountered NULL statement." << std::endl;
        return;
    }

    //Gain the conditional lock
    std::unique_lock<std::mutex> lock(queue_mutex_);
    //Add statement to the queue to process.
    sql_queue_.push(sql);

    //Only notify the queue above a certain size
    if(sql_queue_.size() > SQL_BATCH_SIZE){
        queue_lock_condition_.notify_all();
    }
}


void SQLiteDatabase::Flush(bool blocking){
    //Gain the conditional lock
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        if(blocking){
            blocking_flushed_ = false;
        }
        flush_ = true;
        //Notify the sql thread
        queue_lock_condition_.notify_all();
    }

    if(blocking){
        std::unique_lock<std::mutex> lock(queue_mutex_);
        flush_lock_condition_.wait(lock, [this]{return blocking_flushed_;});
    }
}

void SQLiteDatabase::ProcessQueue(){
    while(true){
        bool force_flush = false;
        bool terminate = false;
        std::queue<sqlite3_stmt*> sQueue;
        {
            //Wait for condition, if we don't have any SQL Statements.
            std::unique_lock<std::mutex> lock(queue_mutex_);
            queue_lock_condition_.wait(lock, [this]{return terminate_ || flush_;});
            
            terminate = terminate_;
            force_flush = terminate || flush_;
            //Unset the flush value
            flush_ = false;
            
            if(!sql_queue_.empty()){
                //Swap our queues, and release our lock
                sql_queue_.swap(sQueue);
            }
		}

        if(sQueue.size()){
            int result = sqlite3_exec(database_, BEGIN_TRANSACTION.c_str(), NULL, NULL, NULL);
        
            if(result != SQLITE_OK){
                std::cerr << "SQLite Failed to BEGIN TRANSACTION. ERROR: " << result << std::endl;            
            }

            //Execute Statements
            while(!sQueue.empty()){
                sqlite3_stmt* statement = sQueue.front();
                if(statement){
                    //Attempt to run the statement
                    result = sqlite3_step(statement);

                    if(result != SQLITE_DONE){
                        std::cerr << "SQLite Failed to step statement. ERROR: " << result << std::endl;
                    }

                    //Finalize deletes the statement object
                    result = sqlite3_finalize(statement);

                    if(result != SQLITE_OK){
                        std::cerr << "SQLite Failed to finalize statement. ERROR: " << result << std::endl;
                    }
                }
                sQueue.pop();
            }


            //End the transaction
            result = sqlite3_exec(database_, END_TRANSACTION.c_str(), NULL, NULL, NULL);

            if(result != SQLITE_OK){
                std::cerr << "SQLite Failed to END TRANSACTION. ERROR: " << result << std::endl;
            }
        }

        //Notify the Flush Function
        if(force_flush){
            std::unique_lock<std::mutex> lock(queue_mutex_);
            blocking_flushed_ = true;
            flush_lock_condition_.notify_all();
        }

        if(terminate){
            break;
        }
	}
}
