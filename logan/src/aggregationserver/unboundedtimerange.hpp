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

/**
 * Convert a time_point to a string representation
 * @param timepoint The time_point value
 * @return The stringed representation
 */
static inline auto
StringFromTimepoint(const time_point& timepoint) -> std::string {
    std::stringstream ss;
    ss << clock::to_time_t(timepoint);
    return ss.str();
}

/**
 * A class representing the range of time between a start and an end time, with the option to have boundaries of
 * negative and positive infinity. In plain english, this allows for time ranges such as "From time X until the end of
 * time" or "Until the beginning of time until time Y" to be represented.
 * These infinite values are represented by the use of 'nullopt'.
 * @tparam TimeType The type being used to represent time points, e.g. std::chrono::time_point
 */
template <typename TimeType>
class UnboundedTimeRange {
public:
    /**
     * Create an UnboundedTimeRange
     * @param start The TimeType value from which the range starts, or nullopt for the beginning of time
     * @param end The TimeType value at which the range ends, or nullopt for the end of time
     */
    UnboundedTimeRange(std::optional<TimeType> start, std::optional<TimeType> end) :
        start_(std::move(start)),
        end_(std::move(end)){};

    /**
     * Get the start time of the time range
     * @return nullopt if the beginning of time, a wrapped time point if not.
     */
    std::optional<TimeType> Start() const { return start_; };

    /**
     * Get the end time of the time range
     * @return nullopt if the end of time, a wrapped time point if not.
     */
    std::optional<TimeType> End() const { return end_; };

private:
    std::optional<TimeType> start_;
    std::optional<TimeType> end_;
};

} // namespace re::types

#endif // RE_UNBOUNDEDTIMERANGE_HPP
