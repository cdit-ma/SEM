#include "utils.h"

#include <algorithm>

#include <google/protobuf/util/time_util.h>

std::string AggServer::GetFullLocation(
    const std::vector<std::string>& locations,
    const std::vector<int>& replication_indices,
    const std::string& component_name)
{
    std::string full_location;

    auto loc_iter = locations.begin();
    auto rep_iter = replication_indices.begin();

    while (loc_iter != locations.end() || rep_iter != replication_indices.end()) {
        full_location.append(*loc_iter).append(".").append(std::to_string(*rep_iter)).append("/");
        loc_iter++;
        rep_iter++;
    }

    return full_location.append(component_name);
}

std::string AggServer::FormatTimestamp(double timestamp) {
    auto&& g_timestamp = google::protobuf::util::TimeUtil::NanosecondsToTimestamp(timestamp*1000000000);
    auto&& str = google::protobuf::util::TimeUtil::ToString(g_timestamp);
    return str;
}

void ltrim(std::string& str, char token) {
    auto token_iter = std::find_if(str.begin(), str.end(), [token](char ch) {
        return (ch == token);
    });
    if(token_iter != str.end()) {
        str.erase(str.begin(), std::next(token_iter));
    }
}

// REVIEW(Jackson): Namespace and stringview
std::string strip_schema(const std::string& str) {
    std::string new_str = str;
    ltrim(new_str, '.');
    return new_str;
}