
#include "basic.hpp"
#include <core/ports/pubsub/publisherport.hpp>
#include <core/ports/pubsub/subscriberport.hpp>

namespace PubSub{
    namespace Basic{
        namespace Stable{
            void RunTest(::PublisherPort<Base::Basic>& pub_port, ::SubscriberPort<Base::Basic>& sub_port, int& rx_callback_count);
        };
        namespace Busy{
            void RunTest(::PublisherPort<Base::Basic>& pub_port, ::SubscriberPort<Base::Basic>& sub_port, int& rx_callback_count);
        };
        namespace Terminate{
            void RunTest(::PublisherPort<Base::Basic>& pub_port, ::SubscriberPort<Base::Basic>& sub_port, int& rx_callback_count);
        };
    };
};

void PubSub::Basic::Stable::RunTest(::PublisherPort<Base::Basic>& pub_port, ::SubscriberPort<Base::Basic>& sub_port, int& rx_callback_count){
    //Configure
    EXPECT_TRUE(sub_port.Configure());
    EXPECT_TRUE(pub_port.Configure());
    
    //Activate
    EXPECT_TRUE(sub_port.Activate());
    EXPECT_TRUE(pub_port.Activate());

    int send_count = 100;

    //Send as fast as possible
    for(int i = 0; i < send_count; i++){
       Base::Basic b;
        b.int_val = i;
        b.str_val = std::to_string(i);
        pub_port.Send(b);
        sleep_ms(1);
    }

    //Passivate
    EXPECT_TRUE(sub_port.Passivate());
    EXPECT_TRUE(pub_port.Passivate());
    
    //Terminate
    EXPECT_TRUE(sub_port.Terminate());
    EXPECT_TRUE(pub_port.Terminate());

    auto total_rxd = sub_port.GetEventsReceived();
    auto total_received = sub_port.GetEventsProcessed();

    auto total_txd = pub_port.GetEventsReceived();
    auto total_sent = pub_port.GetEventsProcessed();

    //Test Results
    EXPECT_EQ(total_txd, send_count);
    EXPECT_EQ(total_sent, send_count);
    EXPECT_EQ(total_rxd, send_count);
    EXPECT_EQ(total_received, send_count);
    EXPECT_EQ(rx_callback_count, send_count);
};

void PubSub::Basic::Busy::RunTest(::PublisherPort<Base::Basic>& pub_port, ::SubscriberPort<Base::Basic>& sub_port, int& rx_callback_count){
    //Configure
    EXPECT_TRUE(sub_port.Configure());
    EXPECT_TRUE(pub_port.Configure());
    
    //Activate
    EXPECT_TRUE(sub_port.Activate());
    EXPECT_TRUE(pub_port.Activate());

    int send_count = 100;

    //Send as fast as possible
    for(int i = 0; i < send_count; i++){
       Base::Basic b;
        b.int_val = i;
        b.str_val = std::to_string(i);
        pub_port.Send(b);
    }

    //Sleep for a reasonable time (Bigger than the callback work)
    sleep_ms(500);

    //Passivate
    EXPECT_TRUE(sub_port.Passivate());
    EXPECT_TRUE(pub_port.Passivate());
    
    //Terminate
    EXPECT_TRUE(sub_port.Terminate());
    EXPECT_TRUE(pub_port.Terminate());

    auto total_rxd = sub_port.GetEventsReceived();
    auto total_received = sub_port.GetEventsProcessed();

    auto total_txd = pub_port.GetEventsReceived();
    auto total_sent = pub_port.GetEventsProcessed();

    EXPECT_EQ(total_txd, send_count);
    EXPECT_EQ(total_sent, send_count);
    EXPECT_EQ(total_rxd, send_count);
    EXPECT_EQ(total_received, 1);
    EXPECT_EQ(rx_callback_count, 1);
};

void PubSub::Basic::Terminate::RunTest(::PublisherPort<Base::Basic>& pub_port, ::SubscriberPort<Base::Basic>& sub_port, int& rx_callback_count){
    //Configure
    EXPECT_TRUE(sub_port.Configure());
    EXPECT_TRUE(pub_port.Configure());
    
    //Activate
    EXPECT_TRUE(sub_port.Activate());
    EXPECT_TRUE(pub_port.Activate());

    int send_count = 10;

    //Send as fast as possible
    for(int i = 0; i < send_count; i++){
        Base::Basic b;
        b.int_val = i;
        b.str_val = std::to_string(i);
        pub_port.Send(b);
    }
    
    //Terminate the Publisher
    EXPECT_TRUE(pub_port.Passivate());
    EXPECT_TRUE(pub_port.Terminate());

    //Sleep for a reasonable time (Bigger than the callback work)
    sleep_ms(2000);

    //Send as fast as possible
    for(int i = 0; i < send_count; i++){
        Base::Basic b;
        b.int_val = i;
        b.str_val = std::to_string(i);
        pub_port.Send(b);
    }
    
    auto total_txd = pub_port.GetEventsReceived();
    auto total_sent = pub_port.GetEventsProcessed();

    EXPECT_EQ(total_txd, send_count * 2);
    EXPECT_EQ(total_sent, send_count);
};