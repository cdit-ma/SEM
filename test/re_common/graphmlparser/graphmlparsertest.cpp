//
// Created by cdit-ma on 8/01/19.
//

#include "gtest/gtest.h"
#include "google/protobuf/util/message_differencer.h"
#include <util/graphmlparser/protobufmodelparser.h>

TEST(GraphmlParser, FileNotFound) {

    try {
        auto out = ProtobufModelParser::ParseModel("models/non-existent.graphml", "FileNotFound");
        FAIL() << "Expected runtime_error exception on non-existent file.";
    } catch(const std::runtime_error& ex) {

    } catch(...) {
        FAIL() << "Expected runtime_error exception specifically on non-existent file.";
    }
}

TEST(GraphmlParser, Deterministic) {
    using namespace google::protobuf::util;

    //Current working dir is re/bin/test, get model from re/bin/test/models (copied by cmake).
    auto out = ProtobufModelParser::ParseModel("models/model.graphml", "simple_test");
    auto out2 = ProtobufModelParser::ParseModel("models/model.graphml", "simple_test");

    // The same model parsed twice should result in exactly the same output.
    EXPECT_TRUE(MessageDifferencer::Equals(*out, *out2));
}

TEST(GraphmlParser, ExperimentID) {
    using namespace google::protobuf::util;

    auto out = ProtobufModelParser::ParseModel("models/model.graphml", "simple_test");
    auto out2 = ProtobufModelParser::ParseModel("models/model.graphml", "not_simple_test");

    // Should be different with a differing experiment id.
    EXPECT_FALSE(MessageDifferencer::Equals(*out, *out2));
}

TEST(GraphmlParser, SimpleReplication) {
    using namespace google::protobuf::util;
    auto out = ProtobufModelParser::ParseModel("models/simple_replication.graphml", "replication");

    NodeManager::Experiment expected;

    out->PrintDebugString();


    EXPECT_TRUE(MessageDifferencer::Equals(*out, expected));
}

TEST(GraphmlParser, ComplexReplication) {
    using namespace google::protobuf::util;
    auto out = ProtobufModelParser::ParseModel("models/complex_replication.graphml", "replication");

    NodeManager::Experiment expected;

    EXPECT_TRUE(MessageDifferencer::Equals(*out, expected));
}

TEST(GraphmlParser, ReplicationConnections) {
    using namespace google::protobuf::util;
    auto out = ProtobufModelParser::ParseModel("models/simple_replication_connections.graphml", "replication");

    NodeManager::Experiment expected;

    EXPECT_TRUE(MessageDifferencer::Equals(*out, expected));
}

TEST(GraphmlParser, ComplexReplicationConnections) {
    using namespace google::protobuf::util;
    auto out = ProtobufModelParser::ParseModel("models/complex_replication_connections.graphml", "replication");

    NodeManager::Experiment expected;

    EXPECT_TRUE(MessageDifferencer::Equals(*out, expected));
}