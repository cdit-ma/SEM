#include <iostream>
#include "server.h"
#include "tao/IORTable/IORTable.h"

Hello::Hello (CORBA::ORB_ptr orb)
: orb_ (CORBA::ORB::_duplicate (orb))
{
}


void Hello::send(const Test::Message& message){
    std::cout << "\ttime: " << message.time << std::endl;
    std::cout << "\tinst_name: " << message.inst_name << std::endl;
}

//TODO, look at how we can bind without passing parameters to the ORB!
//Clean Client and server
//Test multithreadedly
int main(int argc, char ** argv){
    //Initialize the orb
    CORBA::ORB_var orb = CORBA::ORB_init (argc, argv);

    // Get a reference to the RootPOA.
    CORBA::Object_var obj = orb->resolve_initial_references ("RootPOA");

    // Get the POA_var object from Object_var.
    PortableServer::POA_var root_poa = PortableServer::POA::_narrow (obj.in());

    // Construct the policy list for the LoggingServerPOA.
    CORBA::PolicyList policies (2);
    policies.length (2);
    policies[0] = root_poa->create_id_assignment_policy (PortableServer::USER_ID);
    policies[1] = root_poa->create_lifespan_policy (PortableServer::PERSISTENT);

     // Get the POAManager of the RootPOA.
    PortableServer::POAManager_var poa_manager = root_poa->the_POAManager();

    // Create the child POA for the test logger factory servants.
    ::PortableServer::POA_var child_poa = root_poa->create_POA ("LoggingServerPOA", poa_manager, policies);

     // Destroy the POA policies
    for (::CORBA::ULong i = 0; i < policies.length (); ++ i){
        policies[i]->destroy ();
    }

    //Construct a sender
    Hello* hello_impl = new Hello(orb);

    /*
    // Activate object WITH OUT ID
    PortableServer::ObjectId_var myObjID = child_poa->activate_object(hello_impl);
    // Get a CORBA reference with the POA through the servant
    CORBA::Object_var o = child_poa->servant_to_reference(hello_impl);
    // The reference is converted to a character string
    CORBA::String_var ior = orb->object_to_string(o);
    */

    //Acitivate WITH ID
    //Convert our string into an object_id
    PortableServer::ObjectId_var obj_id = PortableServer::string_to_ObjectId ("Stock_Factory");
    //Activate the object with the obj_id
    child_poa->activate_object_with_id(obj_id.in(), hello_impl);
    //Get the reference to the obj, using the obj_id
    CORBA::Object_var obj_ref = child_poa->id_to_reference (obj_id.in ());
    //Get the IOR from the object
    CORBA::String_var ior = orb->object_to_string (obj_ref.in ());

    //Register with the IOR Table
    //Get the IORTable for the application
    ::CORBA::Object_var temp = orb->resolve_initial_references ("IORTable");
    //Cast into concrete class
    ::IORTable::Table_var ior_table = IORTable::Table::_narrow (temp.in ());

    if (::CORBA::is_nil (ior_table.in ()))
        ACE_ERROR_RETURN ((LM_ERROR,
                        ACE_TEXT ("%T (%t) - %M - failed to resolve IOR table\n")),
                        1);

    //Bind the IOR file into the IOR table
    ior_table->bind ("LoggingServer", ior.in ());

    //Activate the POA
    poa_manager->activate ();

    //Run the ORB
    orb->run ();

    // Destroy POA, waiting until the destruction terminates.
    root_poa->destroy (1, 1);
    orb->destroy ();
}