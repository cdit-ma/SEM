#ifndef DDSTXINTERFACE_H
#define DDSTXINTERFACE_H

#include <string>
#include <dds/dds.hpp>


template <class T, class S> class DDS_TX_Interface{
        public:
            DDS_TX_Interface(dds::pub::Publisher publisher, std::string topic_name);

            void push_message(S* s);
        private:
            dds::pub::Publisher publisher_ = dds::pub::Publisher(dds::core::null);
            dds::pub::DataWriter<T> writer_ = dds::pub::DataWriter<T>(dds::core::null);
};

template <class T, class S>
DDS_TX_Interface<T,S>::DDS_TX_Interface(dds::pub::Publisher publisher, std::string topic_name){
    this->publisher_ = publisher;

    //Try find first
    auto topic = dds::topic::find<dds::topic::Topic<T> >(publisher_.participant(), topic_name);
    if(topic == dds::core::null){
        //Construct
        topic = dds::topic::Topic<T>(publisher_.participant(), topic_name); 
    }

    //construct a DDS writer
    writer_ = dds::pub::DataWriter<T>(publisher_, topic);

}

template <class T, class S>         
void DDS_TX_Interface<T,S>::push_message(S* s){
    T t = convert_message(s);
    writer_.write(t);
}
#endif //DDSTXINTERFACE