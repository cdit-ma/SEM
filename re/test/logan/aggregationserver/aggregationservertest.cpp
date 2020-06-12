#include "gtest/gtest.h"

#include "aggregationserver.h"

// TODO: Create a test fixture that spawns a postgres database instance for test isolation

// TODO: re-enable once a more ergonomic way of killing the server is provided
TEST(Re_Logan_Aggregation, DISABLED_AggregationServer_Create_CommonArgs)
{
    EXPECT_NO_THROW(
        AggregationServer as("192.168.111.231", "agg_test_pass", "tcp://192.168.111.231:20000"));

    // Need to send interrupt to the AggregationServer here
}

// TODO: Add tests for the rest of the functions once a newly spawned DB instance can be created