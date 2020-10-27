//
// Created by mitchell on 22/10/20.
//

#include "epmregistryimpl.h"
namespace sem::node_manager {

auto EpmRegistryImpl::start_epm() -> void {}

auto EpmRegistryImpl::epm_started(types::Uuid request_uuid,
                                  types::SocketAddress control_endpoint,
                                  types::SocketAddress data_endpoint) -> void
{

}

auto EpmRegistryImpl::remove_epm(sem::types::Uuid uuid) -> void
{
    std::lock_guard lock(mutex_);
}

auto EpmRegistryImpl::get_epm_info(sem::types::Uuid uuid) -> EpmRegistry::EpmInfo
{
    std::lock_guard lock(mutex_);
    return epm_handles_.at(uuid);
}

#if 0

/// Starts a new ExperimentProcessManager with the given configuration details.
/// EPM will communicate directly with EnvironmentManager(ExperimentManager) for further, experiment
///  specific, configuration details.
auto NodeManager::NewEpm(const sem::types::Uuid& experiment_uuid,
                         const std::string& experiment_name) -> sem::types::Uuid
{
    std::cout << "[NodeManager] - Starting new epm for experiment:\n    " << experiment_name << " ("
              << experiment_uuid << ")" << std::endl;

    namespace bp = boost::process;
    auto&& request_uuid = sem::types::Uuid{};
    auto&& epm_start_command = detail::BuildEpmStartCommand(node_config_, experiment_uuid,
                                                            request_uuid);

    // Wait for our epm to send its registration message back to the node manager. Return its
    // uuid s.t. the environment manager knows where to find it.
    auto epm_uuid = WaitForEpmRegistrationMessage(request_uuid);
    epm_process_handles_.emplace(epm_uuid, std::make_unique<bp::child>(epm_start_command));
    std::cout << "[NodeManager] - Successfully STARTED epm:\n    (" << epm_uuid << ")" << std::endl;
    return epm_uuid;
}

/// Waits on a semaphore that is notified in the epm registration replier thread that runs:
///  HandleEpmRegistration
auto NodeManager::WaitForEpmRegistrationMessage(const sem::types::Uuid& request_uuid)
-> sem::types::Uuid
{
    // How long we should wait for the EPM to start
    // TODO: Tune this number?
    auto registration_wait_timeout = std::chrono::milliseconds(1000);

    std::unique_lock lock{epm_registration_mutex_};
    if(epm_registration_semaphore_.wait_for(lock, registration_wait_timeout, [this, request_uuid]() {
      // Check that we have a registration to process and
      // that the registration id matches.
      //  XXX: This could be a problem?
      return !epm_registrations_.empty()
             && epm_registrations_.front().request_uuid == request_uuid;
    })) {
        auto epm_uuid = epm_registrations_.front().epm_uuid;
        epm_registrations_.pop();

        return epm_uuid;
    } else {
        throw std::runtime_error("EPM startup timed out.");
    }
}

/// Used as callback for the NodeManager's EPM registration listener.
auto NodeManager::HandleEpmRegistration(const EpmRegistrationRequest& request)
-> EpmRegistrationReply
{
    // TODO: Build list of pending registration uuids. Check against that before we notify.
    auto request_uuid = sem::types::Uuid{request.request_uuid()};
    auto epm_uuid = sem::types::Uuid{request.epm_uuid()};
    {
        std::lock_guard lock{epm_registration_mutex_};
        epm_registrations_.push({request_uuid, epm_uuid});
    }
    epm_registration_semaphore_.notify_one();
    EpmRegistrationReply reply;
    reply.set_success(true);
    return reply;
}
#endif

} // namespace sem::node_manager
