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
        void QueueSqlStatement(sqlite3_stmt * statement);
        void Flush(bool blocking = false);
    private:
        sqlite3* database_ = 0;
        void ProcessQueue();

        std::future<void> writer_future;

        std::queue<sqlite3_stmt*> sql_queue_;
        std::mutex queue_mutex_;
        std::condition_variable queue_lock_condition_;
        std::mutex flush_mutex_;
        std::condition_variable flush_lock_condition_;
        bool terminate_ = false;
        bool flush_ = false;
        bool blocking_flushed_ = false;
};
#endif //SQLITEDATABASE_H
