#include "deployment_2.h"

#include <iostream>
#include <core/periodiceventport.h>

//Implementations for the components
#include "../../components/hellosender/hellosenderimpl.h"
#include "../../components/helloreceiver/helloreceiverimpl.h"

//ZMQ Implementation of the event ports
//#include "../../middleware/zmq/message/tx.h"
//#include "../../middleware/zmq/message/rx.h"
#include "../../middleware/zmq/test.h"

#include <iostream>
#include <map>

Deployment_2::Deployment_2(){
    auto zmq_rx = std::bind<EventPort*>(zmq::construct_rx, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    auto zmq_tx = std::bind<EventPort*>(zmq::construct_tx, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

    auto sender_c = std::bind<Component*>(HelloSenderImpl::constructor_fn, std::placeholders::_1);
    auto receiver_c = std::bind<Component*>(HelloReceiverImpl::constructor_fn, std::placeholders::_1);

    add_tx_constructor("zmq", zmq_tx);
    add_rx_constructor("zmq", zmq_rx);

    
    add_component_constructor("HelloSender", sender_c);
    add_component_constructor("HelloReceiver", receiver_c);
}
