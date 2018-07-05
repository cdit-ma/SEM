#ifndef TAO_HELPER_H
#define TAO_HELPER_H

#include <string>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <future>

#include <tao/ORB.h>
#include <tao/PortableServer/PortableServer.h>
#include <tao/IORTable/IORTable.h>
#include <tao/Messaging/Messaging.h>
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
        CORBA::Object_ptr resolve_initial_references(CORBA::ORB_ptr orb, const std::string& obj_id);

        bool register_servant(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa, PortableServer::Servant servant, const std::string& object_name);
        bool deregister_servant(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa, PortableServer::Servant servant, const std::string& object_name);
    private:
        static void OrbThread(ThreadManager& thread_manager, CORBA::ORB_ptr orb);
        IORTable::Table_var GetIORTable(CORBA::ORB_ptr orb);
        PortableServer::POA_var get_root_poa(CORBA::ORB_ptr orb);

        
        std::mutex orb_mutex_;
        
        std::unordered_map<std::string, CORBA::ORB_ptr> orb_lookup_;
        std::unordered_map<std::string, std::unique_ptr<ThreadManager> > orb_run_futures_;
        std::unordered_set<PortableServer::POA_ptr> registered_poas_;
    };
};


#endif //RTI_DDSHELPER_H
