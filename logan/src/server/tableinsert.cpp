#include "tableinsert.h"

#include <math.h>

#include "sqlite3.h"
#include "table.h"
#include "sqlitedatabase.h"

TableInsert::TableInsert(Table& table):
table_(table)
{
    stmt_ = table_.get_table_insert_statement();
}

TableInsert::~TableInsert(){
    table_.free_table_insert_statement(stmt_);
    stmt_ = 0;
}

int TableInsert::GetFieldIndex(const std::string& field){
    return sqlite3_bind_parameter_index(stmt_, field.c_str());
}

int TableInsert::BindString(const std::string& field, const std::string& val){
    const auto& id = GetFieldIndex(field);
        
    if(val.size()){
        //SQLITE_TRANSIENT = Copy straight away
        return sqlite3_bind_text(stmt_, id, val.c_str(), val.size(), SQLITE_TRANSIENT);
    }else{
        return sqlite3_bind_null(stmt_, id);
    }
}

int TableInsert::BindInt(const std::string& field, const int64_t& val){
    const auto& id = GetFieldIndex(field); 
    return sqlite3_bind_int64(stmt_, id, val);
}

int TableInsert::BindDouble(const std::string& field, const double& val){
    double dbl_var(val);
    if(isnan(dbl_var)){
        //avoid NULL in database
        dbl_var = 0.0;
    }
    const auto& id = GetFieldIndex(field);
    return sqlite3_bind_double(stmt_, id, dbl_var);
}

sqlite3_stmt& TableInsert::get_statement(){
    return *stmt_;
}
