#include "gtest/gtest.h"
#include <databaseclient.h>
#include <gmock/gmock.h>
#include <string>

#include "experimenttracker.h"

using testing::_;
using testing::Return;

class MockDatabaseClient : public DatabaseClient {
public:
    MockDatabaseClient() : DatabaseClient(""){};
    MOCK_METHOD0(Test, (), (override));
    MOCK_METHOD1(void, InitFromSchema, (std::string & connection_details));
};

// TODO: Create a test fixture that spawns a postgres database instance for test isolation
class ExperimentTrackerFixture : public ::testing::Test {
public:
    ExperimentTrackerFixture() :
        db_client("dbname = postgres user = postgres password = ***REMOVED*** hostaddr = "
                  "192.168.111.231 port = 5432")
    {
    }
    MockDatabaseClient db_client;
};

TEST_F(ExperimentTrackerFixture, ExperimentTracker_Create_CommonArgs)
{
    EXPECT_NO_THROW(ExperimentTracker et(std::make_shared<DatabaseClient>(db_client)));
}

// TODO: Add tests for the rest of the functions once a newly spawned DB instance can be created