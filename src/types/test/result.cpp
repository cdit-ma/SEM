//
// Created by Jackson Michael on 20/8/20.
//

#include "gtest/gtest.h"
#include "result.hpp"

namespace sem::types::test {

    constexpr int a_valid_int() {
        return 4;
    }

    std::vector<int> a_valid_int_vector() {
        return {1,2,3,4};
    }

    std::string a_valid_string() {
        return "A valid string!";
    }

TEST(sem_types_result, get_value_consistently_returns_valid_int){
    auto input_value = a_valid_int();
    int output_value = !input_value;

    // Construct the result type instance using our input
    Result<int> result{input_value};

    // Check that the required operations complete successfully
    ASSERT_NO_THROW(output_value = result.GetValue(););
    ASSERT_EQ(output_value, input_value);

    // Check that
    ASSERT_EQ(result.GetValue(), input_value);
}

TEST(sem_types_result, error_has_correct_msg) {
    std::string input_error_msg = a_valid_string();
    ErrorResult input_error(input_error_msg);

    // Construct the result type instance using our input
    Result<int> result{input_error};

    // We expect the correct exception is thrown if we try to unwrap an error to get the value
    ASSERT_THROW(result.GetValue(), unexpected_error_result);

    // Make sure that the error message is what was provided in the error class
    try {
        result.GetValue();
        // Should not reach this point
        FAIL();
    } catch (const unexpected_error_result& uer) {
        ASSERT_STREQ(uer.what() ,input_error_msg.c_str());
    } catch (...) {
        // Any other type of throw exception is a failure
        FAIL();
    }
}

TEST(sem_types_result, get_value_consistently_returns_valid_int_vector){
    auto input_value = a_valid_int_vector();
    std::vector<int> output_value;
    if (input_value.empty()) {
        output_value = {0};
    } else {
        output_value = {};
    }

    // Construct the result type instance using our input
    Result<std::vector<int>> result{input_value};

    // Check that the required operations complete successfully
    ASSERT_NO_THROW(output_value = result.GetValue(););
    ASSERT_EQ(output_value, input_value);

    // Check that
    ASSERT_EQ(result.GetValue(), input_value);
}

}