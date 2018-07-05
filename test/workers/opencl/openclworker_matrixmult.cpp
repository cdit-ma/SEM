#include "common.h"
#include <random>

/****
 * Matrix testing
 ****/

struct Matrix{
    Matrix(size_t rows, size_t columns){
        this->rows = rows;
        this->columns = columns;
    }
    Matrix(size_t size) : Matrix(size, size){};
    Matrix() = default;

     size_t GetElements() const{
        return rows * columns;
    }

    size_t rows = 0;
    size_t columns = 0;
};

struct MatrixMultParam{
    MatrixMultParam(DeviceParam device, const Matrix& matrix_a, const Matrix& matrix_b, bool expect_success = true){
        this->device = device;
        this->matrix_a = matrix_a;
        this->matrix_b = matrix_b;
        this->expect_success = expect_success;
    };
    
    DeviceParam device;
    Matrix matrix_a;
    Matrix matrix_b;
    bool expect_success = true;
};

std::ostream& operator<<(std::ostream& os, const Matrix& m) {
    return os << "[" << m.rows << " x " << m.columns << "]";
};

std::ostream& operator<<(std::ostream& os, const MatrixMultParam& m) {
    return os << m.device << " - Matrix A: " << m.matrix_a << " x Matrix B: " << m.matrix_b;
};

class MatrixMultFixture: public ::testing::TestWithParam<MatrixMultParam>, public OpenCL_WorkerConstructor{
    public:
        MatrixMultFixture() : OpenCL_WorkerConstructor(GetParam().device){
            if(!worker_.Configure()){
                throw std::runtime_error("Failed to configure worker in MatrixMultFixture constructor");
            }
        }
};

TEST_P(MatrixMultFixture, ZeroMatrix)
{
    auto& m_a = GetParam().matrix_a;
    auto& m_b = GetParam().matrix_b;

	std::vector<float> matrix_a(m_a.rows * m_a.columns, 0.0f);
	std::vector<float> matrix_b(m_b.rows * m_b.columns, 0.0f);
	std::vector<float> matrix_c(m_a.rows * m_b.columns, std::numeric_limits<float>::signaling_NaN());

    ASSERT_EQ(worker_.MatrixMult(matrix_a, matrix_b, matrix_c), GetParam().expect_success);
    
    if(GetParam().expect_success){
        //Calculate the expected result
        auto expected_result = CPUMatrixMult(matrix_a.data(), matrix_b.data(), m_a.rows, m_a.columns, m_b.columns);
        EXPECT_FLOATS_NEARLY_EQ(matrix_c, expected_result, EPS);
    }
}

TEST_P(MatrixMultFixture, IncrementalMatrix)
{
    auto& m_a = GetParam().matrix_a;
    auto& m_b = GetParam().matrix_b;

	std::vector<float> matrix_a(m_a.rows * m_a.columns);
	std::vector<float> matrix_b(m_b.rows * m_b.columns);
	std::vector<float> matrix_c(m_a.rows * m_b.columns, std::numeric_limits<float>::signaling_NaN());

    for (size_t index = 0; index < matrix_a.size(); index++) matrix_a[index] = (float)index;
    for (size_t index = 0; index < matrix_b.size(); index++) matrix_b[index] = (float)index;

    ASSERT_EQ(worker_.MatrixMult(matrix_a, matrix_b, matrix_c), GetParam().expect_success);
    
    if(GetParam().expect_success){
        //Calculate the expected result
        auto expected_result = CPUMatrixMult(matrix_a.data(), matrix_b.data(), m_a.rows, m_a.columns, m_b.columns);
        EXPECT_FLOATS_NEARLY_EQ(matrix_c, expected_result, EPS);
    }
}

TEST_P(MatrixMultFixture, DISABLED_RandomTest)
{
    auto& m_a = GetParam().matrix_a;
    auto& m_b = GetParam().matrix_b;

	std::vector<float> matrix_a(m_a.rows * m_a.columns);
	std::vector<float> matrix_b(m_b.rows * m_b.columns);
	std::vector<float> matrix_c(m_a.rows * m_b.columns, std::numeric_limits<float>::signaling_NaN());

    std::default_random_engine random_generator;
    std::default_random_engine generator(testing::UnitTest::GetInstance()->random_seed());
    std::uniform_real_distribution<float> distribution(-1, 1);

    for (size_t index = 0; index < matrix_a.size(); index++) matrix_a[index] = distribution(generator);
    for (size_t index = 0; index < matrix_b.size(); index++) matrix_b[index] = distribution(generator);
    
    ASSERT_EQ(worker_.MatrixMult(matrix_a, matrix_b, matrix_c), GetParam().expect_success);
    
    if(GetParam().expect_success){
        //Calculate the expected result
        auto expected_result = CPUMatrixMult(matrix_a.data(), matrix_b.data(), m_a.rows, m_a.columns, m_b.columns);
        EXPECT_FLOATS_NEARLY_EQ(matrix_c, expected_result, 1.2e-1);
    }
}

// Quick test for NVIDIA to try to isolate apparent rounding errors during fused multiply addition operations
TEST_P(MatrixMultFixture, RandomIdentityTest)
{
    auto& m_a = GetParam().matrix_a;
    auto& m_b = GetParam().matrix_b;

	std::vector<float> matrix_a(m_a.rows * m_a.columns);
	std::vector<float> matrix_b(m_a.columns * m_a.columns);
	std::vector<float> matrix_c(m_a.rows * m_a.columns, std::numeric_limits<float>::signaling_NaN());

    std::default_random_engine random_generator;
    std::default_random_engine generator(testing::UnitTest::GetInstance()->random_seed());
    std::uniform_real_distribution<float> distribution(-1, 1);

    for (size_t index = 0; index < matrix_a.size(); index++) matrix_a[index] = distribution(generator);
    for (size_t x = 0; x < m_a.columns; x++) {
        for (size_t y = 0; y < m_a.columns; y++) {
            if (x == y) {
                matrix_b[x + y*m_a.columns] = 1.0f;
            } else {
                matrix_b[x + y*m_a.columns] = 0.0f;
            }
        }
    }
    
    ASSERT_EQ(worker_.MatrixMult(matrix_a, matrix_b, matrix_c), m_a.columns != 0);
    
    if(GetParam().expect_success){
        //Calculate the expected result
        auto expected_result = CPUMatrixMult(matrix_a.data(), matrix_b.data(), m_a.rows, m_a.columns, m_a.columns);
        EXPECT_FLOATS_NEARLY_EQ(matrix_c, expected_result, 1e-6);
    }
}

std::vector<MatrixMultParam> getMatrixTests(std::vector<DeviceParam> devices, std::vector< std::vector<int> > dimensions, bool expect_success = true){
    std::vector<MatrixMultParam> params;
    for(auto device : devices){
        for(auto dimension : dimensions){
            if(dimension.size() ==  4){
                params.emplace_back(device, Matrix(dimension[0], dimension[1]), Matrix(dimension[2], dimension[3]), expect_success);
            }
        }
    }
    return params;
};

std::vector<MatrixMultParam> getSquareMatrixTests(std::vector<DeviceParam> devices, std::vector<int> sizes, bool expect_success = true){
    std::vector<std::vector<int> > dimensions;
    for(auto size : sizes) dimensions.emplace_back(4, size);
    return getMatrixTests(devices, dimensions, expect_success);
};

std::vector<MatrixMultParam> getSquareTests(){
    std::vector<MatrixMultParam> all_params;
    
    auto devices = getDevices();
    std::vector<int> valid_tests = {1, 2, 4, 32, 37, 64, 1023, 1024};
    std::vector<int> invalid_tests = {0};

    auto valid_params = getSquareMatrixTests(devices, valid_tests, true);
    auto invalid_params = getSquareMatrixTests(devices, invalid_tests, false);

    all_params.insert(all_params.end(), valid_params.begin(), valid_params.end());
    all_params.insert(all_params.end(), invalid_params.begin(), invalid_params.end());
    return all_params;
};

std::vector<MatrixMultParam> getRectTests(){
    std::vector<MatrixMultParam> all_params;

    auto devices = getDevices();
    std::vector<std::vector<int> > valid_tests;
    std::vector<std::vector<int> > invalid_tests;

    //Valid Tests
    valid_tests.emplace_back(std::vector<int>({1,2,2,1}));
    valid_tests.emplace_back(std::vector<int>({2,1,1,2}));
    valid_tests.emplace_back(std::vector<int>({2,3,3,2}));
    valid_tests.emplace_back(std::vector<int>({4,3,3,2}));

    // Following tests intended to target <= 32x32 tile boundaries, but also 64x64 to some degree
    valid_tests.emplace_back(std::vector<int>({13,15,15,17}));
    valid_tests.emplace_back(std::vector<int>({15,17,17,19}));

    valid_tests.emplace_back(std::vector<int>({15,5,5,11}));

    valid_tests.emplace_back(std::vector<int>({29,31,31,33}));
    valid_tests.emplace_back(std::vector<int>({31,33,33,35}));

    valid_tests.emplace_back(std::vector<int>({153,56,56,87}));
    valid_tests.emplace_back(std::vector<int>({74,49,49,169}));

    valid_tests.emplace_back(std::vector<int>({1021,1022,1022,1023}));

    //Invalid Tests
    //TODO: Add more invalid tests
    invalid_tests.emplace_back(std::vector<int>({1,2,3,4}));
    invalid_tests.emplace_back(std::vector<int>({4,2,3,4}));
    invalid_tests.emplace_back(std::vector<int>({1,0,0,1}));
    invalid_tests.emplace_back(std::vector<int>({0,0,0,0}));

    auto valid_params = getMatrixTests(devices, valid_tests, true);
    auto invalid_params = getMatrixTests(devices, invalid_tests, false);

    all_params.insert(all_params.end(), valid_params.begin(), valid_params.end());
    all_params.insert(all_params.end(), invalid_params.begin(), invalid_params.end());
    return all_params;
};

INSTANTIATE_TEST_CASE_P(LONG_Square, MatrixMultFixture, ::testing::ValuesIn(getSquareTests()));
INSTANTIATE_TEST_CASE_P(LONG_Rectangle, MatrixMultFixture, ::testing::ValuesIn(getRectTests()));


/*
class RunParallelFixture: public ::testing::TestWithParam<RunParallelParam>, public OpenCL_WorkerConstructor{
    public:
        RunParallelFixture() : OpenCL_WorkerConstructor(GetParam().device){
            if(!worker_.Configure()){
                HasFatalFailure();
            }
        }
};*/
