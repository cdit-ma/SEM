#include "taohelper.h"
#include <iostream>
#include <tao/IORTable/IORTable.h>
#include <tao/PortableServer/Servant_Base.h>

namespace tao {
    const std::string orb_endpoint_flag = "-ORBEndpoint";
    const std::string orb_debug_flag = "-ORBDebugLevel";
    const std::string orb_id_flag = "-ORBId";
    const std::string orb_collocation_flag = "-ORBCollocation";
    const std::string orb_no = "no";
}

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
            // Have to strdup out to fresh allocs of constant strings to
            //   dodge any potential UB if CORBA::ORB_init ever modifies strs
            auto endpoint_flag = strdup(tao::orb_endpoint_flag.c_str());
            auto endpoint_str = strdup(orb_endpoint.c_str());

            auto debug_flag = strdup(tao::orb_debug_flag.c_str());
            auto debug_str = strdup(debug_mode ? "10" : "0");

            auto id_flag = strdup(tao::orb_id_flag.c_str());
            auto id_str = strdup(orb_endpoint.c_str());

            auto collocation_flag = strdup(tao::orb_collocation_flag.c_str());
            auto collocation_str = strdup(tao::orb_no.c_str());

            int argc = 8;

            char* argv[8] = {endpoint_flag,
                             endpoint_str,
                             debug_flag,
                             debug_str,
                             id_flag,
                             id_str,
                             collocation_flag,
                             collocation_str};

            orb = CORBA::ORB_init (argc, argv);

            // Free allocations
            for(size_t i = 0; i < argc; ++i) {
                delete[] argv[i];
            }

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

CosNaming::NamingContext_ptr tao::TaoHelper::GetNamingContext(CORBA::ORB_ptr orb, const std::string& naming_service_name){
    try{
        auto naming_object = orb->resolve_initial_references(naming_service_name.c_str());
        return CosNaming::NamingContext::_narrow(naming_object);
    }catch(const CORBA::Exception& e){
        throw std::runtime_error("Cannot Resolve Naming Context: " + naming_service_name);
    }
}

CosNaming::Name tao::TaoHelper::GetCosName(const std::vector<std::string>& object_name, const std::string& server_kind){
    auto size = object_name.size();
    if(size == 0){
        throw std::runtime_error("Cannot get name for zero length vector");
    }

    CosNaming::Name name(size);
    name.length(size);
    
    for(int i = 0; i < size ; i++){
        name[i].id = object_name[i].c_str();
    }
    name[size - 1].kind = server_kind.c_str();

    return name;
}

bool tao::TaoHelper::deregister_servant(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa, PortableServer::Servant servant, const std::string& object_name){
    if(orb && poa){
        CORBA::OctetSeq_var obj_id = PortableServer::string_to_ObjectId (object_name.c_str());
        //Get the reference to the obj, using the obj_id
        auto obj_ref = poa->id_to_reference(obj_id);

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
            orb->register_initial_reference(obj_id.c_str(), object);
        }catch(const CORBA::Exception& e){
            std::cerr << e._info() << std::endl;
            throw std::runtime_error("Cannot register_initial_reference: " + obj_id + " = " + corba_str);
        }
    }
}

std::string tao::TaoHelper::GetPOAName(const std::vector<std::string>& object_name){
    std::string poa_name;

    for(int i = 0; i < object_name.size(); i++){
        poa_name += object_name[i];
        if(i != object_name.size() - 1){
            poa_name += "/";
        }
    }
    return poa_name;
}

void tao::TaoHelper::deregister_initial_reference(CORBA::ORB_ptr orb, const std::string& obj_id){
    if(orb){
        try{
            orb->unregister_initial_reference(obj_id.c_str());
        }catch(const CORBA::Exception& e){
            throw std::runtime_error("Cannot unregister_initial_reference: " + obj_id);
        }
    }
}


CORBA::Object_ptr tao::TaoHelper::resolve_initial_references(CORBA::ORB_ptr orb, const std::string& obj_id){
    if(orb){
        return orb->resolve_initial_references(obj_id.c_str());
    }
    return 0;
}

CORBA::Object_ptr tao::TaoHelper::resolve_reference_via_namingservice(CORBA::ORB_ptr orb, const std::string& naming_service_name, const std::vector<std::string>& object_name, const std::string& server_kind){
    if(orb){
        auto name_context = GetNamingContext(orb, naming_service_name);
        if(name_context){
            auto cos_name = GetCosName(object_name, server_kind);
            return name_context->resolve(cos_name);
        }
    }
    return 0;
}


void tao::TaoHelper::register_servant_via_namingservice(CORBA::ORB_ptr orb, const std::string& naming_service_name, PortableServer::POA_ptr poa, PortableServer::Servant servant, const std::vector<std::string>& object_name, const std::string& server_kind){
    if(orb && poa){
        const auto& object_str = object_name.back();
        PortableServer::ObjectId_var obj_id = PortableServer::string_to_ObjectId(object_str.c_str());
        
        //Activate the object with the obj_id
        poa->activate_object_with_id(obj_id, servant);

        //Get the reference to the obj, using the obj_id
        auto obj_ref = poa->id_to_reference(obj_id);

        auto top_context = GetNamingContext(orb, naming_service_name);
        if(top_context){
            auto name_context = top_context;

            //Construct the contexts if they don't exist
            for(int i = 0; i < object_name.size() - 1; i++){
                auto context = name_context->new_context();
                auto cos_name = GetCosName({object_name[i]}, "");
                name_context->rebind_context(cos_name, context);
                name_context = context;
            }

            //Rebind the Object
            auto cos_name = GetCosName(object_name, server_kind);
            top_context->rebind(cos_name, obj_ref);
        }
    }
}
void tao::TaoHelper::deregister_servant_via_namingservice(CORBA::ORB_ptr orb, const std::string& naming_service_name, PortableServer::POA_ptr poa, PortableServer::Servant servant, const std::vector<std::string>& object_name, const std::string& server_kind){
    if(orb && poa){
        const auto& object_str = object_name.back();

        

        PortableServer::ObjectId_var obj_id = PortableServer::string_to_ObjectId (object_str.c_str());
        //Get the reference to the obj, using the obj_id
        auto obj_ref = poa->id_to_reference(obj_id);

        poa->deactivate_object(obj_id);
        
        //Remove from the maps
        registered_poas_.erase(poa);
        poa->destroy(1,1);
        
        auto name_context = GetNamingContext(orb, naming_service_name);

        /*if(name_context){
            auto addr = resolve_reference_via_namingservice(orb, naming_service_name, object_name);
            if(addr->_is_equivalent(obj_ref)){
                auto cos_name = GetCosName(object_name);
                //name_context->unbind(cos_name);
            }
        }*/
    }
}
