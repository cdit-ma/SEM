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

    // Create a USER_ID IdAssignmentpolicy object.
    PortableServer::IdAssignmentPolicy_var idassignment =
      root_poa->create_id_assignment_policy (PortableServer::USER_ID);

    // Create a PERSISTENT LifespanPolicy object.
    PortableServer::LifespanPolicy_var lifespan =
      root_poa->create_lifespan_policy (PortableServer::PERSISTENT);

    // Policies for the childPOA to be created.
    CORBA::PolicyList policies;
    policies.length (2);

    policies[0] =
      PortableServer::IdAssignmentPolicy::_duplicate (idassignment.in ());

    policies[1] =
      PortableServer::LifespanPolicy::_duplicate (lifespan.in ());

    // Create the childPOA under the RootPOA.
    PortableServer::POA_var child_poa =
      root_poa->create_POA ("Test",
                            poa_manager.in (),
                            policies);

    // Destroy policy objects.
    idassignment->destroy ();
    lifespan->destroy ();

        
    Hello *hello_impl = 0;
    ACE_NEW_RETURN (hello_impl, Hello (orb.in ()), 1);
    

    // Get the Object ID.
    PortableServer::ObjectId_var oid =
      PortableServer::string_to_ObjectId ("HelloSender");


    // Activate the Stock_Factory object.
    child_poa->activate_object_with_id (oid.in (),
                                        hello_impl);

    // Get the object reference.
    CORBA::Object_var stock_factory = child_poa->id_to_reference (oid.in ());

    // Stringify all the object referencs.
    CORBA::String_var ior = orb->object_to_string (stock_factory.in ());
    // Print them out !
    std::cout << ior.in () << std::endl;

    orb-> run ();

    // Destroy POA, waiting until the destruction terminates.
    root_poa->destroy (1, 1);
    orb->destroy ();
}