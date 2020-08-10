#ifndef SQLITEDATABASE_H
#define SQLITEDATABASE_H

#include <string>
#include <queue>
#include <future>
#include <mutex>
#include <condition_variable>


class sqlite3_stmt;
class sqlite3;

class SQLiteDatabase{
    public:

        /// Creates sqlite database handle pointing to sqlite database at databaseFilepath
        SQLiteDatabase(const std::string& databaseFilepath);
        /// Creates "in memory" sqlite database
        SQLiteDatabase();
        ~SQLiteDatabase();
        
        sqlite3_stmt* GetSqlStatement(const std::string& query);
        //void QueueSqlStatement(sqlite3_stmt * statement);
        void ExecuteSqlStatement(sqlite3_stmt& statement, bool flush = false);
        size_t Flush();
        sqlite3* GetDatabase();
    private:
        size_t Flush_();
        sqlite3* database_ = 0;
        
        std::mutex mutex_;
        size_t transaction_count_ = 0;
};
#endif //SQLITEDATABASE_H
