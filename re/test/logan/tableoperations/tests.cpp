#include "gtest/gtest.h"

#include "sqlitedatabase.h"
#include "table.h"
#include "tableinsert.h"
#include "sqlite3.h"

#include <limits>

TEST(Logan, TableCreation){
    SQLiteDatabase db(":memory");

    Table table1(db, "table1");
    
    EXPECT_TRUE(table1.AddColumn("col1", "INTEGER"));
    EXPECT_TRUE(table1.AddColumn("col2", "INTEGER"));
    EXPECT_TRUE(table1.AddColumn("col3", "VARCHAR"));
    EXPECT_TRUE(table1.AddColumn("col4", "DECIMAL"));
    //Try a duplicate insertion
    EXPECT_FALSE(table1.AddColumn("col2", "INTEGER"));
    table1.Finalize();
    db.ExecuteSqlStatement(table1.get_table_construct_statement(), true);
}

TEST(Logan, TableInsertion_BigInts){
    SQLiteDatabase db(":memory:");
    Table table(db, "test_table");
    
    //Construct 4 tables
    EXPECT_TRUE(table.AddColumn("int32", "INTEGER"));
    EXPECT_TRUE(table.AddColumn("uint32", "INTEGER"));
    EXPECT_TRUE(table.AddColumn("int64", "INTEGER"));
    EXPECT_TRUE(table.AddColumn("uint64", "INTEGER"));
    table.Finalize();

    db.ExecuteSqlStatement(table.get_table_construct_statement(), true);

    {
        auto insert_statement = table.get_insert_statement();
        //Insert the max ints for each of there types
        EXPECT_EQ(insert_statement.BindInt(":int32", std::numeric_limits<int32_t>::max()), SQLITE_OK);
        EXPECT_EQ(insert_statement.BindInt(":uint32", std::numeric_limits<uint32_t>::max()), SQLITE_OK);
        EXPECT_EQ(insert_statement.BindInt(":int64", std::numeric_limits<int64_t>::max()), SQLITE_OK);
        EXPECT_EQ(insert_statement.BindInt(":uint64", std::numeric_limits<uint64_t>::max()), SQLITE_OK);
        db.ExecuteSqlStatement(insert_statement.get_statement(), true);
    }

    auto sql_db = db.GetDatabase();
    //Run a tester
    std::string select{"SELECT * from test_table"};

    //Run a select statement, which calls into the callback function to test the args
    auto rc = sqlite3_exec(sql_db, select.c_str(), [](void*, int argc, char** argv, char** col_name)->int{
        EXPECT_EQ(argc, 5);
        auto int32 = std::stoi(argv[1]);
        auto uint32 = std::stoul(argv[2]);
        auto int64 = std::stoll(argv[3]);
        auto uint64 = std::stoull(argv[4]);
        EXPECT_EQ(int32, std::numeric_limits<int32_t>::max());
        EXPECT_EQ(uint32, std::numeric_limits<uint32_t>::max());
        EXPECT_EQ(int64, std::numeric_limits<int64_t>::max());
        EXPECT_EQ(uint64, std::numeric_limits<uint64_t>::max());
        return 0;
    }, nullptr, nullptr);

    EXPECT_EQ(rc, SQLITE_OK);
}