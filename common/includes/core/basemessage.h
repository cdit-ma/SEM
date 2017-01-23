#ifndef BASEMESSAGE_H
#define BASEMESSAGE_H

static int counter_ = 0;

class BaseMessage{

public:
    BaseMessage(int type);
    int get_base_message_id() const;
    int get_base_message_type() const;

private:
    int type_;
    int id_;
};

#endif //BASEMESSAGE_H