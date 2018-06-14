#include <iostream>
#include "message_serverS.h"


/// Implement the Test::Hello interface
class Hello: public virtual POA_Test::Hello
{
    protected:
        ~Hello(){};
    public:
        void send(const Test::Message2& message);
        void send22(const Test::Message& message);
};