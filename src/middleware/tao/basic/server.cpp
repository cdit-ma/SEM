#include <iostream>
#include "server.h"


Hello::Hello (CORBA::ORB_ptr orb)
: orb_ (CORBA::ORB::_duplicate (orb))
{
}


void Hello::send(const Test::Message& message){
    std::cout << "\ttime: " << message.time << std::endl;
    std::cout << "\tinst_name: " << message.inst_name << std::endl;
}


int main(int argc, char ** argv){
    ::CORBA::Object_var obj = this->orb_->resolve_initial_references ("RootPOA");
    auto root_poa_ = ::PortableServer::POA::_narrow (obj.in ());

    // Activate the RootPOA's manager.
    ::PortableServer::POAManager_var mgr = this->root_poa_->the_POAManager ();
    mgr->activate ();

    // Construct the policy list for the LoggingServerPOA.
    CORBA::PolicyList policies (6);
    policies.length (6);

    policies[0] = root_poa_->create_thread_policy (PortableServer::ORB_CTRL_MODEL);
    policies[1] = root_poa_->create_servant_retention_policy (PortableServer::RETAIN);
    policies[2] = root_poa_->create_id_assignment_policy (PortableServer::SYSTEM_ID);
    policies[3] = root_poa_->create_id_uniqueness_policy (PortableServer::UNIQUE_ID);
    policies[4] = root_poa_->create_lifespan_policy (PortableServer::TRANSIENT);
    policies[5] = root_poa_->create_request_processing_policy (PortableServer::USE_ACTIVE_OBJECT_MAP_ONLY);

    // Create the child POA for the test logger factory servants.
    ::PortableServer::POA_var child_poa =
      this->root_poa_->create_POA ("LoggingServerPOA",
                                   ::PortableServer::POAManager::_nil (),
                                   policies);

    // Destroy the POA policies
    for (::CORBA::ULong i = 0; i < policies.length (); ++ i)
      policies[i]->destroy ();

    mgr = child_poa->the_POAManager ();
    mgr->activate ();

    // Activate the servant.
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("%T (%t) - %M - activating logger server servant\n")));

    Hello *hello_impl = 0;
    ACE_NEW_RETURN (hello_impl, Hello (child_poa.in ()), 1);

    child_poa->run ();

    ACE_DEBUG ((LM_DEBUG, "(%P|%t) server - event loop finished\n"));
    root_poa->destroy (1, 1);
    child_poa->destroy ();

    return 0;


}