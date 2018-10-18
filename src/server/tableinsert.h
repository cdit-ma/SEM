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
 
#ifndef LOGAN_TABLEINSERT_H
#define LOGAN_TABLEINSERT_H

#include <string>

class Table;
class sqlite3_stmt;

class TableInsert{
    public:   
        TableInsert(Table* table);

        int BindString(const std::string& field, const std::string& val);
        int BindInt(const std::string& field, const int64_t& val);
        int BindDouble(const std::string& field, const double& val);

        sqlite3_stmt* get_statement();
    private:
        sqlite3_stmt* stmt_ = 0;
        Table* table_ = 0;        
};
#endif //LOGAN_TABLEINSERT_H
