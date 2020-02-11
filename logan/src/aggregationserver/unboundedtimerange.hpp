//
// Created by Jackson on 24/01/2020.
//

#ifndef RE_UNBOUNDEDTIMERANGE_HPP
#define RE_UNBOUNDEDTIMERANGE_HPP

#include <chrono>
#include <optional>

#include <google/protobuf/util/time_util.h>

namespace re::types {

using clock = std::chrono::system_clock;
using time_point = std::chrono::time_point<clock>;

static inline auto
StringFromTimepoint(const time_point& timepoint) -> std::string {
    std::stringstream ss;
    ss << clock::to_time_t(timepoint);
    return ss.str();
}

template <typename TimeType>
class UnboundedTimeRange {
public:
    UnboundedTimeRange(std::optional<TimeType> start, std::optional<TimeType> end) :
        start_(std::move(start)),
        end_(std::move(end)){};

    std::optional<TimeType> Start() const { return start_; };
    std::optional<TimeType> End() const { return end_; };

private:
    std::optional<TimeType> start_;
    std::optional<TimeType> end_;
};

} // namespace re::types

#endif // RE_UNBOUNDEDTIMERANGE_HPP
