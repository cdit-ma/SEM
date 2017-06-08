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
    // Initialze the ORB.
    CORBA::ORB_var orb = CORBA::ORB_init (argc, argv);

    // Get a reference to the RootPOA.
    CORBA::Object_var obj = orb->resolve_initial_references ("RootPOA");

    // Get the POA_var object from Object_var.
    PortableServer::POA_var root_poa =
      PortableServer::POA::_narrow (obj.in ());

    // Get the POAManager of the RootPOA.
    PortableServer::POAManager_var poa_manager =
      root_poa->the_POAManager ();


    poa_manager->activate ();

    /*
     // Construct the policy list for the LoggingServerPOA.
    CORBA::PolicyList policies (6);
    policies.length (6);
    policies[0] = root_poa->create_thread_policy (PortableServer::ORB_CTRL_MODEL);
    policies[1] = root_poa->create_servant_retention_policy (PortableServer::RETAIN);
    policies[2] = root_poa->create_id_assignment_policy (PortableServer::SYSTEM_ID);
    policies[3] = root_poa->create_id_uniqueness_policy (PortableServer::UNIQUE_ID);
    policies[4] = root_poa->create_lifespan_policy (PortableServer::PERSISTENT);
    policies[5] = root_poa->create_request_processing_policy (PortableServer::USE_ACTIVE_OBJECT_MAP_ONLY);

    USER_ID

    // Create the child POA for the test logger factory servants.
    ::PortableServer::POA_var child_poa =
      root_poa->create_POA ("LoggingServerPOA",
                                   ::PortableServer::POAManager::_nil (),
                                   policies);

     // Destroy the POA policies
    for (::CORBA::ULong i = 0; i < policies.length (); ++ i)
      policies[i]->destroy ();
      */

      
     // Construct the policy list for the LoggingServerPOA.
    CORBA::PolicyList policies (2);
    policies.length (2);
    policies[0] = root_poa->create_id_assignment_policy (PortableServer::USER_ID);
    policies[1] = root_poa->create_lifespan_policy (PortableServer::PERSISTENT);

    // Create the child POA for the test logger factory servants.
    ::PortableServer::POA_var child_poa =
      root_poa->create_POA ("LoggingServerPOA",
                                   poa_manager.in (), /*::PortableServer::POAManager::_nil (),*/
                                   policies);

     // Destroy the POA policies
    for (::CORBA::ULong i = 0; i < policies.length (); ++ i)
      policies[i]->destroy ();
      

    auto mgr = child_poa->the_POAManager ();
    mgr->activate ();
        
    Hello *hello_impl = 0;
    ACE_NEW_RETURN (hello_impl, Hello (orb.in ()), 1);

    child_poa->activate_object(hello_impl);


    // Get the object reference.
    //CORBA::Object_var stock_factory = child_poa->id_to_reference (oid.in ());

    // Stringify all the object referencs.
    //CORBA::String_var ior = orb->object_to_string (stock_factory.in ());
    // Print them out !
    std::cout << "RUNNING ORB:" << std::endl;

    orb-> run ();

    // Destroy POA, waiting until the destruction terminates.
    root_poa->destroy (1, 1);
    orb->destroy ();
}