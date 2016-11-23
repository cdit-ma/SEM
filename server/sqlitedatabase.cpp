#include "sqlitedatabase.h"
#include <stdexcept>
#include <iostream>
#include <stdio.h>

#define SQL_BATCH_SIZE 100

SQLiteDatabase::SQLiteDatabase(std::string dbFilepath){
    database = 0;
    terminate_ = false;

    //Open Database, create it if it's not there
    int result = sqlite3_open_v2(dbFilepath.c_str(), &database, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);

    if(result != SQLITE_OK){
        throw std::runtime_error("SQLite Failed to Open Database");
    }

    //Start a writer thread
    writerThread_ = new std::thread(&SQLiteDatabase::process_queue, this);
}

SQLiteDatabase::~SQLiteDatabase(){
    flush();
    {
        //Gain the lock so we can notify and set our terminate flag.
        std::unique_lock<std::mutex> lock(queueMutex_);
        terminate_ = true;
        queueLockCondition_.notify_all();
    }

    //Join and then delete the thread
    writerThread_->join();
    delete writerThread_;
    
    int result = sqlite3_close_v2(database);
    if(result != SQLITE_OK){
        std::cerr << "SQLite failed to close database" << std::endl;
    }
}

void SQLiteDatabase::queue_sql_statement(sqlite3_stmt *sql){
    //Gain the conditional lock
    std::unique_lock<std::mutex> lock(queueMutex_);
    //Add statement to the queue to process.
    sqlQueue_.push(sql);

    //Only notify the queue above a certain size
    if(sqlQueue_.size() > SQL_BATCH_SIZE){
        queueLockCondition_.notify_all();
    }
}

void SQLiteDatabase::flush(){
    //Gain the conditional lock
    std::unique_lock<std::mutex> lock(queueMutex_);
    //Notify the sql thread
    queueLockCondition_.notify_all();
}

void SQLiteDatabase::process_queue(){
    while(!terminate_){
        std::queue<sqlite3_stmt*> sQueue;
        {
            //Wait for condition, if we don't have any SQL Statements.
            std::unique_lock<std::mutex> lock(queueMutex_);
            queueLockCondition_.wait(lock);
            
            if(!sqlQueue_.empty()){
                //Swap our queues, and release our lock
                sqlQueue_.swap(sQueue);
            }
		}

        if(sQueue.empty()){
            //Don't do anything
            continue;
        }

        int result = sqlite3_exec(database, "BEGIN TRANSACTION", NULL, NULL, NULL);
        
        if(result != SQLITE_OK){
            throw std::runtime_error("SQLite Failed to BEGIN TRANSACTION");
        }

		//Execute Statements
		while(!sQueue.empty()){
            sqlite3_stmt* statement = sQueue.front();
            //Attempt to run the statement
            result = sqlite3_step(statement);

            
            if(result != SQLITE_DONE){
                std::cout << "ERROR NO: " << result << std::endl;
                char  error[100];
                std::cout << sqlite3_sql(statement) << std::endl;
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
        result = sqlite3_exec(database, "END TRANSACTION", NULL, NULL, NULL);

        if(result != SQLITE_OK){
            throw std::runtime_error("SQLite Failed to END TRANSACTION");
        }
	}
}


sqlite3_stmt* SQLiteDatabase::get_sql_statement(std::string query){
    sqlite3_stmt *statement;
    int result = sqlite3_prepare_v2(database, query.c_str(), -1, &statement, NULL);
    if(result == SQLITE_OK){
        return statement;
    }
    return 0;
}