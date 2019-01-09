//
// Created by cdit-ma on 8/01/19.
//

#include "gtest/gtest.h"
#include "google/protobuf/util/message_differencer.h"
#include <util/graphmlparser/protobufmodelparser.h>
#include <google/protobuf/util/json_util.h>
#include <fstream>

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
    std::string diffs;
    MessageDifferencer differ;
    differ.ReportDifferencesToString(&diffs);

    auto same = differ.Compare(*out, *out2);

    EXPECT_TRUE(same) << diffs;
}

TEST(GraphmlParser, ExperimentID) {
    using namespace google::protobuf::util;

    auto out = ProtobufModelParser::ParseModel("models/model.graphml", "simple_test");
    auto out_same = ProtobufModelParser::ParseModel("models/model.graphml", "simple_test");
    auto out_different = ProtobufModelParser::ParseModel("models/model.graphml", "not_simple_test");

    // Should be different with a differing experiment id.
    EXPECT_FALSE(MessageDifferencer::Equals(*out, *out_different));
    EXPECT_TRUE(MessageDifferencer::Equals(*out, *out_same));
}

// Simple replication test. Model contains component assembly (replicated twice) containing sender + receiver components.
// Should replicate out to 2x sender + 2x receiver. Each sender should be connected to its paired receiver and nothing else.
TEST(GraphmlParser, SimpleReplication) {
    using namespace google::protobuf::util;
    auto out = ProtobufModelParser::ParseModel("models/simple_replication.graphml", "replication");

    NodeManager::Experiment expected;
    std::ifstream infile {"models/simple_replication.json"};
    std::string expected_string {std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>()};
    JsonStringToMessage(expected_string, &expected);

    std::string diffs;
    MessageDifferencer differ;
    differ.ReportDifferencesToString(&diffs);

    auto same = differ.Compare(expected, *out);

    EXPECT_TRUE(same) << diffs;
}

// Complex replication test. Model contains two component assemblies both replicated twice. One contains sender, the other contains receiver.
// Should replicate out to 2x sender + 2x receiver. Each sender should be connected to both receivers.
TEST(GraphmlParser, ComplexReplication) {
    using namespace google::protobuf::util;
    auto out = ProtobufModelParser::ParseModel("models/complex_replication.graphml", "replication");

    NodeManager::Experiment expected;
    std::ifstream infile {"models/complex_replication.json"};
    std::string expected_string {std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>()};
    JsonStringToMessage(expected_string, &expected);

    std::string diffs;
    MessageDifferencer differ;
    differ.ReportDifferencesToString(&diffs);

    auto same = differ.Compare(expected, *out);

    EXPECT_TRUE(same) << diffs;
}

// See RE-414 on cdit-ma jira
TEST(GraphmlParser, RE414) {
    using namespace google::protobuf::util;
    auto out = ProtobufModelParser::ParseModel("models/RE414.graphml", "replication");

    NodeManager::Experiment expected;
    std::ifstream infile {"models/RE414.json"};
    std::string expected_string {std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>()};
    JsonStringToMessage(expected_string, &expected);


    // Set up protobuf message differ
    std::string diffs;
    MessageDifferencer differ;
    // Treat connected ports as set s.t. ordering doesn't ruin an otherwise successful match
    auto connected_ports_field_descriptor = expected.GetDescriptor()
            ->FindFieldByLowercaseName("clusters")->message_type()
            ->FindFieldByLowercaseName("nodes")->message_type()
            ->FindFieldByLowercaseName("containers")->message_type()
            ->FindFieldByLowercaseName("components")->message_type()
            ->FindFieldByLowercaseName("ports")->message_type()
            ->FindFieldByLowercaseName("connected_ports");

    differ.TreatAsSet(connected_ports_field_descriptor);
    differ.ReportDifferencesToString(&diffs);

    auto same = differ.Compare(expected, *out);

    EXPECT_TRUE(same) << diffs;
}