//
// Created by mitchell on 18/11/20.
//

#ifndef SEM_SERVERLIFETIMEMANAGER_H
#define SEM_SERVERLIFETIMEMANAGER_H

#include <functional>

namespace sem::grpc_util {
class ServerLifetimeManager {
public:

    virtual ~ServerLifetimeManager() = 0;

    virtual auto shutdown() -> void = 0;
    virtual auto wait() -> void = 0;

    virtual auto set_wait_call(std::function<void(void)> wait_call) -> void = 0;
    virtual auto set_shutdown_call(std::function<void(void)> shutdown_call) -> void = 0;
};

inline ServerLifetimeManager::~ServerLifetimeManager() = default;
} // namespace sem::grpc_util

#endif // SEM_SERVERLIFETIMEMANAGER_H
