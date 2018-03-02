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


CORBA::ORB_ptr tao::TaoHelper::get_orb(const std::string& orb_endpoint, bool debug_mode){
    if(orb_lookup_.count(orb_endpoint)){
        std::cout << "Got Orb: " << orb_endpoint << std::endl;
        return orb_lookup_[orb_endpoint];
    }else{
        std::cout << "Constructing Orb: " << orb_endpoint << std::endl;
        
        CORBA::ORB_ptr orb = 0;
        
        std::mutex mutex_;
        std::unique_lock<std::mutex> lock(mutex_);
        std::condition_variable lock_condition;

        auto orb_future = std::async(std::launch::async, [&orb, &orb_endpoint, &lock_condition, &debug_mode](){ 
            bool setup = false;
            try{
                // "-ORBSvcConfDirective", "static Server_Strategy_Factory '-ORBConcurrency thread-per-connection'"
                auto endpoint = strdup(orb_endpoint.c_str());
                auto debug_str =  strdup(debug_mode ? "10" : "0");
                int argc = 4;

                char* argv[4] = {"-ORBEndpoint", endpoint, "-ORBDebugLevel", debug_str};
                orb = CORBA::ORB_init(argc, argv);

                //Free my vars
                delete[] endpoint;
                delete[] debug_str;
                
                if(orb){
                    //Run for a short period of time to check if things can be run
                    ACE_Time_Value tv(0, 100);
                    orb->run(tv);
                    
                    lock_condition.notify_all();
                    orb->run();
                    std::cout << "Dead Orb" << std::endl;
                }
            }catch(...){
                std::cerr << "TaoHelper: ERROR: " << std::endl;
                if(!setup){
                    lock_condition.notify_all();
                }
            }
        });

        lock_condition.wait(lock);

        if(orb){
            orb_lookup_[orb_endpoint] = orb;
            orb_run_futures_[orb_endpoint] = std::move(orb_future);
        }
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
            std::cout << "FOUND POA" << std::endl;
        }catch(const PortableServer::POA::AdapterNonExistent& e){
            poa = 0;
        }

        if(poa){
            std::cout << "Found POA: " << poa_name << std::endl;
        }else{
            std::cout << "Constructing POA: " << poa_name << std::endl;
            // Construct the policy list for the LoggingServerPOA.
            CORBA::PolicyList policies (3);
            policies.length (3);
            policies[0] = root_poa->create_id_assignment_policy (PortableServer::USER_ID);
            policies[1] = root_poa->create_lifespan_policy (PortableServer::PERSISTENT);
            policies[2] = root_poa->create_thread_policy (PortableServer::ORB_CTRL_MODEL);

            // Get the POA manager object
            PortableServer::POAManager_var manager = root_poa->the_POAManager();
            
            
            // Create the child POA for the test logger factory servants.
            poa = root_poa->create_POA(poa_name.c_str(), manager, policies);
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
    if(orb){
        try{
            auto object = orb->string_to_object(corba_str.c_str());
            orb->register_initial_reference(obj_id.c_str(), object);
        }catch(...){

        }
    }
}

CORBA::Object_ptr tao::TaoHelper::resolve_initial_references(CORBA::ORB_ptr orb, const std::string& obj_id){
    if(orb){
        return orb->resolve_initial_references(obj_id.c_str());
    }
    return 0;
}
