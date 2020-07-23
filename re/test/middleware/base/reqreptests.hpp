
#include "basic.hpp"
#include "ports/requestreply/requesterport.hpp"
#include "ports/requestreply/replierport.hpp"
#include "../../core/testfunctions.h"

namespace ReqRep{
    namespace Basic2Basic{
        namespace Stable{
            Base::Basic Callback(Base::Basic& message);
            void RunTest(::RequesterPort<Base::Basic, Base::Basic>& req_port, ::ReplierPort<Base::Basic, Base::Basic>& rep_port);
        };
        namespace Busy{
            Base::Basic Callback(Base::Basic& message);
            void RunTest(::RequesterPort<Base::Basic, Base::Basic>& req_port, ::ReplierPort<Base::Basic, Base::Basic>& rep_port);
        };
        namespace Timeout{
            Base::Basic Callback(Base::Basic& message);
            void RunTest(::RequesterPort<Base::Basic, Base::Basic>& req_port, ::ReplierPort<Base::Basic, Base::Basic>& rep_port);
        };
    };

    namespace Basic2Void{
        namespace Stable{
            void Callback(Base::Basic& message);
            void RunTest(::RequesterPort<void, Base::Basic>& req_port, ::ReplierPort<void, Base::Basic>& rep_port);
        };
        namespace Busy{
            void Callback(Base::Basic& message);
            void RunTest(::RequesterPort<void, Base::Basic>& req_port, ::ReplierPort<void, Base::Basic>& rep_port);
        };
        namespace Timeout{
            void Callback(Base::Basic& message);
            void RunTest(::RequesterPort<void, Base::Basic>& req_port, ::ReplierPort<void, Base::Basic>& rep_port);
        };
    };

    namespace Void2Basic{
        namespace Stable{
            Base::Basic Callback();
            void RunTest(::RequesterPort<Base::Basic, void>& req_port, ::ReplierPort<Base::Basic, void>& rep_port);
        };
        namespace Busy{
            Base::Basic Callback();
            void RunTest(::RequesterPort<Base::Basic, void>& req_port, ::ReplierPort<Base::Basic, void>& rep_port);
        };
        namespace Timeout{
            Base::Basic Callback();
            void RunTest(::RequesterPort<Base::Basic, void>& req_port, ::ReplierPort<Base::Basic, void>& rep_port);
        };
    };
};

/*
    Basic to Basic Tests
*/

Base::Basic ReqRep::Basic2Basic::Stable::Callback(Base::Basic& message){
    message.int_val *= 10;
    return message;
}

void ReqRep::Basic2Basic::Stable::RunTest(
        ::RequesterPort<Base::Basic, Base::Basic>& req_port,
        ::ReplierPort<Base::Basic, Base::Basic>& rep_port){

    //Configure
    EXPECT_TRUE(rep_port.Configure());
    EXPECT_TRUE(req_port.Configure());

    //Activate
    EXPECT_TRUE(rep_port.Activate());
    EXPECT_TRUE(req_port.Activate());

    int send_count = 400;

    //Send as fast as possible
    for(int i = 0; i < send_count; i++){
        Base::Basic b;
        b.int_val = i;
        b.str_val = std::to_string(i);
        auto c = req_port.SendRequest(b, std::chrono::milliseconds(200));
        EXPECT_TRUE(c.first);
        if(c.first){
            EXPECT_EQ(b.int_val * 10, c.second.int_val);
        }
    }
    
    //Passivate
    EXPECT_TRUE(rep_port.Passivate());
    EXPECT_TRUE(req_port.Passivate());
    
    //Terminate
    EXPECT_TRUE(rep_port.Terminate());
    EXPECT_TRUE(req_port.Terminate());

    auto total_rxd = req_port.GetEventsReceived();
    auto proc_rxd = rep_port.GetEventsProcessed();

    auto total_txd = req_port.GetEventsReceived();
    auto total_sent = req_port.GetEventsProcessed();

    EXPECT_EQ(total_txd, send_count);
    EXPECT_EQ(total_sent, send_count);
    EXPECT_EQ(total_rxd, send_count);
    EXPECT_EQ(proc_rxd, send_count);
};

Base::Basic ReqRep::Basic2Basic::Busy::Callback(Base::Basic& message){
    sleep_ms(50);
    message.int_val *= 10;
    return message;
}

void ReqRep::Basic2Basic::Busy::RunTest(
        ::RequesterPort<Base::Basic, Base::Basic>& req_port,
        ::ReplierPort<Base::Basic, Base::Basic>& rep_port){

    //Configure
    EXPECT_TRUE(rep_port.Configure());
    EXPECT_TRUE(req_port.Configure());

    //Activate
    EXPECT_TRUE(rep_port.Activate());
    EXPECT_TRUE(req_port.Activate());

    int send_count = 100;

    //Send as fast as possible
    for(int i = 0; i < send_count; i++){
        Base::Basic b;
        b.int_val = i;
        b.str_val = std::to_string(i);
        auto c = req_port.SendRequest(b, std::chrono::milliseconds(250));
        EXPECT_TRUE(c.first);
        if(c.first){
            EXPECT_EQ(b.int_val * 10, c.second.int_val);
        }
    }

    //Passivate
    EXPECT_TRUE(rep_port.Passivate());
    EXPECT_TRUE(req_port.Passivate());
    
    //Terminate
    EXPECT_TRUE(rep_port.Terminate());
    EXPECT_TRUE(req_port.Terminate());

    auto total_rxd = req_port.GetEventsReceived();
    auto proc_rxd = rep_port.GetEventsProcessed();

    auto total_txd = req_port.GetEventsReceived();
    auto total_sent = req_port.GetEventsProcessed();

    EXPECT_EQ(total_txd, send_count);
    EXPECT_EQ(total_sent, send_count);
    EXPECT_EQ(total_rxd, send_count);
    EXPECT_EQ(proc_rxd, send_count);
};

Base::Basic ReqRep::Basic2Basic::Timeout::Callback(Base::Basic& message){
    sleep_ms(100); 
    message.int_val *= 10;
    return message;
}

void ReqRep::Basic2Basic::Timeout::RunTest(::RequesterPort<Base::Basic, Base::Basic>& req_port, ::ReplierPort<Base::Basic, Base::Basic>& rep_port){
    //Configure
    EXPECT_TRUE(rep_port.Configure());
    EXPECT_TRUE(req_port.Configure());
    
    //Activate
    EXPECT_TRUE(rep_port.Activate());
    EXPECT_TRUE(req_port.Activate());

    int send_count = 100;
    int timeout_ms = 50;
    double busy_timeout = 100.0;

    auto start = std::chrono::high_resolution_clock::now();
    //Send as fast as possible
    for(int i = 0; i < send_count; i++){
        Base::Basic b;
        b.int_val = i;
        b.str_val = std::to_string(i);
        auto c = req_port.SendRequest(b, std::chrono::milliseconds(timeout_ms));
        EXPECT_FALSE(c.first);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    auto expected_rx = duration / busy_timeout;
    auto confidence = 0.80;
    int low_rx = expected_rx * confidence;
    int high_rx = expected_rx / confidence;

    //Passivate
    EXPECT_TRUE(req_port.Passivate());
    EXPECT_TRUE(rep_port.Passivate());
    
    //Terminate
    EXPECT_TRUE(req_port.Terminate());
    EXPECT_TRUE(rep_port.Terminate());

    auto total_rxd = rep_port.GetEventsReceived();
    auto proc_rxd = rep_port.GetEventsProcessed();

    auto total_txd = req_port.GetEventsReceived();
    auto total_sent = req_port.GetEventsProcessed();

    EXPECT_EQ(total_txd, send_count);
    EXPECT_EQ(total_sent, 0);
    EXPECT_GE(total_rxd, low_rx);
    EXPECT_LE(total_rxd, high_rx);
    EXPECT_EQ(total_rxd, proc_rxd);
};

/*
    Basic to Void Tests
*/

void ReqRep::Basic2Void::Stable::Callback(Base::Basic& message){
    //Do Nothing
};

void ReqRep::Basic2Void::Stable::RunTest(
        ::RequesterPort<void, Base::Basic>& req_port,
        ::ReplierPort<void, Base::Basic>& rep_port){
    //Configure
    EXPECT_TRUE(rep_port.Configure());
    EXPECT_TRUE(req_port.Configure());
    
    //Activate
    EXPECT_TRUE(rep_port.Activate());
    EXPECT_TRUE(req_port.Activate());

    int send_count = 100;

    //Send as fast as possible
    for(int i = 0; i < send_count; i++){
        Base::Basic b;
        b.int_val = i;
        b.str_val = std::to_string(i);
        auto c = req_port.SendRequest(b, std::chrono::milliseconds(200));
        EXPECT_TRUE(c);
    }
    
    //Passivate
    EXPECT_TRUE(rep_port.Passivate());
    EXPECT_TRUE(req_port.Passivate());
    
    //Terminate
    EXPECT_TRUE(rep_port.Terminate());
    EXPECT_TRUE(req_port.Terminate());

    auto total_rxd = req_port.GetEventsReceived();
    auto proc_rxd = rep_port.GetEventsProcessed();

    auto total_txd = req_port.GetEventsReceived();
    auto total_sent = req_port.GetEventsProcessed();

    EXPECT_EQ(total_txd, send_count);
    EXPECT_EQ(total_sent, send_count);
    EXPECT_EQ(total_rxd, send_count);
    EXPECT_EQ(proc_rxd, send_count);
};

void ReqRep::Basic2Void::Busy::Callback(Base::Basic& message){
    sleep_ms(50);
};

void ReqRep::Basic2Void::Busy::RunTest(
        ::RequesterPort<void, Base::Basic>& req_port,
        ::ReplierPort<void, Base::Basic>& rep_port){
    //Configure
    EXPECT_TRUE(rep_port.Configure());
    EXPECT_TRUE(req_port.Configure());
    
    //Activate
    EXPECT_TRUE(rep_port.Activate());
    EXPECT_TRUE(req_port.Activate());

    int send_count = 100;

    //Send as fast as possible
    for(int i = 0; i < send_count; i++){
        Base::Basic b;
        b.int_val = i;
        b.str_val = std::to_string(i);
        auto c = req_port.SendRequest(b, std::chrono::milliseconds(250));
        EXPECT_TRUE(c);
    }
    
    //Passivate
    EXPECT_TRUE(rep_port.Passivate());
    EXPECT_TRUE(req_port.Passivate());
    
    //Terminate
    EXPECT_TRUE(rep_port.Terminate());
    EXPECT_TRUE(req_port.Terminate());

    auto total_rxd = req_port.GetEventsReceived();
    auto proc_rxd = rep_port.GetEventsProcessed();

    auto total_txd = req_port.GetEventsReceived();
    auto total_sent = req_port.GetEventsProcessed();

    EXPECT_EQ(total_txd, send_count);
    EXPECT_EQ(total_sent, send_count);
    EXPECT_EQ(total_rxd, send_count);
    EXPECT_EQ(proc_rxd, send_count);
};

void ReqRep::Basic2Void::Timeout::Callback(Base::Basic& message){
    sleep_ms(100); 
}

void ReqRep::Basic2Void::Timeout::RunTest(::RequesterPort<void, Base::Basic>& req_port, ::ReplierPort<void, Base::Basic>& rep_port){
    //Configure
    EXPECT_TRUE(rep_port.Configure());
    EXPECT_TRUE(req_port.Configure());
    
    //Activate
    EXPECT_TRUE(rep_port.Activate());
    EXPECT_TRUE(req_port.Activate());

    int send_count = 100;
    int timeout_ms = 50;
    double busy_timeout = 100.0;

    auto start = std::chrono::high_resolution_clock::now();
    //Send as fast as possible
    for(int i = 0; i < send_count; i++){
        Base::Basic b;
        b.int_val = i;
        b.str_val = std::to_string(i);
        auto c = req_port.SendRequest(b, std::chrono::milliseconds(timeout_ms));
        EXPECT_FALSE(c);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    auto expected_rx = duration / busy_timeout;
    auto confidence = 0.80;
    int low_rx = expected_rx * confidence;
    int high_rx = expected_rx / confidence;

    //Passivate
    EXPECT_TRUE(req_port.Passivate());
    EXPECT_TRUE(rep_port.Passivate());
    
    //Terminate
    EXPECT_TRUE(req_port.Terminate());
    EXPECT_TRUE(rep_port.Terminate());

    auto total_rxd = rep_port.GetEventsReceived();
    auto proc_rxd = rep_port.GetEventsProcessed();

    auto total_txd = req_port.GetEventsReceived();
    auto total_sent = req_port.GetEventsProcessed();

    EXPECT_EQ(total_txd, send_count);
    EXPECT_EQ(total_sent, 0);
    EXPECT_GE(total_rxd, low_rx);
    EXPECT_LE(total_rxd, high_rx);
    EXPECT_EQ(total_rxd, proc_rxd);
};

/*
    Void to Basic Tests
*/

Base::Basic ReqRep::Void2Basic::Stable::Callback(){
    Base::Basic message;
    message.int_val = 10;
    message.str_val = "Void2Basic";
    return message;
};

void ReqRep::Void2Basic::Stable::RunTest(
        ::RequesterPort<Base::Basic, void>& req_port,
        ::ReplierPort<Base::Basic, void>& rep_port){
    //Configure
    EXPECT_TRUE(rep_port.Configure());
    EXPECT_TRUE(req_port.Configure());
    
    //Activate
    EXPECT_TRUE(rep_port.Activate());
    EXPECT_TRUE(req_port.Activate());

    int send_count = 100;

    //Send as fast as possible
    for(int i = 0; i < send_count; i++){
        auto c = req_port.SendRequest(std::chrono::milliseconds(200));
        EXPECT_TRUE(c.first);
        if(c.first){
            EXPECT_EQ(c.second.int_val, 10);
            EXPECT_EQ(c.second.str_val, "Void2Basic");
        }
    }
    
    //Passivate
    EXPECT_TRUE(req_port.Passivate());
    EXPECT_TRUE(rep_port.Passivate());
    
    //Terminate
    EXPECT_TRUE(req_port.Terminate());
    EXPECT_TRUE(rep_port.Terminate());

    auto total_rxd = rep_port.GetEventsReceived();
    auto proc_rxd = rep_port.GetEventsProcessed();

    auto total_txd = req_port.GetEventsReceived();
    auto total_sent = req_port.GetEventsProcessed();

    EXPECT_EQ(total_txd, send_count);
    EXPECT_EQ(total_sent, send_count);
    EXPECT_EQ(total_rxd, send_count);
    EXPECT_EQ(proc_rxd, send_count);
};

Base::Basic ReqRep::Void2Basic::Busy::Callback(){
    sleep_ms(50);
    Base::Basic message;
    message.int_val = 10;
    message.str_val = "Void2Basic";
    return message;
};

void ReqRep::Void2Basic::Busy::RunTest(
        ::RequesterPort<Base::Basic, void>& req_port,
        ::ReplierPort<Base::Basic, void>& rep_port){
    //Configure
    EXPECT_TRUE(rep_port.Configure());
    EXPECT_TRUE(req_port.Configure());
    
    //Activate
    EXPECT_TRUE(rep_port.Activate());
    EXPECT_TRUE(req_port.Activate());

    int send_count = 100;

    //Send as fast as possible
    for(int i = 0; i < send_count; i++){
        auto c = req_port.SendRequest(std::chrono::milliseconds(250));
        EXPECT_TRUE(c.first);
        if(c.first){
            EXPECT_EQ(c.second.int_val, 10);
            EXPECT_EQ(c.second.str_val, "Void2Basic");
        }
    }
    
    //Passivate
    EXPECT_TRUE(req_port.Passivate());
    EXPECT_TRUE(rep_port.Passivate());
    
    //Terminate
    EXPECT_TRUE(req_port.Terminate());
    EXPECT_TRUE(rep_port.Terminate());

    auto total_rxd = rep_port.GetEventsReceived();
    auto proc_rxd = rep_port.GetEventsProcessed();

    auto total_txd = req_port.GetEventsReceived();
    auto total_sent = req_port.GetEventsProcessed();

    EXPECT_EQ(total_txd, send_count);
    EXPECT_EQ(total_sent, send_count);
    EXPECT_EQ(total_rxd, send_count);
    EXPECT_EQ(proc_rxd, send_count);
};

Base::Basic ReqRep::Void2Basic::Timeout::Callback(){
    sleep_ms(100); 
    Base::Basic message;
    return message;
}

void ReqRep::Void2Basic::Timeout::RunTest(::RequesterPort<Base::Basic, void>& req_port, ::ReplierPort<Base::Basic, void>& rep_port){
    //Configure
    EXPECT_TRUE(rep_port.Configure());
    EXPECT_TRUE(req_port.Configure());
    
    //Activate
    EXPECT_TRUE(rep_port.Activate());
    EXPECT_TRUE(req_port.Activate());

    int send_count = 100;
    int timeout_ms = 50;
    double busy_timeout = 100.0;

    auto start = std::chrono::high_resolution_clock::now();
    //Send as fast as possible
    for(int i = 0; i < send_count; i++){
        auto c = req_port.SendRequest(std::chrono::milliseconds(timeout_ms));
        EXPECT_FALSE(c.first);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    auto expected_rx = duration / busy_timeout;
    auto confidence = 0.80;
    int low_rx = expected_rx * confidence;
    int high_rx = expected_rx / confidence;

    //Passivate
    EXPECT_TRUE(req_port.Passivate());
    EXPECT_TRUE(rep_port.Passivate());
    
    //Terminate
    EXPECT_TRUE(req_port.Terminate());
    EXPECT_TRUE(rep_port.Terminate());

    auto total_rxd = rep_port.GetEventsReceived();
    auto proc_rxd = rep_port.GetEventsProcessed();

    auto total_txd = req_port.GetEventsReceived();
    auto total_sent = req_port.GetEventsProcessed();

    EXPECT_EQ(total_txd, send_count);
    EXPECT_EQ(total_sent, 0);
    EXPECT_GE(total_rxd, low_rx);
    EXPECT_LE(total_rxd, high_rx);
    EXPECT_EQ(total_rxd, proc_rxd);
};