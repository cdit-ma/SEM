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
    auto orb = CORBA::ORB_init (argc, argv);

    //Get the reference to the RootPOA
    auto obj = orb->resolve_initial_references("RootPOA");
    auto root_poa = ::PortableServer::POA::_narrow(obj);

    // Construct the policy list for the LoggingServerPOA.
    CORBA::PolicyList policies (2);
    policies.length (2);
    policies[0] = root_poa->create_id_assignment_policy (PortableServer::USER_ID);
    policies[1] = root_poa->create_lifespan_policy (PortableServer::PERSISTENT);

    // Get the POAManager of the RootPOA.
    PortableServer::POAManager_var poa_manager = root_poa->the_POAManager();

    // Create the child POA for the test logger factory servants.
    auto child_poa = root_poa->create_POA("LoggingServerPOA", poa_manager, policies);

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

    //Activate WITH ID
    //Convert our string into an object_id
    CORBA::OctetSeq_var obj_id = PortableServer::string_to_ObjectId ("LoggingServer2");
    //Activate the object with the obj_id
    child_poa->activate_object_with_id(obj_id, hello_impl);
    //Get the reference to the obj, using the obj_id
    auto obj_ref = child_poa->id_to_reference (obj_id);
    //Get the IOR from the object
    auto ior = orb->object_to_string (obj_ref);

    //Register with the IOR Table
    //Get the IORTable for the application
    auto temp = orb->resolve_initial_references ("IORTable");
    //Cast into concrete class
    auto ior_table = IORTable::Table::_narrow (temp);

    if(!ior_table){
        std::cerr << "Failed to resolve IOR Table" << std::endl;
    }

    //Bind the IOR file into the IOR table
    ior_table->bind("LoggingServer2", ior);

    //Activate the POA
    poa_manager->activate();

    //Run the ORB
    orb->run();

    orb->destroy();
    return 0;
}