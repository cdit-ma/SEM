//
// Created by cdit-ma on 8/01/19.
//

#include "gtest/gtest.h"
#include "google/protobuf/util/message_differencer.h"
#include "protobufmodelparser.h"
#include <google/protobuf/util/json_util.h>
#include <fstream>
#include <cmrc/cmrc.hpp>
CMRC_DECLARE(test_models);

TEST(ReCommon_GraphmlParser, FileNotFound) {

    try {
        auto out = ProtobufModelParser::ParseModel("models/non-existent.graphml", "FileNotFound");
        FAIL() << "Expected runtime_error exception on non-existent file.";
    } catch(const std::runtime_error& ex) {

    } catch(...) {
        FAIL() << "Expected runtime_error exception specifically on non-existent file.";
    }
}

TEST(ReCommon_GraphmlParser, Deterministic) {
    using namespace google::protobuf::util;

    // Get model from resource fs (copied by cmrs).
    auto model_file = cmrc::test_models::get_filesystem().open("models/model.graphml");
    std::string model_string{model_file.begin(), model_file.end()};

    auto out = ProtobufModelParser::ParseModelString(model_string, "simple_test");
    auto out2 = ProtobufModelParser::ParseModelString(model_string, "simple_test");

    // The same model parsed twice should result in exactly the same output.
    std::string diffs;
    MessageDifferencer differ;
    differ.ReportDifferencesToString(&diffs);

    auto same = differ.Compare(*out, *out2);

    EXPECT_TRUE(same) << diffs;
}

TEST(ReCommon_GraphmlParser, ExperimentID) {
    using namespace google::protobuf::util;

    // Get model from resource fs (copied by cmrs).
    auto model_file = cmrc::test_models::get_filesystem().open("models/model.graphml");
    std::string model_string{model_file.begin(), model_file.end()};

    auto out = ProtobufModelParser::ParseModelString(model_string, "simple_test");
    auto out_same = ProtobufModelParser::ParseModelString(model_string, "simple_test");
    auto out_different = ProtobufModelParser::ParseModelString(model_string, "not_simple_test");

    // Should be different with a differing experiment id.
    EXPECT_FALSE(MessageDifferencer::Equals(*out, *out_different));
    EXPECT_TRUE(MessageDifferencer::Equals(*out, *out_same));
}

// Simple replication test. Model contains component assembly (replicated twice) containing sender + receiver components.
// Should replicate out to 2x sender + 2x receiver. Each sender should be connected to its paired receiver and nothing else.
TEST(ReCommon_GraphmlParser, SimpleReplication) {
    using namespace google::protobuf::util;

    // Get model from resource fs (copied by cmrs).
    auto model_file = cmrc::test_models::get_filesystem().open("models/simple_replication.graphml");
    std::string model_string{model_file.begin(), model_file.end()};
    auto out = ProtobufModelParser::ParseModelString(model_string, "replication");

    auto expected_file = cmrc::test_models::get_filesystem().open("models/simple_replication.json");
    std::string expected_string{expected_file.begin(), expected_file.end()};
    NodeManager::Experiment expected;
    JsonStringToMessage(expected_string, &expected);

    std::string diffs;
    MessageDifferencer differ;
    differ.ReportDifferencesToString(&diffs);

    auto same = differ.Compare(expected, *out);

    EXPECT_TRUE(same) << diffs;
}

// Complex replication test. Model contains two component assemblies both replicated twice. One contains sender, the other contains receiver.
// Should replicate out to 2x sender + 2x receiver. Each sender should be connected to both receivers.
TEST(ReCommon_GraphmlParser, ComplexReplication) {
    using namespace google::protobuf::util;

    // Get model from resource fs (copied by cmrs).
    auto model_file = cmrc::test_models::get_filesystem().open("models/complex_replication.graphml");
    std::string model_string{model_file.begin(), model_file.end()};
    auto out = ProtobufModelParser::ParseModelString(model_string, "replication");

    auto expected_file = cmrc::test_models::get_filesystem().open("models/complex_replication.json");
    std::string expected_string{expected_file.begin(), expected_file.end()};
    NodeManager::Experiment expected;
    JsonStringToMessage(expected_string, &expected);

    std::string diffs;
    MessageDifferencer differ;
    differ.ReportDifferencesToString(&diffs);

    auto same = differ.Compare(expected, *out);

    EXPECT_TRUE(same) << diffs;
}

// See RE-414 on cdit-ma jira
TEST(ReCommon_GraphmlParser, RE_414) {
    using namespace google::protobuf::util;

    // Get model from resource fs (copied by cmrs).
    auto model_file = cmrc::test_models::get_filesystem().open("models/RE414.graphml");
    std::string model_string{model_file.begin(), model_file.end()};
    auto out = ProtobufModelParser::ParseModelString(model_string, "replication");

    auto expected_file = cmrc::test_models::get_filesystem().open("models/RE414.json");
    std::string expected_string{expected_file.begin(), expected_file.end()};
    NodeManager::Experiment expected;
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

TEST(ReCommon_GraphmlParser, RE414_uneven_depth) {
    using namespace google::protobuf::util;

    // Get model from resource fs (copied by cmrs).
    auto model_file = cmrc::test_models::get_filesystem().open("models/RE414_uneven_depth.graphml");
    std::string model_string{model_file.begin(), model_file.end()};
    auto out = ProtobufModelParser::ParseModelString(model_string, "replication");

    auto expected_file = cmrc::test_models::get_filesystem().open("models/RE414_uneven_depth.json");
    std::string expected_string{expected_file.begin(), expected_file.end()};
    NodeManager::Experiment expected;
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