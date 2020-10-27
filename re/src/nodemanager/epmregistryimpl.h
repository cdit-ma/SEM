//
// Created by mitchell on 22/10/20.
//

#ifndef SEM_EPMREGISTRYIMPL_H
#define SEM_EPMREGISTRYIMPL_H
#include "epmregistry.h"
#include <mutex>

namespace sem::node_manager {
class EpmRegistryImpl : public EpmRegistry {
public:
    auto start_epm() -> void final;
    auto remove_epm(sem::types::Uuid uuid) -> void final;
    auto get_epm_info(sem::types::Uuid uuid) -> EpmInfo final;

    static constexpr std::chrono::seconds epm_start_duration_max{5};
private:
    std::mutex mutex_;


#if 0


    std::mutex epm_registration_mutex_;
    std::condition_variable epm_registration_semaphore_;

    struct EpmRegistration {
        sem::types::Uuid request_uuid;
        sem::types::Uuid epm_uuid;
    };

    std::queue<EpmRegistration> epm_registrations_;
    std::unordered_map<sem::types::Uuid, std::unique_ptr<boost::process::child>>
        epm_process_handles_;
#endif
};

} // namespace sem::node_manager
#endif // SEM_EPMREGISTRYIMPL_H
