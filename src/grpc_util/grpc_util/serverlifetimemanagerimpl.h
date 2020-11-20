//
// Created by mitchell on 18/11/20.
//

#ifndef SEM_SERVERLIFETIMEMANAGERIMPL_H
#define SEM_SERVERLIFETIMEMANAGERIMPL_H

#include "serverlifetimemanager.h"
#include <condition_variable>
#include <mutex>

namespace sem::grpc_util {

class ServerLifetimeManagerImpl : public ServerLifetimeManager {
public:
    auto shutdown() -> void override;
    auto wait() -> void override;
    auto set_wait_call(std::function<void(void)> wait_call) -> void override;
    auto set_shutdown_call(std::function<void(void)> shutdown_call) -> void override;

private:
    bool shutdown_called_{false};
    std::mutex mutex_;
    std::condition_variable condition_variable_;
    std::function<void()> wait_call_;
    std::function<void()> shutdown_call_;
};

} // namespace sem::grpc_util
#endif // SEM_SERVERLIFETIMEMANAGERIMPL_H
