#include "taohelper.h"
#include <iostream>
#include <tao/IORTable/IORTable.h>


tao::TaoHelper& tao::TaoHelper::get_tao_helper(){
    static tao::TaoHelper t;
    return t;
};

tao::TaoHelper::~TaoHelper(){
    registered_poas_.clear();
    
    for(auto& pair : orb_lookup_){
        try{
            //Interupt the thread running the orb, it'll handle destruction
            pair.second->shutdown(true);
        }catch(const CORBA::Exception& e){
            std::cerr << "Corba Exception:" << e._name() << e._info() << std::endl;
        }
    }

    orb_run_futures_.clear();
    orb_lookup_.clear();
}

void tao::TaoHelper::OrbThread(ThreadManager& thread_manager, CORBA::ORB_ptr orb){
    const auto orb_id = orb->id();
    try{
        thread_manager.Thread_Configured();
        thread_manager.Thread_Activated();
        orb->run();
        orb->destroy();
        CORBA::release(orb);
        orb = CORBA::ORB::_nil();
    }catch(const CORBA::Exception& e){
        std::cerr << "tao::TaoHelper::get_orb(): Background Thread '" << orb_id << "' - Corba Exception: " << e._name() << " - " << e._info() << std::endl;
    }
    thread_manager.Thread_Terminated();
}

CORBA::ORB_ptr tao::TaoHelper::get_orb(const std::string& orb_endpoint, bool debug_mode){
    std::unique_lock<std::mutex> lock(orb_mutex_);
    if(orb_lookup_.count(orb_endpoint)){
        return orb_lookup_[orb_endpoint];
    }else{
        CORBA::ORB_ptr orb = 0;
        try{
            auto endpoint = strdup(orb_endpoint.c_str());
            auto orb_id = strdup(orb_endpoint.c_str());
            auto debug_str = strdup(debug_mode ? "10" : "0");
            int argc = 8;
            char* argv[8] = {"-ORBEndpoint", endpoint, "-ORBDebugLevel", debug_str, "-ORBId", orb_id, "-ORBCollocation", "no"};
            orb = CORBA::ORB_init (argc, argv);
            
            delete[] endpoint;
            delete[] debug_str;
        }catch(const CORBA::Exception& e){
            std::cerr << "CORBA Exception: " << e._name() << "orb_endpoint: " << orb_endpoint << std::endl;
            orb = 0;
        }

        if(!CORBA::is_nil(orb)){
            //std::cerr << "CONSTRUCTED ORB: " << orb << ": orb_endpoint: " << orb_endpoint << std::endl;
            //Start an async task to run the ORB, and attach it into a thread manager object
            auto thread_manager = std::unique_ptr<ThreadManager>(new ThreadManager());
            thread_manager->SetFuture(std::async(std::launch::async, OrbThread, std::ref(*thread_manager), orb));


            //Wait for activated
            auto success = thread_manager->WaitForActivated();

            if(success){
                 //Now that the orb is running, we should try resolve a reference, If we can resolve, the orb should be fine
                try{
                    registered_poas_.insert(get_root_poa(orb));
                }catch(const CORBA::Exception& e){
                    success = false;
                    //If we get an exception, destroy the orb
                    std::cerr << "tao::TaoHelper::get_orb(): '" << orb_endpoint << "' - Corba Exception: " << e._name() <<  e._info() << std::endl;
                }
            }

            if(success){
                orb_lookup_[orb_endpoint] = orb;
                orb_run_futures_[orb_endpoint] = std::move(thread_manager);
                //std::cout << "tao::TaoHelper::get_orb(): Constructed Orb: '" << orb_endpoint << "'" << std::endl;
            }else{
                std::cerr << "tao::TaoHelper::get_orb(): Failed to construct Orb: '" << orb_endpoint << "'" << std::endl;
                //Destroy the Orb, which should interupt our async task
                orb->shutdown(true);
                orb = 0;
            }
        }
        return orb;
    }
}

PortableServer::POA_var tao::TaoHelper::get_root_poa(CORBA::ORB_ptr orb){
    CORBA::Object_var poa_object = orb->resolve_initial_references("RootPOA");
    return PortableServer::POA::_narrow(poa_object.in());

}

PortableServer::POA_var tao::TaoHelper::get_poa(CORBA::ORB_ptr orb, const std::string& poa_name){
    if(orb){
        auto root_poa = get_root_poa(orb);
        
        PortableServer::POA_ptr poa = 0;
        try{
            poa = root_poa->find_POA(poa_name.c_str(), true);
            //std::cerr << "FOUND POA: " << poa_name << std::endl;
        }catch(const PortableServer::POA::AdapterNonExistent& e){
            poa = 0;
        }

        if(!poa){
            // Construct the policy list for the LoggingServerPOA.
            CORBA::PolicyList policies (3);
            policies.length (3);
            policies[0] = root_poa->create_id_assignment_policy (PortableServer::USER_ID);
            policies[1] = root_poa->create_lifespan_policy (PortableServer::PERSISTENT);
            policies[2] = root_poa->create_thread_policy (PortableServer::ORB_CTRL_MODEL);
            
            // Get the POA manager object
            auto poa_manager = root_poa->the_POAManager();
            poa_manager->activate();
            
            //std::cerr << "Creating POA: " << poa_name << std::endl;
            // Create the child POA for the test logger factory servants.
            poa = root_poa->create_POA(poa_name.c_str(), poa_manager, policies);

            // Destroy the POA policies
            for (::CORBA::ULong i = 0; i < policies.length (); ++ i){
                policies[i]->destroy ();
            }
            registered_poas_.insert(poa);
        }
        return poa;
    }else{
        std::cout << "No ORB" << std::endl;
        return nullptr;
    }
}

bool tao::TaoHelper::register_servant(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa, PortableServer::Servant servant, const std::string& object_name){
    if(orb && poa){

        
        PortableServer::ObjectId_var obj_id = PortableServer::string_to_ObjectId(object_name.c_str());
        
        //Activate the object with the obj_id
        poa->activate_object_with_id(obj_id, servant);

        //Get the reference to the obj, using the obj_id
        auto obj_ref = poa->id_to_reference(obj_id);
        //obj_ref->_remove_ref();

        //Get the IOR from the object
        CORBA::String_var ior = orb->object_to_string(obj_ref);

        auto ior_table = GetIORTable(orb);
       
        if(ior_table){
            ior_table->rebind(object_name.c_str(), ior);
            return true;
        }
    }
    return false;
}

IORTable::Table_var tao::TaoHelper::GetIORTable(CORBA::ORB_ptr orb){
    CORBA::Object_var table_object = orb->resolve_initial_references ("IORTable");
    return IORTable::Table::_narrow(table_object.in());
}

bool tao::TaoHelper::deregister_servant(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa, PortableServer::Servant servant, const std::string& object_name){
    if(orb && poa){
        CORBA::OctetSeq_var obj_id = PortableServer::string_to_ObjectId (object_name.c_str());
        poa->deactivate_object(obj_id);

        //Remove from the maps
        registered_poas_.erase(poa);
        poa->destroy(1,1);
        

        auto ior_table = GetIORTable(orb);
        if(ior_table){
            ior_table->unbind(object_name.c_str());
            return true;
        }
    }
    return false;
}

void tao::TaoHelper::register_initial_reference(CORBA::ORB_ptr orb, const std::string& obj_id, const std::string& corba_str){
    if(orb){
        try{
            auto object = orb->string_to_object(corba_str.c_str());
            //orb->unregister_initial_reference(obj_id.c_str());
            orb->register_initial_reference(obj_id.c_str(), object);
        }catch(const CORBA::Exception& e){
            std::cerr << "Corba Exception:" << e._name() << e._info() << std::endl;
        }
    }
}

void tao::TaoHelper::deregister_initial_reference(CORBA::ORB_ptr orb, const std::string& obj_id){
    if(orb){
        try{
            orb->unregister_initial_reference(obj_id.c_str());
        }catch(const CORBA::Exception& e){
            std::cerr << "Corba Exception:" << e._name() << e._info() << std::endl;
        }
    }
}

CORBA::Object_ptr tao::TaoHelper::resolve_initial_references(CORBA::ORB_ptr orb, const std::string& obj_id){
    if(orb){
        return orb->resolve_initial_references(obj_id.c_str());
    }
    return 0;
}
