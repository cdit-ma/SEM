#include "helper.h"
#include <iostream>

#include <tao/IORTable/IORTable.h>

tao::TaoHelper* tao::TaoHelper::singleton_ = 0;
std::mutex tao::TaoHelper::global_mutex_;

tao::TaoHelper* tao::TaoHelper::get_tao_helper(){
    std::lock_guard<std::mutex> lock(global_mutex_);

    if(singleton_ == 0){
        std::cout << "Constructing TaoHelper" << std::endl;
        singleton_ = new TaoHelper();
    }
    return singleton_;
};

CORBA::ORB_ptr tao::TaoHelper::get_orb(const std::string& orb_endpoint){
    if(orb_lookup_.count(orb_endpoint)){
        std::cout << "Got Orb: " << orb_endpoint << std::endl;
        return orb_lookup_[orb_endpoint];
    }else{
        std::cout << "Constructing Orb: " << orb_endpoint << std::endl;
        int argc = 3;
        auto endpoint = strdup(orb_endpoint.c_str());
        char* argv[3] = {"", "-ORBEndpoint", endpoint};
        auto orb = CORBA::ORB_init(argc, argv);
        orb_lookup_[orb_endpoint] = orb;
        return orb;
    }
}

PortableServer::POA_ptr tao::TaoHelper::get_poa(CORBA::ORB_ptr orb, const std::string& poa_name){
    if(orb){
        auto obj = orb->resolve_initial_references("RootPOA");
        auto root_poa = ::PortableServer::POA::_narrow(obj);
        
        PortableServer::POA_ptr poa = 0;

        try{
            poa = root_poa->find_POA(poa_name.c_str(), true);
        }catch(const PortableServer::POA::AdapterNonExistent& e){
            poa = 0;
        }

        if(poa){
            std::cout << "Found POA: " << poa_name << std::endl;
        }else{
            std::cout << "Constructing POA: " << poa_name << std::endl;
            // Construct the policy list for the LoggingServerPOA.
            CORBA::PolicyList policies (2);
            policies.length (2);
            policies[0] = root_poa->create_id_assignment_policy (PortableServer::USER_ID);
            policies[1] = root_poa->create_lifespan_policy (PortableServer::PERSISTENT);
            
            // Create the child POA for the test logger factory servants.
            poa = root_poa->create_POA(poa_name.c_str(), nullptr, policies);
            //poa->the_POAManager()->activate();

            // Destroy the POA policies
            for (::CORBA::ULong i = 0; i < policies.length (); ++ i){
                policies[i]->destroy ();
            }
        }
        return poa;
    }else{
        std::cout << "No ORB" << std::endl;
        return nullptr;
    }
}

bool tao::TaoHelper::register_servant(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa, PortableServer::Servant servant, const std::string& object_name){
    if(orb && poa){
        CORBA::OctetSeq_var obj_id = PortableServer::string_to_ObjectId (object_name.c_str());
        //Activate the object with the obj_id
        poa->activate_object_with_id(obj_id, servant);
        //Get the reference to the obj, using the obj_id
        auto obj_ref = poa->id_to_reference(obj_id);
        //Get the IOR from the object
        auto ior = orb->object_to_string (obj_ref);

        auto ior_ref = orb->resolve_initial_references("IORTable");
        //Cast into concrete class
        auto ior_table = IORTable::Table::_narrow(ior_ref);
        if(ior_table){
            std::cout << "Binding: " << object_name << std::endl;
            ior_table->rebind(object_name.c_str(), ior);
            return true;
        }
    }
    return false;
}

void tao::TaoHelper::register_initial_reference(CORBA::ORB_ptr orb, const std::string& obj_id, const std::string& corba_str){

}