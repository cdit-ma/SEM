#ifndef BASEMESSAGE_H
#define BASEMESSAGE_H

static int counter_ = 0;

class BaseMessage{

public:
    BaseMessage();
    int get_base_message_id() const;
    int get_base_message_type() const;

private:
    int type_ = 0;
    int id_;
};

#endif //BASEMESSAGE_H