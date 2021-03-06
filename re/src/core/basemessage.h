#ifndef BASEMESSAGE_H
#define BASEMESSAGE_H

// REVIEW (Mitch): Context: This class is only ever used as a parent class for code generated
//  classes
class BaseMessage{
    public:
        BaseMessage();
        ~BaseMessage() = default;

        //Copy
        BaseMessage(const BaseMessage& other);
        BaseMessage operator=(const BaseMessage& other);
        
        //Move
        BaseMessage(BaseMessage&& other);
        BaseMessage& operator=(BaseMessage&& other);

        int get_base_message_id() const;
    private:
        int id_ = 0;
};

#endif //BASEMESSAGE_H