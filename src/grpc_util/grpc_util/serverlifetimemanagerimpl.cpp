//
// Created by mitchell on 18/11/20.
//

#include "serverlifetimemanagerimpl.h"
void sem::grpc_util::ServerLifetimeManagerImpl::shutdown()
{
    std::lock_guard lock{mutex_};
    shutdown_called_ = true;
    condition_variable_.notify_all();
    shutdown_call_();
}
void sem::grpc_util::ServerLifetimeManagerImpl::wait()
{
    std::unique_lock lock{mutex_};
    condition_variable_.wait(lock, [this]() { return shutdown_called_; });
    wait_call_();
}
void sem::grpc_util::ServerLifetimeManagerImpl::set_wait_call(std::function<void(void)> wait_call)
{
    wait_call_ = wait_call;
}
void sem::grpc_util::ServerLifetimeManagerImpl::set_shutdown_call(
    std::function<void(void)> shutdown_call)
{
    shutdown_call_ = shutdown_call;
}
