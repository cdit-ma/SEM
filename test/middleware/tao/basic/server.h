#include <iostream>
#include "messageS.h"


/// Implement the Test::Hello interface
class Hello: public virtual POA_Test::Hello
{
    public:
        void send(const Test::Message& message);
        void send22(const Test::Message& message);
};