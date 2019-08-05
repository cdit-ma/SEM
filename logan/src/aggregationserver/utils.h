#ifndef AGGSERVER_UTILS_H
#define AGGSERVER_UTILS_H

#include <string>
#include <vector>

namespace AggServer {
    // String builder functions
    std::string GetFullLocation(
        const std::vector<std::string>& locations,
        const std::vector<int>& replication_indices,
        const std::string& component_name);
    std::string FormatTimestamp(double timestamp);
}

// Trims leftmost part of string up to and including token, does nothing if no token found
void ltrim(std::string& str, char token);

// Removes schema and punctuation from qualified "Schema.Table" notation
std::string strip_schema(const std::string& str);

#endif