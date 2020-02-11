#ifndef RE_PROTOTIMERANGE_H
#define RE_PROTOTIMERANGE_H

#include "unboundedtimerange.hpp"

namespace re::types::proto {

namespace proto = google::protobuf;
using Timestamp = google::protobuf::Timestamp;

/// Convert a protobuf timestamp list to an unbounded timerange
/// \param timerange
/// \return
static UnboundedTimeRange<time_point>
FromProto(const google::protobuf::RepeatedPtrField<Timestamp>& timerange)
{
    // Handle the case where there are more times provided than just a start and end
    if(timerange.size() > 2) {
        throw std::invalid_argument("Cannot construct UnboundedTimeRange from protobuf repeated "
                                    "field containing more than 2 Timestamps");
    }

    using proto_time = google::protobuf::util::TimeUtil;
    std::optional<time_point> start_time, end_time;
    if(timerange.size() >= 1) {
        start_time.emplace(clock::from_time_t(proto::util::TimeUtil::TimestampToTimeT(timerange[0])));
        //start_time = clock::from_time_t(proto_time::TimestampToTimeT(*start));
    }

    if(timerange.size() == 2) {
        end_time.emplace(clock::from_time_t(proto::util::TimeUtil::TimestampToTimeT(timerange[1])));
        //end_time = clock::from_time_t(proto_time::TimestampToTimeT(*end));
    }

    return UnboundedTimeRange<time_point >(start_time, end_time);
}

static inline auto to_string(const Timestamp& timestamp) -> std::string
{
    using proto_tu = google::protobuf::util::TimeUtil;

    return proto_tu::ToString(timestamp);
}

/*static inline auto ToStdUnboundedTimeRange(const UnboundedTimeRange<Timestamp>& proto_timerange)
    -> UnboundedTimeRange<time_point>
{
    return UnboundedTimeRange<time_point>(proto_timerange.Start(), proto_timerange.End());
}*/

} // namespace re::types::proto

#endif