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
    CORBA::ORB_var orb = CORBA::ORB_init (argc, argv);
    CORBA::Object_var poa_object = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var root_poa = PortableServer::POA::_narrow (poa_object.in ());
    if (CORBA::is_nil (root_poa.in ())){
        std::cerr << "PANIC" << std::endl;
    }


    PortableServer::POAManager_var poa_manager = root_poa->the_POAManager ();
    
    Hello *hello_impl = 0;
    ACE_NEW_RETURN (hello_impl, Hello (orb.in ()), 1);



    PortableServer::ServantBase_var owner_transfer(hello_impl);
    PortableServer::ObjectId_var id = root_poa->activate_object (hello_impl);
    CORBA::Object_var object = root_poa->id_to_reference (id.in ());
    Test::Hello_var hello = Test::Hello::_narrow (object.in ());
    


    CORBA::String_var ior = orb->object_to_string (hello.in ());
    // Output the IOR to the ior_output_file
    FILE *output_file= ACE_OS::fopen ("server.ior", "w");
    if (output_file == 0)
    ACE_ERROR_RETURN ((LM_ERROR, "Cannot open output file for writing IOR: %s\n", output_file), 1);
    ACE_OS::fprintf (output_file, "%s", ior.in ());
    ACE_OS::fclose (output_file);
    
    poa_manager->activate ();
    orb->run ();

    ACE_DEBUG ((LM_DEBUG, "(%P|%t) server - event loop finished\n"));
    root_poa->destroy (1, 1);
    orb->destroy ();

    return 0;


}