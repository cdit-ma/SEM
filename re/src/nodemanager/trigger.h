//
// Created by cdit-ma on 13/2/20.
//

#ifndef RE_TRIGGER_H
#define RE_TRIGGER_H

#include "strategy.h"
#include <condition_variable>
#include <future>

#include "controlmessage.pb.h"
#include "socketaddress.hpp"
#include "systeminfobroker.h"
#include "uuid.h"

namespace re::NodeManager {
class Trigger {
public:
    enum class Metric { CpuUtil, MemUtil };
    enum class Comparator { Greater, Less, GreaterOrEqual, LessOrEqual, Equal, NotEqual };

    Trigger(const ::NodeManager::TriggerPrototype& trigger_pb, Strategy strategy);

    Trigger(std::string id,
            std::string name,
            Metric metric,
            Comparator comparator,
            double value,
            Strategy strategy);
    void Start();
    void Terminate();
    ~Trigger(){ Terminate();};

private:
    std::string id_;
    std::string name_;

    Strategy strategy_;

    SystemInfoBroker system_;
    const int listener_id_;

    Metric metric_;
    Comparator comparator_;
    double threshold_value_;

    auto Check() -> void;
    auto GetCurrentMetricValue() -> double;
    auto PollLoop() -> void;

    std::atomic_bool stop_poll_thread_{false};
    std::mutex poll_thread_mutex_;
    std::condition_variable poll_thread_cv_;
    std::future<void> poll_thread_handle_;
};

} // namespace re::NodeManager
#endif // RE_TRIGGER_H
