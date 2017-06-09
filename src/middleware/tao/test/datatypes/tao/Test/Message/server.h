#include <iostream>
#include "messageS.h"


/// Implement the Test::Hello interface
class Hello: public virtual POA_Test::Hello
{
public:
    /// Constructor
    Hello (CORBA::ORB_ptr orb);
    void send(const Test::Message& message);
private:
    /// Use an ORB reference to shutdown the application.
    CORBA::ORB_var orb_;
};