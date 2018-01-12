#include "gtest/gtest.h"

// Include the convert functions for the port type
#include "convert.hpp"
#include <middleware/ospl/ineventport.hpp>
#include <middleware/ospl/outeventport.hpp>
#include <middleware/ospl/ddshelper.h>


//Include the FSM Tester
#include "../../core/activatablefsmtester.h"

#include <vector>
#include <algorithm>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>


void empty_callback(Base::Basic& b){};

bool setup_port(EventPort& port, int domain, std::string topic_name){
    auto d = port.GetAttribute("domain_id").lock();
    auto t = port.GetAttribute("topic_name").lock();
   
    if(d && t){
        d->set_Integer(domain);
        t->set_String(topic_name);
        return true;
    }
    return false;
}

//Define an In/Out Port FSM Tester
class OSPL_InEventPort_FSMTester : public ActivatableFSMTester{
    protected:
        void SetUp(){
            ActivatableFSMTester::SetUp();
            auto port_name = get_long_test_name();
            auto port = new ospl::InEventPort<Base::Basic, Basic>(std::weak_ptr<Component>(),  port_name, empty_callback);
            
            EXPECT_TRUE(setup_port(*port, 0, port_name));

            a = port;
            ASSERT_TRUE(a);
        }
};

class OSPL_OutEventPort_FSMTester : public ActivatableFSMTester{
protected:
    void SetUp(){
        ActivatableFSMTester::SetUp();
        auto port_name = get_long_test_name();
        auto port = new ospl::OutEventPort<Base::Basic, Basic>(std::weak_ptr<Component>(), port_name);
        EXPECT_TRUE(setup_port(*port, 0, port_name));
        a = port;
        ASSERT_TRUE(a);
    }
};


#define TEST_FSM_CLASS OSPL_InEventPort_FSMTester
//#include "../../core/activatablefsmtestcases.h"
#undef TEST_FSM_CLASS

#define TEST_FSM_CLASS OSPL_OutEventPort_FSMTester
//#include "../../core/activatablefsmtestcases.h"
#undef TEST_FSM_CLASS

TEST(OSPL_EventportPair, Stable100){
    {
    auto participant = ospl::DdsHelper::get_dds_helper()->get_participant(0);
    
    {

        auto test_name = get_long_test_name();
        auto rx_callback_count = 0;
        int send_count = 100;

        boost::uuids::random_generator uuid_generator_;

        std::vector<std::string> guids;

        for(int i = 0; i < send_count; i++){
            guids.push_back(boost::uuids::to_string(uuid_generator_()));
        }

        std::vector<std::string> guids_copy = guids;

        auto c = std::make_shared<Component>("Test");
        ospl::OutEventPort<Base::Basic, Basic> out_port(c, "tx_" + test_name);
        ospl::InEventPort<Base::Basic, Basic> in_port(c, "rx_" + test_name, [&guids_copy, &rx_callback_count](Base::Basic& message){
                rx_callback_count ++;
                guids_copy.erase(std::remove(guids_copy.begin(), guids_copy.end(), message.guid_val), guids_copy.end());
        });

        EXPECT_TRUE(setup_port(out_port, 0, test_name));
        EXPECT_TRUE(setup_port(in_port, 0, test_name));
        

        EXPECT_TRUE(in_port.Configure());
        EXPECT_TRUE(out_port.Configure());
        EXPECT_TRUE(in_port.Activate());
        EXPECT_TRUE(out_port.Activate());


        //Send as fast as possible
        for(int i = 0; i < send_count; i++){
        Base::Basic b;
            b.int_val = i;
            b.str_val = std::to_string(i);
            b.guid_val = guids[i];
            out_port.tx(b);
            sleep_ms(1);
        }


        EXPECT_TRUE(in_port.Passivate());
        EXPECT_TRUE(out_port.Passivate());
        EXPECT_TRUE(in_port.Terminate());
        EXPECT_TRUE(out_port.Terminate());

        auto total_rxd = in_port.GetEventsReceived();
        auto proc_rxd = in_port.GetEventsProcessed();

        auto total_txd = out_port.GetEventsReceived();
        auto total_sent = out_port.GetEventsProcessed();

        EXPECT_EQ(guids_copy.size(), 0);

        EXPECT_EQ(total_txd, send_count);
        EXPECT_EQ(total_sent, send_count);
        EXPECT_EQ(total_rxd, send_count);
        EXPECT_EQ(proc_rxd, send_count);
        EXPECT_EQ(rx_callback_count, send_count);
    }
    EXPECT_TRUE(true);
    delete ospl::DdsHelper::get_dds_helper();
    std::cout << "THIS IS A PRINT" << std::endl;
    EXPECT_TRUE(true);}

    std::cout << "dead partic" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
}



//Run a blocking callback which runs for 1 second,
//During that one second, send maximum num
TEST(OSPL_EventportPair, Busy100){
    auto test_name = get_long_test_name();
    auto rx_callback_count = 0;

    auto c = std::make_shared<Component>("Test");
    ospl::OutEventPort<Base::Basic, Basic> out_port(c, "tx_" + test_name);
    ospl::InEventPort<Base::Basic, Basic> in_port(c, "rx_" + test_name, [&rx_callback_count, &out_port](Base::Basic&){
            rx_callback_count ++;
            sleep_ms(1000);
    });
    
    EXPECT_TRUE(setup_port(out_port, 0, test_name));
    EXPECT_TRUE(setup_port(in_port, 0, test_name));


    EXPECT_TRUE(in_port.Configure());
    EXPECT_TRUE(out_port.Configure());
    EXPECT_TRUE(in_port.Activate());
    EXPECT_TRUE(out_port.Activate());

    int send_count = 100;

    //Send as fast as possible
    for(int i = 0; i < send_count; i++){
        Base::Basic b;
        b.int_val = i;
        b.str_val = std::to_string(i);
        out_port.tx(b);
    }

    //Sleep for a reasonable time (Bigger than the callback work)
    sleep_ms(500);

    EXPECT_TRUE(out_port.Passivate());
    EXPECT_TRUE(in_port.Passivate());

    EXPECT_TRUE(out_port.Terminate());
    EXPECT_TRUE(in_port.Terminate());

    auto total_rxd = in_port.GetEventsReceived();
    auto proc_rxd = in_port.GetEventsProcessed();

    auto total_txd = out_port.GetEventsReceived();
    auto total_sent = out_port.GetEventsProcessed();

    EXPECT_EQ(total_txd, send_count);
    EXPECT_EQ(total_sent, send_count);
    EXPECT_EQ(total_rxd, send_count);
    EXPECT_EQ(proc_rxd, 1);
    EXPECT_EQ(rx_callback_count, 1);
}

TEST(OSPL_EventportPair, PointerTest){
    auto test_name = get_long_test_name();
    auto rx_callback_count = 0;

    auto c = std::make_shared<Component>("Test");
    auto out_port = new ospl::OutEventPort<Base::Basic, Basic>(c, "tx_" + test_name);
    auto in_port = new ospl::InEventPort<Base::Basic, Basic>(c, "rx_" + test_name, [&rx_callback_count, &out_port](Base::Basic&){
            rx_callback_count ++;
            sleep_ms(1000);
    });
    
    EXPECT_TRUE(setup_port(*out_port, 0, test_name));
    EXPECT_TRUE(setup_port(*in_port, 0, test_name));


    EXPECT_TRUE(in_port->Configure());
    EXPECT_TRUE(out_port->Configure());
    EXPECT_TRUE(in_port->Activate());
    EXPECT_TRUE(out_port->Activate());

    int send_count = 100;

    //Send as fast as possible
    for(int i = 0; i < send_count; i++){
        Base::Basic b;
        b.int_val = i;
        b.str_val = std::to_string(i);
        out_port->tx(b);
    }

    //Sleep for a reasonable time (Bigger than the callback work)
    sleep_ms(500);

    EXPECT_TRUE(out_port->Passivate());
    EXPECT_TRUE(in_port->Passivate());

    EXPECT_TRUE(out_port->Terminate());
    EXPECT_TRUE(in_port->Terminate());

    auto total_rxd = in_port->GetEventsReceived();
    auto proc_rxd = in_port->GetEventsProcessed();

    auto total_txd = out_port->GetEventsReceived();
    auto total_sent = out_port->GetEventsProcessed();

    EXPECT_EQ(total_txd, send_count);
    EXPECT_EQ(total_sent, send_count);
    EXPECT_EQ(total_rxd, send_count);
    EXPECT_EQ(proc_rxd, 1);
    EXPECT_EQ(rx_callback_count, 1);
}

TEST(OSPL_EventportPair, TestyTest2){
    auto test_name = get_long_test_name();
    auto rx_callback_count = 0;
    int send_count = 100;

    boost::uuids::random_generator uuid_generator_;

    std::vector<std::string> guids;

    for(int i = 0; i < send_count; i++){
        guids.push_back(boost::uuids::to_string(uuid_generator_()));
    }

    std::vector<std::string> guids_copy = guids;

    auto c = std::make_shared<Component>("Test");
    ospl::OutEventPort<Base::Basic, Basic> out_port(c, "tx_" + test_name);
    ospl::InEventPort<Base::Basic, Basic> in_port(c, "rx_" + test_name, [&guids_copy, &rx_callback_count](Base::Basic& message){
            rx_callback_count ++;
            guids_copy.erase(std::remove(guids_copy.begin(), guids_copy.end(), message.guid_val), guids_copy.end());
    });

    //EXPECT_TRUE(setup_port(out_port, 0, test_name));
    EXPECT_TRUE(setup_port(in_port, 0, test_name));
    

    EXPECT_TRUE(in_port.Configure());
    //EXPECT_TRUE(out_port.Configure());
    EXPECT_TRUE(in_port.Activate());
    //EXPECT_TRUE(out_port.Activate());


    //Send as fast as possible
    for(int i = 0; i < send_count; i++){
       Base::Basic b;
        b.int_val = i;
        b.str_val = std::to_string(i);
        b.guid_val = guids[i];
        out_port.tx(b);
        sleep_ms(1);
    }


    EXPECT_TRUE(in_port.Passivate());
    //EXPECT_TRUE(out_port.Passivate());
    EXPECT_TRUE(in_port.Terminate());
    //EXPECT_TRUE(out_port.Terminate());

    // auto total_rxd = in_port.GetEventsReceived();
    // auto proc_rxd = in_port.GetEventsProcessed();

    // auto total_txd = out_port.GetEventsReceived();
    // auto total_sent = out_port.GetEventsProcessed();

    // //EXPECT_EQ(guids_copy.size(), 0);

    // //EXPECT_EQ(total_txd, send_count);
    // //EXPECT_EQ(total_sent, send_count);
    // EXPECT_EQ(total_rxd, send_count);
    // EXPECT_EQ(proc_rxd, send_count);
    // EXPECT_EQ(rx_callback_count, send_count);
}

TEST(OSPL_EventportPair, TestyTest){
    auto test_name = get_long_test_name();
    auto rx_callback_count = 0;
    int send_count = 100;

    boost::uuids::random_generator uuid_generator_;

    std::vector<std::string> guids;

    for(int i = 0; i < send_count; i++){
        guids.push_back(boost::uuids::to_string(uuid_generator_()));
    }

    std::vector<std::string> guids_copy = guids;

    auto c = std::make_shared<Component>("Test");
    ospl::OutEventPort<Base::Basic, Basic> out_port(c, "tx_" + test_name);
    ospl::InEventPort<Base::Basic, Basic> in_port(c, "rx_" + test_name, [&guids_copy, &rx_callback_count](Base::Basic& message){
            rx_callback_count ++;
            guids_copy.erase(std::remove(guids_copy.begin(), guids_copy.end(), message.guid_val), guids_copy.end());
    });

    EXPECT_TRUE(setup_port(out_port, 0, test_name));
    //EXPECT_TRUE(setup_port(in_port, 0, test_name));
    

    //EXPECT_TRUE(in_port.Configure());
    EXPECT_TRUE(out_port.Configure());
    //EXPECT_TRUE(in_port.Activate());
    EXPECT_TRUE(out_port.Activate());


    //Send as fast as possible
    for(int i = 0; i < send_count; i++){
       Base::Basic b;
        b.int_val = i;
        b.str_val = std::to_string(i);
        b.guid_val = guids[i];
        out_port.tx(b);
        sleep_ms(1);
    }


    //EXPECT_TRUE(in_port.Passivate());
    EXPECT_TRUE(out_port.Passivate());
    //EXPECT_TRUE(in_port.Terminate());
    EXPECT_TRUE(out_port.Terminate());

    //auto total_rxd = in_port.GetEventsReceived();
    //auto proc_rxd = in_port.GetEventsProcessed();

    auto total_txd = out_port.GetEventsReceived();
    auto total_sent = out_port.GetEventsProcessed();

    //EXPECT_EQ(guids_copy.size(), 0);

    EXPECT_EQ(total_txd, send_count);
    EXPECT_EQ(total_sent, send_count);
    //EXPECT_EQ(total_rxd, send_count);
    //EXPECT_EQ(proc_rxd, send_count);
    //EXPECT_EQ(rx_callback_count, send_count);
}

int main(int ac, char* av[])
{
    testing::InitGoogleTest(&ac, av);
    return RUN_ALL_TESTS();
}