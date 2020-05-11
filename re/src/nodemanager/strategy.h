//
// Created by cdit-ma on 13/2/20.
//

#ifndef RE_STRATEGY_H
#define RE_STRATEGY_H

#include <functional>
namespace re::NodeManager {

class Strategy {
public:
    explicit Strategy(std::function<void(void)> execution_behaviour);
    auto Execute() -> void {execution_behaviour_();}

private:
    std::function<void(void)> execution_behaviour_;
};

} // namespace re::NodeManager
#endif // RE_STRATEGY_H
