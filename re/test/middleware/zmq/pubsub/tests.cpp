#include "gtest/gtest.h"
#include "../../../core/activatablefsmtester.h"

//Include the premade reqrep tests
#include "../../base/pubsubtests.hpp"

// Include the proto convert functions for the port type
#include "basic.pb.h"

#include "ports/libportexport.h"
#include "pubsub/publisherport.hpp"
#include "pubsub/subscriberport.hpp"

static int port_id = 50000;

bool setup_pub_port(Port& port, int test_id){
    auto address = port.GetAttribute("publisher_address").lock();
    if(address){
        std::string port_address = "tcp://127.0.0.1:" + std::to_string(test_id);
        address->set_String(port_address);
        return true;
    }
    return false;
}

bool setup_sub_port(Port& port, int test_id){
    auto address = port.GetAttribute("publisher_addresses").lock();
    if(address){
        std::string port_address = "tcp://127.0.0.1:" + std::to_string(test_id);
        address->StringList().emplace_back(port_address);
        return true;
    }
    return false;
}

void EmptyCallback(Base::Basic& m){
};

//Define an In/Out Port FSM Tester
class Re_Port_Sub_Zmq_FSM : public ActivatableFSMTester{
    protected:
        void SetUp(){
            ActivatableFSMTester::SetUp();
            auto port_name = get_long_test_name();
            component->RegisterCallback<void, Base::Basic>(port_name, EmptyCallback);
            auto port = ConstructSubscriberPort<zmq::SubscriberPort<Base::Basic, ::Basic> >(port_name, component);
            auto port_number = ++port_id;
            EXPECT_TRUE(setup_sub_port(*port, port_number));
            a = port;
            ASSERT_TRUE(a);
        }
};

class Re_Port_Pub_Zmq_FSM : public ActivatableFSMTester{
protected:
    void SetUp(){
        ActivatableFSMTester::SetUp();
        auto port_name = get_long_test_name();
        auto port = ConstructPublisherPort<zmq::PublisherPort<Base::Basic, ::Basic> >(port_name, component);
        auto port_number = ++port_id;
        EXPECT_TRUE(setup_pub_port(*port, port_number));
        a = port;
        ASSERT_TRUE(a);
    }
};

#define TEST_FSM_CLASS Re_Port_Sub_Zmq_FSM
#include "../../../core/activatablefsmtestcases.h"
#undef TEST_FSM_CLASS

#define TEST_FSM_CLASS Re_Port_Pub_Zmq_FSM
#include "../../../core/activatablefsmtestcases.h"
#undef TEST_FSM_CLASS


TEST(Re_Port_PubSub_Zmq, DISABLED_Basic_Stable){
    using namespace ::PubSub::Basic::Stable;

    //Define the base types
    using base_type = Base::Basic;
    using mw_type = ::Basic;

    auto test_name = get_long_test_name();
    
    auto rx_callback_count = 0;
    //Define a callback wrapper
    CallbackWrapper<void, base_type> callback_wrapper([&rx_callback_count](base_type&){
        rx_callback_count ++;
    });

    auto c = std::make_shared<Component>("c_" + test_name);
    zmq::PublisherPort<base_type, mw_type> pub_port(c, "tx_" + test_name);
    zmq::SubscriberPort<base_type, mw_type> sub_port(c, "rx_" + test_name, callback_wrapper);

    auto address = "tcp://" + test_name;

    //Setup Ports
    auto port_number = ++port_id;
    EXPECT_TRUE(setup_pub_port(pub_port, port_number));
    EXPECT_TRUE(setup_sub_port(sub_port, port_number));

    RunTest(pub_port, sub_port, rx_callback_count);
}

// Disabled the basic::busy test for ZeroMQ due to non-determinism causing otherwise fine builds to fail regularly
TEST(Re_Port_PubSub_Zmq, DISABLED_Basic_Busy){
    using namespace ::PubSub::Basic::Busy;

    //Define the base types
    using base_type = Base::Basic;
    using mw_type = ::Basic;

    auto test_name = get_long_test_name();
    
    auto rx_callback_count = 0;
    //Define a callback wrapper
    CallbackWrapper<void, base_type> callback_wrapper([&rx_callback_count](base_type&){
        rx_callback_count ++;
        sleep_ms(1000);
    });

    auto c = std::make_shared<Component>("c_" + test_name);
    zmq::PublisherPort<base_type, mw_type> pub_port(c, "tx_" + test_name);
    zmq::SubscriberPort<base_type, mw_type> sub_port(c, "rx_" + test_name, callback_wrapper);

    auto port_number = ++port_id;
    EXPECT_TRUE(setup_pub_port(pub_port, port_number));
    EXPECT_TRUE(setup_sub_port(sub_port, port_number));

    RunTest(pub_port, sub_port, rx_callback_count);
}

TEST(Re_Port_PubSub_Zmq, Basic_Terminate){
    using namespace ::PubSub::Basic::Terminate;

    //Define the base types
    using base_type = Base::Basic;
    using mw_type = ::Basic;

    auto test_name = get_long_test_name();
    
    auto rx_callback_count = 0;
    //Define a callback wrapper
    CallbackWrapper<void, base_type> callback_wrapper([&rx_callback_count](base_type&){
        rx_callback_count ++;
    });

    auto c = std::make_shared<Component>("c_" + test_name);
    zmq::PublisherPort<base_type, mw_type> pub_port(c, "tx_" + test_name);
    zmq::SubscriberPort<base_type, mw_type> sub_port(c, "rx_" + test_name, callback_wrapper);

    auto port_number = ++port_id;
    EXPECT_TRUE(setup_pub_port(pub_port, port_number));
    EXPECT_TRUE(setup_sub_port(sub_port, port_number));

    RunTest(pub_port, sub_port, rx_callback_count);
}

#include "controlmessage.pb.h"
//#include "deploymentcontainer.h"
//#include "component.h"

// TODO: Rework this test.
/*TEST(Re_Port_PubSub_Zmq, Deadlock){
    using namespace ::PubSub::Basic::Terminate;

    //Define the base types
    using base_type = Base::Basic;
    using mw_type = ::Basic;

    auto test_name = get_long_test_name();
    
    auto rx_callback_count = 0;
    std::unique_ptr<Component> c = std::unique_ptr<Component>(new Component("c_" + test_name));
    auto& component = *c;

    //Define a callback wrapper
    CallbackWrapper<void, base_type> callback_wrapper([&](base_type& m){
        sleep_ms(500);
        auto p_port = component.SafeGetTypedPort<PublisherPort<base_type>>("tx_" + test_name);
        if(p_port){
            p_port->Send(m);
        }
    });

    // XXX: Rework this test?
    //  TODO: We have since refactored container to no longer take these arguments. Reworking this test
    //   will need to be done at a later date.
    DeploymentContainer container("test", "host", ".");

    {
        auto component_sptr = container.AddComponent(std::move(c), "c_" + test_name).lock();

        auto pub_port = std::unique_ptr<zmq::PublisherPort<base_type, mw_type>>(new zmq::PublisherPort<base_type, mw_type>(component_sptr, "tx_" + test_name));
        auto sub_port = std::unique_ptr<zmq::SubscriberPort<base_type, mw_type>>(new zmq::SubscriberPort<base_type, mw_type>(component_sptr, "rx_" + test_name, callback_wrapper));

        auto port_number = ++port_id;
        EXPECT_TRUE(setup_pub_port(*pub_port, port_number));
        EXPECT_TRUE(setup_sub_port(*sub_port, port_number));

        component_sptr->AddEventPort(std::move(pub_port));
        component_sptr->AddEventPort(std::move(sub_port));

        //Attach a Logan
        std::string id{"logan_id"};
        auto logan_client = std::unique_ptr<LoganClient>(new LoganClient(id));
        logan_client->SetEndpoint("127.0.0.1", std::to_string(++port_id));
        logan_client->SetFrequency(1);
        logan_client->SetLiveMode(false);
        container.AddLoganClient(std::move(logan_client), id);
    }

    EXPECT_TRUE(((Activatable&)container).Configure());
    EXPECT_TRUE(container.Activate());

    sleep_ms(500);
    {
        Base::Basic b;
        auto p_port = component.GetTypedPort<PublisherPort<base_type>>("tx_" + test_name);
        p_port->Send(b);
    }
    sleep_ms(1000);

    EXPECT_TRUE(container.Passivate());
    EXPECT_TRUE(container.Terminate());
}*/