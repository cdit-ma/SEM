#include "gtest/gtest.h"
#include "../../../core/activatablefsmtester.h"

//Include the premade reqrep tests
#include "../../base/pubsubtests.hpp"

// Include the proto convert functions for the port type
#include "basic.pb.h"

#include <core/ports/libportexport.h>
#include <middleware/zmq/pubsub/publisherport.hpp>
#include <middleware/zmq/pubsub/subscriberport.hpp>

static int port_id = 7000;

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
class zmq_SubPort_FSMTester : public ActivatableFSMTester{
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

class zmq_PubPort_FSMTester : public ActivatableFSMTester{
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

#define TEST_FSM_CLASS zmq_SubPort_FSMTester
#include "../../../core/activatablefsmtestcases.h"
#undef TEST_FSM_CLASS

#define TEST_FSM_CLASS zmq_PubPort_FSMTester
#include "../../../core/activatablefsmtestcases.h"
#undef TEST_FSM_CLASS

TEST(zmq_PubSub, Basic_Stable){
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

TEST(zmq_PubSub, Basic_Busy){
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

TEST(zmq_PubSub, Basic_Terminate){
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

#include <proto/controlmessage/controlmessage.pb.h>
#include <nodemanager/deploymentcontainer.h>

TEST(zmq_PubSub, Deadlock){
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
        std::cerr << "GOT Message " << std::endl;
        sleep_ms(500);
        std::cerr << "WOKEN" << std::endl;
        auto p_port = component.GetTypedPort<PublisherPort<base_type>>("tx_" + test_name);
        if(p_port){
            std::cerr << " GOT PORT" << std::endl;
            p_port->Send(m);
            std::cerr << " SENT M" << std::endl;
        }else{
            std::cerr << " NO PORT" << std::endl;
        }
    });
    
    DeploymentContainer container("test", "host", ".");

    {
        container.AddLoganLogger(std::unique_ptr<Logan::Logger>(new Logan::Logger("test", "host", "container", "test", "127.0.0.1", std::to_string(++port_id), Logger::Mode::CACHED)));

        auto component_sptr = container.AddComponent(std::move(c), "c_" + test_name).lock();
        std::cerr << component_sptr << std::endl;
        

        auto pub_port = std::unique_ptr<zmq::PublisherPort<base_type, mw_type>>(new zmq::PublisherPort<base_type, mw_type>(component_sptr, "tx_" + test_name));
        auto sub_port = std::unique_ptr<zmq::SubscriberPort<base_type, mw_type>>(new zmq::SubscriberPort<base_type, mw_type>(component_sptr, "rx_" + test_name, callback_wrapper));
        
        container.SetLoggers(*component_sptr);
        container.SetLoggers(*pub_port);
        container.SetLoggers(*sub_port);
    
        auto port_number = ++port_id;
        EXPECT_TRUE(setup_pub_port(*pub_port, port_number));
        EXPECT_TRUE(setup_sub_port(*sub_port, port_number));

        component_sptr->AddPort(std::move(pub_port));
        component_sptr->AddPort(std::move(sub_port));
        std::string id{"logan_id"};
        auto logan_client = std::unique_ptr<LoganClient>(new LoganClient(id));
        logan_client->SetEndpoint("127.0.0.1", std::to_string(++port_id));
        logan_client->SetFrequency(10);
        logan_client->SetLiveMode(false);
        container.AddLoganClient(std::move(logan_client), id);
        
        
        
    }

    
    

    std::cerr << "Configure" << std::endl;
    EXPECT_TRUE(((Activatable&)container).Configure());
    std::cerr << "Activate" << std::endl;
    EXPECT_TRUE(container.Activate());
    std::cerr << "Activated" << std::endl;

    

    sleep_ms(1000);
    {
        Base::Basic b;
        auto p_port = component.GetTypedPort<PublisherPort<base_type>>("tx_" + test_name);
        std::cerr << "SENT MESSAGE" << std::endl;
        p_port->Send(b);
    }
    sleep_ms(10);

    std::cerr << "PASSIVATE" << std::endl;
    EXPECT_TRUE(container.Passivate());
    std::cerr << "PASSIVATED" << std::endl;
    std::cerr << "TERMINATE" << std::endl;
    EXPECT_TRUE(container.Terminate());
    std::cerr << "TERMINATED" << std::endl;
}