#ifndef TAO_HELPER_H
#define TAO_HELPER_H

#include <string>
#include <mutex>
#include <unordered_map>

#include <tao/ORB.h>
#include <tao/PortableServer/PortableServer.h>


namespace tao{
    class TaoHelper{
    public:
        static TaoHelper* get_tao_helper();
    public:
        CORBA::ORB_ptr get_orb(const std::string& orb_endpoint);
        PortableServer::POA_ptr get_poa(CORBA::ORB_ptr orb, const std::string& poa_name);
        void register_initial_reference(CORBA::ORB_ptr orb, const std::string& obj_id, const std::string& corba_str);
        bool register_servant(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa, PortableServer::Servant servant, const std::string& object_name);
        std::mutex mutex;
    private:
        static TaoHelper* singleton_;
        static std::mutex global_mutex_;
        
        std::unordered_map<std::string, CORBA::ORB_ptr> orb_lookup_;
    };
};


#endif //RTI_DDSHELPER_H