#ifndef PORTS_MESSAGE_H
#define PORTS_MESSAGE_H

#include "core/basemessage.h"
#include <string>

class Message : public BaseMessage{            

public:
    Message();
    void set_instName(const std::string val);
    void set_content(const std::string val);
    void set_time(const int val);

    std::string content();
    std::string instName();
    int time();

private:
    long int time_;
    std::string instName_;
    std::string content_;
};

#endif //PORTS_MESSAGE_H