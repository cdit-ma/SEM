//
// Created by Jackson Michael on 1/3/21.
//

#include "boost/program_options.hpp"

#include "component.h"
#include "print_logger.h"
#include "sem_fft_accel_worker.hpp"

#include <filesystem>
#include <fstream>
#include <bitset>
#include <optional>

namespace sem::fft_accel::test {

    struct config_data {
        std::string fae_endpoint;
        std::string input_data_path;
        boost::optional<std::string> expected_result_path;
    };

    namespace cmd {
        std::optional<config_data> parse_args(int argc, char **argv) {
            namespace prog_opts = boost::program_options;

            config_data data;

            prog_opts::options_description generic("Generic options");
            generic.add_options()
                    ("help,h", "Print a description of the available commands and exit");

            prog_opts::options_description config("Client configuration options");
            config.add_options()
                    ("fae-endpoint,e", prog_opts::value(&data.fae_endpoint)->required(),
                     "Set the endpoint of the FFT Acceleration Engine this client should connect to")
                    ("input_data,i", prog_opts::value(&data.input_data_path)->required(),
                     "Specify the base filepath for the input fft data ('_r.txt' and '_i.txt' will be appended)")
                    ("expected_result,x", prog_opts::value(&data.expected_result_path),
                     "Specify the base filepath for the expected fft result ('_r.txt' and '_i.txt' will be appended)");

            prog_opts::options_description cmd_options;
            cmd_options.add(generic).add(config);

            prog_opts::variables_map variables_map;
            prog_opts::store(prog_opts::parse_command_line(argc, argv, cmd_options), variables_map);

            if (variables_map.count("help")) {
                std::cout << cmd_options << std::endl;
                return {};
            }

            // At this point, check if the variables marked as required have been provided, throwing as needed
            prog_opts::notify(variables_map);

            return data;
        }
    }

    /// Reinterpret an integer value as a float without performing any cast conversions
    float float_from_bits(uint32_t value) {
        return *reinterpret_cast<float *>(&value);
    }

    /// Convert a bitset to the equivalent IEEE754 float (not broadly tested across may platforms)
    float float_from(std::bitset<32> bits) {
        return float_from_bits(bits.to_ulong());
    }

    namespace file {
        struct fft_dataset {
            std::filesystem::path real_data;
            std::filesystem::path imaginary_data;
        };

        fft_dataset get_dataset_from_base_path(const std::filesystem::path &path) {
            fft_dataset fileset;

            auto base_filename = path.filename();

            fileset.real_data = path;
            fileset.real_data.replace_filename(base_filename.string().append("_r.txt"));

            fileset.imaginary_data = path;
            fileset.imaginary_data.replace_filename(base_filename.string().append("_i.txt"));

            return fileset;
        }

        std::vector<float> load_ascii_binary_fft_data(const std::filesystem::path &path) {

            auto fileset = get_dataset_from_base_path(path);

            std::ifstream real_component_stream(fileset.real_data);
            if (!real_component_stream) {
                throw std::runtime_error("Failed to open file_stream for " + path.string());
            }
            std::ifstream imaginary_component_stream(fileset.imaginary_data);
            if (!imaginary_component_stream) {
                throw std::runtime_error("Failed to open file_stream for " + path.string());
            }

            std::vector<float> fft_data;

            while (real_component_stream.good() && imaginary_component_stream.good()) {
                // Read the ascii bit field values from both files
                std::bitset<32> real_component_bits;
                real_component_stream >> real_component_bits;
                std::bitset<32> imaginary_component_bits;
                imaginary_component_stream >> imaginary_component_bits;

                // Handle the end-of-file case (one file case and both files case)
                if (real_component_stream.eof() != imaginary_component_stream.eof()) {
                    throw std::runtime_error(
                            "Error when parsing FFT data files; reached end of one file while more data remains in the other");
                }
                if (real_component_stream.eof() && imaginary_component_stream.eof()) {
                    break;
                }

                // If we didn't reach the end of the file, add the data values to the vector
                fft_data.push_back(float_from(real_component_bits));
                fft_data.push_back(float_from(imaginary_component_bits));
            }

            return fft_data;
        }
    }

    std::vector<float> calculate_worker_result(const config_data &config, const std::vector<float> &input_data) {
        using fft_worker = sem::fft_accel::Worker;

        Component client_component("FPGA_FFT_testing_client");
        auto fft_client = client_component.AddTypedWorker<fft_worker>("test_fft_client_worker");

        Print::Logger print_logger;
        fft_client->logger().AddLogger(print_logger);

        auto attr_ref = fft_client->GetAttribute(std::string(fft_worker::AttrNames::accelerator_endpoint));
        if (auto endpoint_attr = attr_ref.lock()) {
            endpoint_attr->SetValue(config.fae_endpoint);
        } else {
            throw std::runtime_error("Failed to acquire lock on FAE endpoint attribute");
        }

        if (!client_component.Configure()) {
            throw std::runtime_error("Client component failed to configure");
        }

        return fft_client->calculate_fft(input_data);
    }

    bool compare_and_print_mismatches(std::vector<float> actual, std::vector<float> expected) {
        if (actual.size() != expected.size()) {
            std::cerr << "Size of calculated FFT result doesn't match size of expected result" << std::endl;
            return false;
        }

        bool mismatch_encountered = false;
        for (size_t index = 0; index < actual.size(); index++) {
            if (actual.at(index) != expected.at(index)) {
                std::cerr << "Calculated result doesn't match with expected at index " << index << ": "
                          << actual.at(index) << " vs " << expected.at(index) << std::endl;
                mismatch_encountered = true;
            }
        }

        return mismatch_encountered;
    }

}

int main(int argc, char **argv) {
    using namespace sem::fft_accel::test;

    auto config_result = cmd::parse_args(argc, argv);
    if (!config_result.has_value()) {
        return 1;
    }
    config_data config = config_result.value();

    auto fft_input_data = file::load_ascii_binary_fft_data(config.input_data_path);

    try {
        auto calculated_result = calculate_worker_result(config, fft_input_data);

        if (config.expected_result_path.has_value()) {
            auto expected_result = file::load_ascii_binary_fft_data(config.expected_result_path.value());

            compare_and_print_mismatches(calculated_result, expected_result);
        }

    } catch (const std::exception &ex) {
        std::cerr << "An exception was thrown from a RE system:" << std::endl
                  << ex.what() << std::endl;
        return 1;
    }

    return 0;
}