//
// Created by Jackson on 10/10/2019.
//

#ifndef RE_COMMON_H
#define RE_COMMON_H

// Provides floating-point compatible version of std::abs on clang
#include <cmath>

#include <algorithm>
#include <vector>

#include <gtest/gtest.h>

#include "testtypes.h"

namespace cdit_ma::test::common {
/**
 * Compare two vectors to see if the actual value is within a % threshold of the expected values
 * @tparam T the type contained by the vectors
 * @param expected The expected values that are to be tested against
 * @param actual The actual values reported during the test
 * @param thresh
 */
template<typename T>
void expect_nearly_equal(const std::vector<T>& expected, const std::vector<T>& actual, float thresh)
{
    EXPECT_EQ(expected.size(), actual.size()) << "Array sizes differ.";
    for(size_t idx = 0; idx < std::min(expected.size(), actual.size()); ++idx) {
        if(actual[idx] == 0) {
            EXPECT_NEAR(expected[idx], actual[idx], thresh) << "at index: " << idx;
        } else {
            const auto&& ratio = std::abs(expected[idx] / actual[idx]);
            EXPECT_NEAR(ratio, 1, thresh);
        }
    }
}

struct PrintParamName {
    /**
     * Prints a parameter class by using the overloaded string piping operator defined for that
     * class and striping all non-alphanumeric/underscore characters  (done to match googletest
     * parameter naming requirements)
     * @tparam ParamType The type of Parameter to pretty print
     * @param info The TestParamInfo object containing the parameter in question
     * @return The formatted string in question
     */
    template<class ParamType>
    std::string operator()(const ::testing::TestParamInfo<ParamType>& info) const
    {
        ::std::stringstream ss;
        ss << info.param;
        std::string out_string = ss.str();

        auto trim_invalid_chars = [](auto const& c) -> bool {
            return (!std::isalnum(c) && c != '_');
        };

        out_string.erase(std::remove_if(out_string.begin(), out_string.end(), trim_invalid_chars),
                         out_string.end());
        return out_string;
    }
};
} // namespace cdit_ma::test::common

#endif // RE_COMMON_H
