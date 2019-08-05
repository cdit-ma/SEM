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
        SQLiteDatabase(const std::string& databaseFilepath);
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
