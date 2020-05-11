//
// Created by cdit-ma on 13/2/20.
//

#include "strategy.h"

#include <utility>
namespace re::NodeManager {
Strategy::Strategy(std::function<void(void)> execution_behaviour) :
    execution_behaviour_{std::move(execution_behaviour)}
{
}
} // namespace re::NodeManager
