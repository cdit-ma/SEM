#ifndef TAO_HELPER_H
#define TAO_HELPER_H

#include <string>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <future>
#include <vector>
#include <list>

#include <tao/TimeBaseC.h>
#include <tao/ORB.h>
#include <tao/PortableServer/PortableServer.h>
#include <tao/IORTable/IORTable.h>
#include <tao/Messaging/Messaging.h>
#include <orbsvcs/CosNamingC.h>
#include <core/threadmanager.h>



namespace tao{
    class TaoHelper{
    public:
        static TaoHelper& get_tao_helper();
    protected:
        ~TaoHelper();
    public:
        CORBA::ORB_ptr get_orb(const std::string& orb_endpoint, bool debug_mode = false);
        PortableServer::POA_var get_poa(CORBA::ORB_ptr orb, const std::string& poa_name);
        void register_initial_reference(CORBA::ORB_ptr orb, const std::string& obj_id, const std::string& corba_str);
        void deregister_initial_reference(CORBA::ORB_ptr orb, const std::string& obj_id);
        CORBA::Object_ptr resolve_initial_references(CORBA::ORB_ptr orb, const std::string& obj_id);

        CORBA::Object_ptr resolve_reference_via_namingservice(CORBA::ORB_ptr orb, const std::string& naming_service_name, const std::vector<std::string>& object_name, const std::string& server_kind);
        void register_servant_via_namingservice(CORBA::ORB_ptr orb, const std::string& naming_service_name, PortableServer::POA_ptr poa, PortableServer::Servant servant, const std::vector<std::string>& object_name, const std::string& server_kind);
        void deregister_servant_via_namingservice(CORBA::ORB_ptr orb, const std::string& naming_service_name, PortableServer::POA_ptr poa, PortableServer::Servant servant, const std::vector<std::string>& object_name, const std::string& server_kind);

        bool register_servant(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa, PortableServer::Servant servant, const std::string& object_name);
        bool deregister_servant(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa, PortableServer::Servant servant, const std::string& object_name);

        std::string GetPOAName(const std::vector<std::string>& object_name);
    private:
        CosNaming::Name GetCosName(const std::vector<std::string>& object_name, const std::string& server_kind);
        static void OrbThread(ThreadManager& thread_manager, CORBA::ORB_ptr orb);
        IORTable::Table_var GetIORTable(CORBA::ORB_ptr orb);
        CosNaming::NamingContext_ptr GetNamingContext(CORBA::ORB_ptr orb, const std::string& naming_service_name);
        PortableServer::POA_var get_root_poa(CORBA::ORB_ptr orb);

        
        std::mutex orb_mutex_;
        
        std::unordered_map<std::string, CORBA::ORB_ptr> orb_lookup_;
        std::unordered_map<std::string, std::unique_ptr<ThreadManager> > orb_run_futures_;
        std::unordered_set<PortableServer::POA_ptr> registered_poas_;
    };

    template<class TaoType> 
    TaoType &GetReference(TaoType *object) { 
        return *object; 
    };

    template<class TaoType> 
    TaoType &GetReference(TaoType &object) { 
        return object; 
    };

    template<typename TaoType>
    void DeleteTaoObject(TaoType *object) {
        delete object;
    };
    template<typename TaoType>
    void DeleteTaoObject(TaoType &object) {
        // do nothing
    };
};


#endif //RTI_DDSHELPER_H
