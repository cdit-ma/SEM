//
// Created by Jackson on 4/02/2020.
//

#include "gtest/gtest.h"

#include <aggregationreplier.h>

using namespace re::logging::aggregation::broker;

// TODO: Mock one or more DatabaseClients for proper dependency injection

TEST(Re_Logan_Aggregation, AggregationReplier_Construct)
{
    std::shared_ptr<DatabaseClient> db_client;
    std::string db_string("dbname = postgres user = postgres password = ***REMOVED*** "
    "hostaddr = 192.168.111.249 port = 5432");

    // Create database client -- replace with mock version!
    try {
        db_client = std::make_shared<DatabaseClient>(db_string);
    } catch (std::exception& e) {
        std::cout << "Failed to create DatabaseClient: " << e.what() << std::endl;
        GTEST_FAIL();
    }

    EXPECT_NO_THROW(
        AggregationReplier replier(db_client);
        );
}