#include "gtest/gtest.h"
#include <databaseclient.h>
#include <gmock/gmock.h>
#include <string>

#include "experimenttracker.h"

using testing::_;
using testing::Return;

class MockDatabaseClient : public DatabaseClient {
public:
    explicit MockDatabaseClient(const std::string& db_params = "dbname = postgres user = postgres "
                                                               "password = ***REMOVED*** hostaddr = "
                                                               "192.168.111.231 port = 5432") :
        DatabaseClient(db_params){};
    // MOCK_METHOD(void, Test, ());
    MOCK_METHOD(void, InitSchemaFrom, (const std::string& sql_string), (override));
};

using ::testing::NaggyMock;
// NaggyMock<MockDatabaseClient> naggy_mock;

// TODO: Create a test fixture that spawns a postgres database instance for test isolation
class ExperimentTrackerFixture : public ::testing::Test {
public:
    ExperimentTrackerFixture() : db_client(std::make_shared<NaggyMock<MockDatabaseClient>>()){};
    std::shared_ptr<NaggyMock<MockDatabaseClient>> db_client;
};

TEST_F(ExperimentTrackerFixture, ExperimentTracker_Create_CommonArgs)
{
    EXPECT_NO_THROW(ExperimentTracker et(db_client));
}

// TODO: Add tests for the rest of the functions once a newly spawned DB instance can be created