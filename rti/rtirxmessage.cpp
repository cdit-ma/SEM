#include "rtirxmessage.h"

#include "message.hpp"

//Include the templated OutEventPort Implementation for OSPL
#include "rti/ineventport.hpp"

#include "messageconvert.h"


EXPORT_FUNC ::InEventPort<::Message>* rti::construct_RxMessage(Component* component, 
                                                        std::function<void (::Message*)> callback_function, 
                                                        int domain_id, 
                                                        std::string subscriber_name, 
                                                        std::string topic_name){

    auto p = new rti::InEventPort<::Message, proto::Message>(component, callback_function, 
                                                            domian_id, 
                                                            subscriber_name, 
                                                            topic_name);
    return p;
}