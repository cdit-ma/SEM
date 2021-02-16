#include "protoreceiver.h"
#include "protowriter.h"
#include <google/protobuf/util/time_util.h>
#include "gtest/gtest.h"


void Sleep(){
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

TEST(ReCommon_ZmqProto, DynamicConnect) {
    int rc_count = 0;
    std::string address = "tcp://127.0.0.1:7001";
    std::string address2 = "tcp://127.0.0.1:7002";
    
    zmq::ProtoReceiver receiver;
    
    zmq::ProtoWriter writer;
    zmq::ProtoWriter writer2;
    
    receiver.RegisterProtoCallback<google::protobuf::Timestamp>([&rc_count](const google::protobuf::Timestamp& t){
        rc_count++;
    });

    writer.BindPublisherSocket(address);
    writer2.BindPublisherSocket(address2);
    receiver.Filter("");
    receiver.Connect(address2);

    for(int i = 0; i < 100; i++){
        writer.PushMessage(std::unique_ptr<google::protobuf::Timestamp>(new google::protobuf::Timestamp()));
        writer2.PushMessage(std::unique_ptr<google::protobuf::Timestamp>(new google::protobuf::Timestamp()));
    }

    Sleep();

    EXPECT_EQ(rc_count, 100);

    receiver.Connect(address);    
 
    for(int i = 0; i < 100; i++){
        writer.PushMessage(std::unique_ptr<google::protobuf::Timestamp>(new google::protobuf::Timestamp()));
        writer2.PushMessage(std::unique_ptr<google::protobuf::Timestamp>(new google::protobuf::Timestamp()));
    }

    Sleep();

    EXPECT_EQ(rc_count, 300);
    receiver.Disconnect(address);

    for(int i = 0; i < 100; i++){
        writer.PushMessage(std::unique_ptr<google::protobuf::Timestamp>(new google::protobuf::Timestamp()));
        writer2.PushMessage(std::unique_ptr<google::protobuf::Timestamp>(new google::protobuf::Timestamp()));
    }
    
    Sleep();
    EXPECT_EQ(rc_count, 400);
    EXPECT_EQ(receiver.GetRxCount(), rc_count);
}

TEST(ReCommon_ZmqProto, DynamicFilters) {
    int rc_count = 0;
    std::string address = "tcp://127.0.0.1:7001";
    
    zmq::ProtoReceiver receiver;
    
    zmq::ProtoWriter writer;
    
    receiver.RegisterProtoCallback<google::protobuf::Timestamp>([&rc_count](const google::protobuf::Timestamp& t){
        rc_count++;
    });

    writer.BindPublisherSocket(address);
    receiver.Connect(address);


    for(int i = 0; i < 100; i++){
        writer.PushMessage("A", std::unique_ptr<google::protobuf::Timestamp>(new google::protobuf::Timestamp()));
    }

    Sleep();
    EXPECT_EQ(rc_count, 0);

    receiver.Filter("A");

    for(int i = 0; i < 100; i++){
        writer.PushMessage("A", std::unique_ptr<google::protobuf::Timestamp>(new google::protobuf::Timestamp()));
        writer.PushMessage("B", std::unique_ptr<google::protobuf::Timestamp>(new google::protobuf::Timestamp()));
    }

    Sleep();
    EXPECT_EQ(rc_count, 100);
    
    receiver.Unfilter("A");
    for(int i = 0; i < 100; i++){
        writer.PushMessage("A", std::unique_ptr<google::protobuf::Timestamp>(new google::protobuf::Timestamp()));
        writer.PushMessage("B", std::unique_ptr<google::protobuf::Timestamp>(new google::protobuf::Timestamp()));
    }
    Sleep();
    EXPECT_EQ(rc_count, 100);
    EXPECT_EQ(receiver.GetRxCount(), rc_count);
}

TEST(ReCommon_ZmqProto, StaticConnect) {
    int rc_count = 0;
    std::string address = "tcp://127.0.0.1:7001";
    
    zmq::ProtoReceiver receiver;
    zmq::ProtoWriter writer;
    
    receiver.RegisterProtoCallback<google::protobuf::Timestamp>([&rc_count](const google::protobuf::Timestamp& t){
        rc_count++;
    });

    writer.BindPublisherSocket(address);
    receiver.Connect(address);
    receiver.Filter("");

    for(int i = 0; i < 100; i++){
        writer.PushMessage(std::unique_ptr<google::protobuf::Timestamp>(new google::protobuf::Timestamp()));
    }

    Sleep();
    EXPECT_EQ(rc_count, 100);
    EXPECT_EQ(receiver.GetRxCount(), rc_count);
}