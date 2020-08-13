
#include "basic.hpp"
#include <list>
#include <future>
#include "ports/pubsub/publisherport.hpp"
#include "ports/pubsub/subscriberport.hpp"
#include "../../core/testfunctions.h"

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

    int future_count = 10;
    int future_send = 100;
    int send_count = future_count * future_send;
    sleep_ms(send_count / 10);

    std::list<std::future<void>> futures;

    for(int i = 0; i < future_count; i++){
        auto future = std::async(std::launch::async, [i, &pub_port](int send_count){
            //Send as fast as possible
            for(int j = 0; j < send_count; j++){
                Base::Basic b;
                b.int_val = (send_count * i) + j;
                b.str_val = std::to_string(b.int_val);
                pub_port.Send(b);
            }
        }, future_send);
        futures.push_back(std::move(future));
    }

    futures.clear();
    
    sleep_ms(send_count / 10);

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
    EXPECT_GE(total_txd, send_count);
    EXPECT_GE(total_sent, send_count);
    EXPECT_GE(total_rxd, send_count);
    EXPECT_GE(total_received, send_count);
    EXPECT_GE(rx_callback_count, send_count);
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

    EXPECT_GE(total_txd, send_count);
    EXPECT_GE(total_sent, send_count);
    EXPECT_GE(total_rxd, send_count);
    EXPECT_GE(total_received, 1);
    EXPECT_GE(rx_callback_count, 1);
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

    EXPECT_GE(total_txd, send_count * 2);
    EXPECT_GE(total_sent, send_count);
};