//
// Created by mitchell on 21/10/20.
//

#ifndef SEM_NODEMANAGERREGISTRYPROVIDER_H
#define SEM_NODEMANAGERREGISTRYPROVIDER_H
#include "nodemanagerregistryimpl.h"

namespace sem::environment_manager {

class NodeManagerRegistryProvider {
public:
    [[nodiscard]] virtual auto get_registry() const -> NodeManagerRegistryImpl& = 0;
    virtual ~NodeManagerRegistryProvider() = 0;
};
inline NodeManagerRegistryProvider::~NodeManagerRegistryProvider() = default;
} // namespace sem::environment_manager

#endif // SEM_NODEMANAGERREGISTRYPROVIDER_H
