#include "common.h"
#include <random>

#include <common/testtypes.h>

/****
 * Matrix testing
 ****/

using namespace cdit_ma::test;

/**
 * Simple representation of matrix dimensions
 */
struct MatrixDimensions {
    MatrixDimensions(size_t rows, size_t columns) : rows(rows), columns(columns) {}

    [[nodiscard]] size_t GetNumElements() const { return rows * columns; }

    size_t rows = 0;
    size_t columns = 0;
};

/**
 * Used to store the parameters required to test the matrix multiplication function.
 */
struct MatrixMultParam {
    /**
     * Simple constructor that initialises the required parameters for the
     * @param device
     * @param matrix_a
     * @param matrix_b
     * @param expected_result
     */
    MatrixMultParam(DeviceParam device,
                    const MatrixDimensions& matrix_a,
                    const MatrixDimensions& matrix_b,
                    Result expected_result) :
        device(device),
        matrix_a(matrix_a),
        matrix_b(matrix_b),
        expected_result(expected_result){};

    DeviceParam device;
    MatrixDimensions matrix_a;
    MatrixDimensions matrix_b;
    Result expected_result;
};

/**
 * Overloaded ostream piping operator for pretty printing Matrix dimensions associated with failing
 * cases
 * @param os
 * @param m
 * @return
 */
std::ostream& operator<<(std::ostream& os, const MatrixDimensions& m)
{
    return os << "[" << m.rows << " x " << m.columns << "]";
}

/**
 * Overloaded ostream piping operator for pretty printing MatrixMultParams associated with failing
 * cases
 * @param os
 * @param m
 * @return
 */
std::ostream& operator<<(std::ostream& os, const MatrixMultParam& m)
{
    return os << m.device << " - Mat A: " << m.matrix_a << " x Mat B: " << m.matrix_b;
}

/**
 * The test fixture responsible for performing required initialisation and teardown for testing the
 * MatrixMult function
 */
class MatrixMultFixture : public ::testing::TestWithParam<MatrixMultParam>,
                          public OpenCL_WorkerConstructor {
public:
    /**
     * Constructor performs test setup by ensuring that the worker is in a configured and valid
     * state before tests run
     */
    MatrixMultFixture() : OpenCL_WorkerConstructor(GetParam().device)
    {
        if(!worker_.Configure()) {
            throw std::runtime_error("Failed to configure worker in MatrixMultFixture constructor");
        }
    }
};

/**
 * Zero matrix test fills two matrices with the dimensions provided by the test parameters and
 * ensures that the MatrixMult function reports its success in the expected fashion, as well as not
 * throwing any exceptions. if it did succeed, check that the result of the calculation is that same
 * as that returned by a naive CPU implementation.
 * Disabled because any bugs caught in this test will also be caught in the
 * incremental test, while also being potentially long-running.
 * Re-enable for diagnostic usage as required.
 */
TEST_P(MatrixMultFixture, DISABLED_ZeroMatrix)
{
    auto& m_a = GetParam().matrix_a;
    auto& m_b = GetParam().matrix_b;

    std::vector<float> matrix_a(m_a.rows * m_a.columns, 0.0f);
    std::vector<float> matrix_b(m_b.rows * m_b.columns, 0.0f);
    std::vector<float> matrix_c(m_a.rows * m_b.columns,
                                std::numeric_limits<float>::signaling_NaN());

    bool did_report_success = false;
    Result actual_result = Result::failure;

    ASSERT_NO_THROW(did_report_success = worker_.MatrixMult(matrix_a, matrix_b, matrix_c));
    if(did_report_success) {
        actual_result = Result::success;
    }

    ASSERT_EQ(actual_result, GetParam().expected_result);

    if(did_report_success) {
        // Calculate the expected result
        auto expected_result = CPUMatrixMult(matrix_a.data(), matrix_b.data(), m_a.rows,
                                             m_a.columns, m_b.columns);
        common::expect_nearly_equal(matrix_c, expected_result, EPS);
    }
}

/**
 * Similar structure to the ZeroMatrix test case, but instead fills the matrices with incrementing
 * numbers to provide a more meaningful test that the multiplications are happening in the correct
 * order.
 */
TEST_P(MatrixMultFixture, IncrementalMatrix)
{
    auto& m_a = GetParam().matrix_a;
    auto& m_b = GetParam().matrix_b;

    std::vector<float> matrix_a(m_a.rows * m_a.columns);
    std::vector<float> matrix_b(m_b.rows * m_b.columns);
    std::vector<float> matrix_c(m_a.rows * m_b.columns,
                                std::numeric_limits<float>::signaling_NaN());

    for(size_t index = 0; index < matrix_a.size(); index++)
        matrix_a[index] = (float)index;
    for(size_t index = 0; index < matrix_b.size(); index++)
        matrix_b[index] = (float)index;

    bool did_report_success = false;
    Result actual_result = Result::failure;

    ASSERT_NO_THROW(did_report_success = worker_.MatrixMult(matrix_a, matrix_b, matrix_c));
    if(did_report_success) {
        actual_result = Result::success;
    }

    ASSERT_EQ(actual_result, GetParam().expected_result);

    if(did_report_success) {
        // Calculate the expected result
        auto calculated_output = CPUMatrixMult(matrix_a.data(), matrix_b.data(), m_a.rows,
                                               m_a.columns, m_b.columns);
        common::expect_nearly_equal(matrix_c, calculated_output, EPS);
    }
}

/**
 * Similar to the above test cases, but instead attempts to perform some minimal fuzz testing.
 * Disabled due to false negatives resulting from suspected imprecise floating point implementations
 * on some NVIDIA hardware. Consider re-enabling after re-considering tolerance constraints or
 * marking the problematic hardware as unsupported.
 */
TEST_P(MatrixMultFixture, DISABLED_RandomTest)
{
    auto& m_a = GetParam().matrix_a;
    auto& m_b = GetParam().matrix_b;

    std::vector<float> matrix_a(m_a.rows * m_a.columns);
    std::vector<float> matrix_b(m_b.rows * m_b.columns);
    std::vector<float> matrix_c(m_a.rows * m_b.columns,
                                std::numeric_limits<float>::signaling_NaN());

    std::default_random_engine generator(testing::UnitTest::GetInstance()->random_seed());
    std::uniform_real_distribution<float> distribution(-1, 1);

    for(size_t index = 0; index < matrix_a.size(); index++)
        matrix_a[index] = distribution(generator);
    for(size_t index = 0; index < matrix_b.size(); index++)
        matrix_b[index] = distribution(generator);

    bool did_report_success = false;
    Result actual_result = Result::failure;

    ASSERT_NO_THROW(did_report_success = worker_.MatrixMult(matrix_a, matrix_b, matrix_c));
    if(did_report_success) {
        actual_result = Result::success;
    }

    ASSERT_EQ(actual_result, GetParam().expected_result);

    if(did_report_success) {
        // Calculate the expected result
        auto expected_result = CPUMatrixMult(matrix_a.data(), matrix_b.data(), m_a.rows,
                                             m_a.columns, m_b.columns);
        common::expect_nearly_equal(matrix_c, expected_result, 1.2e-1);
    }
}

/**
 * Quick test for NVIDIA to try to isolate apparent rounding errors during fused multiply addition
 * operations
 */
TEST_P(MatrixMultFixture, DISABLED_RandomIdentityTest)
{
    auto& m_a = GetParam().matrix_a;
    auto& m_b = GetParam().matrix_b;

    std::vector<float> matrix_a(m_a.rows * m_a.columns);
    std::vector<float> matrix_b(m_b.columns * m_b.columns);
    std::vector<float> matrix_c(m_a.rows * m_b.columns,
                                std::numeric_limits<float>::signaling_NaN());

    std::default_random_engine generator(testing::UnitTest::GetInstance()->random_seed());
    std::uniform_real_distribution<float> distribution(-1, 1);

    for(size_t index = 0; index < matrix_a.size(); index++)
        matrix_a[index] = distribution(generator);
    for(size_t x = 0; x < m_b.columns; x++) {
        for(size_t y = 0; y < m_b.rows; y++) {
            if(x == y) {
                matrix_b[x + y * m_b.columns] = 1.0f;
            } else {
                matrix_b[x + y * m_b.columns] = 0.0f;
            }
        }
    }

    bool did_report_success = false;
    Result actual_result = Result::failure;

    ASSERT_NO_THROW(did_report_success = worker_.MatrixMult(matrix_a, matrix_b, matrix_c));
    if(did_report_success) {
        actual_result = Result::success;
    }

    ASSERT_EQ(actual_result, GetParam().expected_result);

    if(did_report_success) {
        // Calculate the expected result
        auto expected_result = CPUMatrixMult(matrix_a.data(), matrix_b.data(), m_a.rows,
                                             m_a.columns, m_b.columns);
        common::expect_nearly_equal(matrix_c, expected_result, 1e-6);
    }
}

/**
 * Generate a vector of MatrixMultParams that is a permutation of the input/output pairs and the
 * available devices on which the test could run
 * @param devices
 * @param dimensions
 * @param expect_success
 * @return
 */
std::vector<MatrixMultParam> permuteMatrixTests(const std::vector<DeviceParam>& devices,
                                                const std::vector<std::vector<int>>& dimensions,
                                                Result expected_result)
{
    std::vector<MatrixMultParam> params;
    for(auto device : devices) {
        for(auto dimension : dimensions) {
            if(dimension.size() == 4) {
                params.emplace_back(device, MatrixDimensions(dimension[0], dimension[1]),
                                    MatrixDimensions(dimension[2], dimension[3]), expected_result);
            }
        }
    }
    return params;
}

/**
 * A wrapper function for permuteMatrixTests(...) that provides a simplified interface for permuting
 * square matrices
 * @param devices
 * @param sizes
 * @param expect_success
 * @return
 */
std::vector<MatrixMultParam> permuteSquareMatrixTests(const std::vector<DeviceParam>& devices,
                                                      const std::vector<int>& sizes,
                                                      Result expected_result)
{
    std::vector<std::vector<int>> dimensions;
    for(auto size : sizes)
        dimensions.emplace_back(4, size);
    return permuteMatrixTests(devices, dimensions, expected_result);
}

/**
 * Generate a list of MatrixMult test parameters that will run the function with two square matrices
 * of the same size. These tests are all expected to succeed and run in a relatively short amount of
 * time.
 * @return
 */
std::vector<MatrixMultParam> getValidSquareTestParams()
{
    auto devices = getDevices();
    std::vector<int> valid_tests = {1, 2, 4, 32, 37, 64};
    return permuteSquareMatrixTests(devices, valid_tests, Result::success);
}

/**
 * Generate a list of MatrixMult test parameters for square matrices that are expected to cause the
 * function to report failure; currently only the 0 size matrix due to problems in which the
 * underlying OpenCL framework is unable to represent 0 length buffers.
 * @return
 */
std::vector<MatrixMultParam> getInvalidSquareTestParams()
{
    auto devices = getDevices();
    std::vector<int> invalid_tests = {0};
    return permuteSquareMatrixTests(devices, invalid_tests, Result::failure);
}

/**
 * Generate a list of MatrixMult parameters that are expected to succeed, however may take a
 * significant amount of time to complete.
 * @return
 */
std::vector<MatrixMultParam> getLongSquareTestParams()
{
    auto devices = getDevices();
    std::vector<int> long_tests = {1023, 1024};
    return permuteSquareMatrixTests(devices, long_tests, Result::success);
}

/**
 * Generate a list of MatrixMult test parameters that will run the function with an assortment of
 * rectangular matrices. These tests are all expected to succeed and run in a relatively short
 * amount of time.
 * @return
 */
std::vector<MatrixMultParam> getValidRectTestParams()
{
    auto devices = getDevices();
    std::vector<std::vector<int>> valid_tests;

    // Small tests
    valid_tests.emplace_back(std::vector<int>({1, 2, 2, 1}));
    valid_tests.emplace_back(std::vector<int>({2, 1, 1, 2}));
    valid_tests.emplace_back(std::vector<int>({2, 3, 3, 2}));
    valid_tests.emplace_back(std::vector<int>({4, 3, 3, 2}));

    // Following tests intended to target <= 32x32 tile boundaries, but also 64x64 to some degree
    valid_tests.emplace_back(std::vector<int>({13, 15, 15, 17}));
    valid_tests.emplace_back(std::vector<int>({15, 17, 17, 19}));

    valid_tests.emplace_back(std::vector<int>({15, 5, 5, 11}));

    valid_tests.emplace_back(std::vector<int>({29, 31, 31, 33}));
    valid_tests.emplace_back(std::vector<int>({31, 33, 33, 35}));

    valid_tests.emplace_back(std::vector<int>({153, 56, 56, 87}));
    valid_tests.emplace_back(std::vector<int>({74, 49, 49, 169}));

    return permuteMatrixTests(devices, valid_tests, Result::success);
}

/**
 * Generate a list of MatrixMult parameters for rectangular matrices with dimensions that either
 * don't match and therefore cannot be multiplied or dimensions that contain 0. All of these tests
 * Are expected to cause the MatrixMult function to return failure.
 * @return
 */
std::vector<MatrixMultParam> getInvalidRectTestParams()
{
    auto devices = getDevices();
    std::vector<std::vector<int>> invalid_tests;

    // Invalid Tests
    // TODO: Add more invalid tests
    invalid_tests.emplace_back(std::vector<int>({1, 2, 3, 4}));
    invalid_tests.emplace_back(std::vector<int>({4, 2, 3, 4}));
    invalid_tests.emplace_back(std::vector<int>({1, 0, 0, 1}));
    invalid_tests.emplace_back(std::vector<int>({0, 0, 0, 0}));

    return permuteMatrixTests(devices, invalid_tests, Result::failure);
}

/**
 * Generate a list of MatrixMult parameters that show the function is capable of successfully
 * performing calculations on relatively large matrices, albeit taking a significant amount of time
 * to do so.
 * @return
 */
std::vector<MatrixMultParam> getLongRectTestParams()
{
    auto devices = getDevices();
    std::vector<std::vector<int>> long_tests;

    long_tests.emplace_back(std::vector<int>({1021, 1022, 1022, 1023}));

    return permuteMatrixTests(devices, long_tests, Result::success);
}

INSTANTIATE_TEST_CASE_P(Re_Worker_OpenclWorker_Square_Valid,
                        MatrixMultFixture,
                        ::testing::ValuesIn(getValidSquareTestParams()),
                        common::PrintParamName());
INSTANTIATE_TEST_CASE_P(Re_Worker_OpenclWorker_Square_Invalid,
                        MatrixMultFixture,
                        ::testing::ValuesIn(getInvalidSquareTestParams()),
                        common::PrintParamName());
INSTANTIATE_TEST_CASE_P(Re_Worker_OpenclWorker_Square_LONG,
                        MatrixMultFixture,
                        ::testing::ValuesIn(getLongSquareTestParams()),
                        common::PrintParamName());

INSTANTIATE_TEST_CASE_P(Re_Worker_OpenclWorker_Rect_Valid,
                        MatrixMultFixture,
                        ::testing::ValuesIn(getValidRectTestParams()),
                        common::PrintParamName());
INSTANTIATE_TEST_CASE_P(Re_Worker_OpenclWorker_Rect_Invalid,
                        MatrixMultFixture,
                        ::testing::ValuesIn(getInvalidRectTestParams()),
                        common::PrintParamName());
INSTANTIATE_TEST_CASE_P(Re_Worker_OpenclWorker_Rect_LONG,
                        MatrixMultFixture,
                        ::testing::ValuesIn(getLongRectTestParams()),
                        common::PrintParamName());