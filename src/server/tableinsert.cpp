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
 
#include "tableinsert.h"

#include <math.h>

#include "sqlite3.h"
#include "table.h"
#include "sqlitedatabase.h"

TableInsert::TableInsert(Table* table){
    table_ = table;
    stmt_ = table_->get_table_insert_statement();
}

int TableInsert::BindString(const std::string& field, const std::string& val){
    auto id = table_->get_field_id(field);
    
    if(val.size()){
        //SQLITE_TRANSIENT = Copy straight away
        return sqlite3_bind_text(stmt_, id, val.c_str(), val.size(), SQLITE_TRANSIENT);
    }else{
        return sqlite3_bind_null(stmt_, id);
    }
}

int TableInsert::BindInt(const std::string& field, const int64_t& val){
    auto id = table_->get_field_id(field);
    return sqlite3_bind_int(stmt_, id, val);
}

int TableInsert::BindDouble(const std::string& field, const double& val){
    double dbl_var(val);
    if(isnan(dbl_var)){
        //avoid NULL in database
        dbl_var = 0.0;
    }
    auto id = table_->get_field_id(field);
    return sqlite3_bind_double(stmt_, id, dbl_var);
}

sqlite3_stmt* TableInsert::get_statement(){
    return stmt_;
}
