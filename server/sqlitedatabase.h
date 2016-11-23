#ifndef SQLITEDATABASE_H
#define SQLITEDATABASE_H

#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "sqlite3.h"

class SQLiteDatabase{
    public:
        SQLiteDatabase(std::string databaseFilepath);
        virtual ~SQLiteDatabase();
        
        void queue_sql_statement(sqlite3_stmt * statement);
        void flush();
        
        sqlite3_stmt* get_sql_statement(std::string query);
    private:
        void process_queue();
        sqlite3 *database;

        std::thread* writerThread_;

        std::queue<sqlite3_stmt*> sqlQueue_;
        std::mutex queueMutex_;
        std::condition_variable queueLockCondition_;
        bool terminate_;
};
#endif
