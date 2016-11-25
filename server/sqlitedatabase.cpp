#include "sqlitedatabase.h"
#include <stdexcept>
#include <iostream>
#include <stdio.h>

#define SQL_BATCH_SIZE 100

SQLiteDatabase::SQLiteDatabase(std::string dbFilepath){
    //Open Database, create it if it's not there
    int result = sqlite3_open_v2(dbFilepath.c_str(), &database_, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);

    if(result != SQLITE_OK){
        throw std::runtime_error("SQLite Failed to Open Database");
    }

    //Start a writer thread
    writer_thread_ = new std::thread(&SQLiteDatabase::ProcessQueue, this);
}

SQLiteDatabase::~SQLiteDatabase(){
    Flush();
    {
        //Gain the lock so we can notify and set our terminate flag.
        std::unique_lock<std::mutex> lock(queue_mutex_);
        terminate_ = true;
        queue_lock_condition_.notify_all();
    }

    //Join and then delete the thread
    writer_thread_->join();
    delete writer_thread_;
    
    int result = sqlite3_close_v2(database_);
    if(result != SQLITE_OK){
        std::cerr << "SQLite failed to close database" << std::endl;
    }
}

void SQLiteDatabase::QueueSqlStatement(sqlite3_stmt *sql){
    //Gain the conditional lock
    std::unique_lock<std::mutex> lock(queue_mutex_);
    //Add statement to the queue to process.
    sql_queue_.push(sql);

    //Only notify the queue above a certain size
    if(sql_queue_.size() > SQL_BATCH_SIZE){
        queue_lock_condition_.notify_all();
    }
}

void SQLiteDatabase::Flush(){
    //Gain the conditional lock
    std::unique_lock<std::mutex> lock(queue_mutex_);
    //Notify the sql thread
    queue_lock_condition_.notify_all();
}

void SQLiteDatabase::ProcessQueue(){
    while(!terminate_){
        std::queue<sqlite3_stmt*> sQueue;
        {
            //Wait for condition, if we don't have any SQL Statements.
            std::unique_lock<std::mutex> lock(queue_mutex_);
            queue_lock_condition_.wait(lock);
            
            if(!sql_queue_.empty()){
                //Swap our queues, and release our lock
                sql_queue_.swap(sQueue);
            }
		}

        if(sQueue.empty()){
            //Don't do anything
            continue;
        }

        int result = sqlite3_exec(database_, "BEGIN TRANSACTION", NULL, NULL, NULL);
        
        if(result != SQLITE_OK){
            throw std::runtime_error("SQLite Failed to BEGIN TRANSACTION");
        }

		//Execute Statements
		while(!sQueue.empty()){
            sqlite3_stmt* statement = sQueue.front();
            //Attempt to run the statement
            result = sqlite3_step(statement);

            
            if(result != SQLITE_DONE){
                std::cout << sqlite3_sql(statement) << std::endl;
                std::cout << "ERROR NO: " << result << std::endl;
                char  error[100];
                sprintf(error, "SQLite Failed to step statement %d\n", result);
                
                throw std::runtime_error(error);
            }

            //Finalize deletes the statement object
            result = sqlite3_finalize(statement);

            if(result != SQLITE_OK){
                throw std::runtime_error("SQLite Failed to finalize statement");
            }

			sQueue.pop();
		}


        //End the transaction
        result = sqlite3_exec(database_, "END TRANSACTION", NULL, NULL, NULL);

        if(result != SQLITE_OK){
            throw std::runtime_error("SQLite Failed to END TRANSACTION");
        }
	}
}


sqlite3_stmt* SQLiteDatabase::GetSqlStatement(std::string query){
    sqlite3_stmt *statement;
    int result = sqlite3_prepare_v2(database_, query.c_str(), -1, &statement, NULL);
    if(result == SQLITE_OK){
        return statement;
    }
    return 0;
}
