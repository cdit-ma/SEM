#include "rtirxmessage.h"

#include "message.hpp"

#include "messageconvert.h"

//Include the templated OutEventPort Implementation for OSPL
#include "rti/ineventport.hpp"




EXPORT_FUNC ::InEventPort<::Message>* rti::construct_RxMessage(Component* component, 
                                                        std::function<void (::Message*)> callback_function, 
                                                        int domain_id, 
                                                        std::string subscriber_name, 
                                                        std::string topic_name){

    auto p = new rti::InEventPort<::Message, cdit::Message>(component, callback_function, 
                                                            domain_id, 
                                                            subscriber_name, 
                                                            topic_name);
    return p;
}