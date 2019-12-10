#include "gtest/gtest.h"

#include "databaseclient.h"

// TODO: Create a test fixture that spawns a postgres database instance for test isolation

TEST(Re_Logan_Aggregation, DatabaseClient_Create_CommonArgs)
{
    EXPECT_NO_THROW(DatabaseClient dbc("dbname = postgres user = postgres password = ***REMOVED*** "
                                       "hostaddr = 192.168.111.231 port = 5432"));
}

TEST(Re_Logan_Aggregation, DatabaseClient_Create_EmptyArgs)
{
    EXPECT_THROW(DatabaseClient dbc(""), pqxx::broken_connection);
}

TEST(Re_Logan_Aggregation, DatabaseClient_Create_WrongArgs)
{
    EXPECT_THROW(DatabaseClient dbc("dbname = wrong user = wrong password = wrong hostaddr = "
                                    "0.0.0.0 port = 1"),
                 pqxx::broken_connection);
}

// TODO: Add tests for the rest of the functions once a newly spawned DB instance can be created