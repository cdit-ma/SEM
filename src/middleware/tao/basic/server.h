#include <iostream>
#include "messageS.h"


/// Implement the Test::Hello interface
class Hello: public virtual POA_Test::Hello
{
public:
    /// Constructor
    Hello (CORBA::ORB_ptr orb, std::string id);
    void sendLelbs(const Test::Message& message);
private:
    /// Use an ORB reference to shutdown the application.
    CORBA::ORB_var orb_;
    std::string id;
};