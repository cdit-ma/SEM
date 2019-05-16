#include "common.h"
#include <cmath>
#include <random>

using namespace Re::OpenCL;

struct KMeansParam {
    KMeansParam(const DeviceParam& device,
                const std::vector<float>& points,
                const std::vector<float>& centroids,
                const std::vector<int>& expected_classifications,
                size_t iterations,
                bool expect_success = true)
    {
        this->device = device;
        this->points = points;
        this->centroids = centroids;
        this->expected_classifications = expected_classifications;
        this->iterations = iterations;
        this->expect_success = expect_success;
    }
    DeviceParam device;
    std::vector<float> points;
    std::vector<float> centroids;
    std::vector<int> expected_classifications;
    size_t iterations;
    bool expect_success;
};

template<typename T> std::string print_vec(std::vector<T> vec)
{
    std::string str = "{";
    unsigned int elements_appended = 0;
    for(const auto& element : vec) {
        if(elements_appended > 0) {
            str.append(", ");
        }
        str.append(std::to_string(element));
        elements_appended++;
        if(elements_appended > 16) {
            str.append(", ...");
            break;
        }
    }
    str.append("}");
    return str;
}

std::ostream& operator<<(std::ostream& os, const KMeansParam& kmp)
{
    return os << kmp.device << "points: [" << print_vec(kmp.points) << "] - centroids: [" << print_vec(kmp.centroids)
              << "] - expected_classifications: [" << print_vec(kmp.expected_classifications)
              << "] - expect_success: " << kmp.expect_success;
}

class KMeansFixture : public ::testing::TestWithParam<KMeansParam>, public OpenCL_WorkerConstructor {
    public:
    KMeansFixture() : OpenCL_WorkerConstructor(GetParam().device)
    {
        if(!worker_.Configure()) {
            throw std::runtime_error("Failed to configure worker in KMeansFixture constructor");
        }
    }
};

TEST_P(KMeansFixture, KMeans)
{
    auto points = GetParam().points;
    auto centroids = GetParam().centroids;
    auto expected_classifications = GetParam().expected_classifications;
    auto iterations = GetParam().iterations;

    std::vector<int> out_classifications(expected_classifications.size());
    bool did_report_success = false;
    ASSERT_NO_THROW(did_report_success = worker_.KmeansCluster(points, centroids, out_classifications, iterations));
    ASSERT_EQ(did_report_success, true);

    ASSERT_EQ(out_classifications, expected_classifications);
}

class float4 : public std::vector<float> {
    using std::vector<float>::vector;

    public:
    float4(float x, float y, float z, float w) : std::vector<float>({x, y, z, w}){};
    float4() : std::vector<float>(4, 0.0){};
};

struct KMeansData {
    KMeansData(const std::vector<float>& points,
               const std::vector<float>& centers,
               const std::vector<int>& classifications)
    {
        this->points = points;
        this->centers = centers;
        this->classifications = classifications;
    }
    KMeansData() = default;

    std::vector<float> points;
    std::vector<float> centers;
    std::vector<int> classifications;
};

struct ClusterData {
    ClusterData(const float4& center, size_t num_points, float spread)
    {
        this->center = center;
        this->num_points = num_points;
        this->spread = spread;
    }
    ClusterData() = default;
    float4 center;
    size_t num_points;
    float spread;
};

const double PI = 3.141592653589793;
// Note: simple generator, don't let clusters overlap!! Will need to be extended to test overlaps
KMeansData generateKMeansTuple(const std::vector<ClusterData>& clusters)
{
    KMeansData data;
    std::vector<float> points;

    std::default_random_engine random_generator;
    std::default_random_engine generator(testing::UnitTest::GetInstance()->random_seed());

    int cluster_num = 0;
    for(const auto& cluster : clusters) {
        std::normal_distribution<double> spread_distribution(-cluster.spread, cluster.spread);
        float4 new_center;
        for(unsigned int i = 0; i < cluster.num_points; i++) {
            for(unsigned int i = 0; i < 4; i++) {
                double val = spread_distribution(generator);
                data.points.emplace_back((float)val);
                new_center[i] += (float)(val / cluster.num_points);
            }
            data.classifications.emplace_back(cluster_num);
        }
        for(int i = 0; i < 4; i++) {
            data.centers.emplace_back(new_center[i]);
        }
        cluster_num++;
    }

    return data;
}

std::vector<KMeansParam> getKMeansParamPermutations(const std::vector<DeviceParam>& devices,
                                                    const std::vector<KMeansData>& datasets,
                                                    const std::vector<size_t> iteration_vec)
{
    std::vector<KMeansParam> all_params;

    for(const auto& device : devices) {
        for(const auto& data : datasets) {
            for(const auto& iterations : iteration_vec) {
                all_params.emplace_back(device, data.points, data.centers, data.classifications, iterations);
            }
        }
    }
    return all_params;
}

KMeansData generateSimpleData()
{
    KMeansData data;
    data.points = {10, 4, 5, 7, 19, 3, 6, 5, 2, 11, 0, -3, 4, 9, 1, 0, 13, 5, 4, 9, 7, 7, 2, 2};
    data.centers = {0, 10, 0, 0, 10, 1, 1, 7};
    data.classifications = {1, 1, 0, 0, 1, 0};
    return data;
}

KMeansData generateMediumData()
{
    KMeansData data;
    for(int i = 0; i < 2000; i++) {
        data.points.push_back(300 + (float)(rand() % 50));
        data.points.push_back(900 + (float)(rand() % 50));
        data.points.push_back(100 + (float)(rand() % 50));
        data.points.push_back(100 + (float)(rand() % 50));
        data.classifications.push_back(1);
    }
    for(int i = 0; i < 4000; i++) {
        data.points.push_back(700 + (float)(rand() % 50));
        data.points.push_back(200 + (float)(rand() % 50));
        data.points.push_back(600 + (float)(rand() % 50));
        data.points.push_back(700 + (float)(rand() % 50));
        data.classifications.push_back(2);
    }
    for(int i = 0; i < 3000; i++) {
        data.points.push_back(100 + (float)(rand() % 50));
        data.points.push_back(400 + (float)(rand() % 50));
        data.points.push_back(900 + (float)(rand() % 50));
        data.points.push_back(300 + (float)(rand() % 50));
        data.classifications.push_back(0);
    }
    data.centers = {250, 350, 900, 400, 450, 750, 200, 300, 650, 300, 750, 900};
    return data;
}

std::vector<KMeansParam> getKMeansParams()
{
    std::vector<KMeansData> datasets;

    const auto& devices = getDevices();

    datasets.emplace_back(generateSimpleData());
    datasets.emplace_back(generateMediumData());
    std::vector<size_t> iterations = {1, 5};

    return getKMeansParamPermutations(devices, datasets, iterations);
}

INSTANTIATE_TEST_CASE_P(Re_Worker_OpenclWorker_KMeans, KMeansFixture, ::testing::ValuesIn(getKMeansParams()));
