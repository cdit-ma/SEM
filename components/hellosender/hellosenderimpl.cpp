#include "hellosenderimpl.h"
#include <iostream>

HelloSenderImpl::HelloSenderImpl(std::string name) : HelloSenderInt(name){

};

void HelloSenderImpl::tick(){
    this->sentCount_ ++;
    {
        Message* msg = new Message();
        msg->set_time(sentCount_);
        msg->set_instName(this->instName());
        msg->set_content(this->message());
        txMessage(msg);
    }

    {
        Message* msg = new Message();
        msg->set_time(sentCount_);
        msg->set_instName(this->instName());
        msg->set_content(this->message());


        auto m = new VectorMessage();
        m->set_dataName("test");


        for(int i = 0; i < sentCount_ && i < 10; i++){
            Message ms;
            ms.set_time(i);
            ms.set_instName(this->instName());
            ms.set_content(this->message());
            
            m->Vector_Inst().push_back(ms);
            m->data().push_back(i);
        }

        m->set_msgInst(*msg);
        
        txVectorMessage(m);
    }
}