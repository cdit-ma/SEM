#include "convert.h"
#include "vectormessage.pb.h"

cdit::VectorMessage* proto::translate(::VectorMessage* message){
        auto out = new cdit::VectorMessage();
        out->set_dataname(message->dataName());

        for(auto e: message->data()){
                out->add_data(e);
        }

        return out;
}

::VectorMessage* proto::translate(cdit::VectorMessage* message){
        auto out = new ::VectorMessage();
        out->dataName(message->dataname());

        for(int i = 0; i < message->data_size(); i++){
                out->data_ptr().push_back(message->data(i));
        }

        return out;
}

template <>
::VectorMessage* proto::decode <cdit::VectorMessage>(std::string message){
        auto pb = new cdit::VectorMessage();
        pb->ParseFromString(message);
        auto m = translate(pb);
        delete pb;
        return m;
}

std::string proto::encode(::VectorMessage* message){
        std::string str;
        auto pb = translate(message);
        pb->SerializeToString(&str);
        delete pb;
        return str;
}