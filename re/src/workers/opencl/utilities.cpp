#include "utilities.h"

#include <algorithm>
#include "logger.h"
#include "worker.h"
#include <fstream>
#include <functional>
#include <sstream>

namespace Re::OpenCL {

std::string OpenCLErrorName(int opencl_error_code)
{
    static const std::vector<std::string> clErrorNames({"CL_SUCCESS",
                                                        "CL_DEVICE_NOT_FOUND",
                                                        "CL_DEVICE_NOT_AVAILABLE",
                                                        "CL_COMPILER_NOT_AVAILABLE",
                                                        "CL_MEM_OBJECT_ALLOCATION_FAILURE",
                                                        "CL_OUT_OF_RESOURCES",
                                                        "CL_OUT_OF_HOST_MEMORY",
                                                        "CL_PROFILING_INFO_NOT_AVAILABLE",
                                                        "CL_MEM_COPY_OVERLAP",
                                                        "CL_IMAGE_FORMAT_MISMATCH",
                                                        "CL_IMAGE_FORMAT_NOT_SUPPORTED",
                                                        "CL_BUILD_PROGRAM_FAILURE",
                                                        "CL_MAP_FAILURE",
                                                        "CL_MISALIGNED_SUB_BUFFER_OFFSET",
                                                        "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST",
                                                        "",
                                                        "",
                                                        "",
                                                        "",
                                                        "",
                                                        "",
                                                        "",
                                                        "",
                                                        "",
                                                        "",
                                                        "",
                                                        "",
                                                        "",
                                                        "",
                                                        "",
                                                        "CL_INVALID_VALUE",
                                                        "CL_INVALID_DEVICE_TYPE",
                                                        "CL_INVALID_PLATFORM",
                                                        "CL_INVALID_DEVICE",
                                                        "CL_INVALID_CONTEXT",
                                                        "CL_INVALID_QUEUE_PROPERTIES",
                                                        "CL_INVALID_COMMAND_QUEUE",
                                                        "CL_INVALID_HOST_PTR",
                                                        "CL_INVALID_MEM_OBJECT",
                                                        "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR",
                                                        "CL_INVALID_IMAGE_SIZE",
                                                        "CL_INVALID_SAMPLER",
                                                        "CL_INVALID_BINARY",
                                                        "CL_INVALID_BUILD_OPTIONS",
                                                        "CL_INVALID_PROGRAM",
                                                        "CL_INVALID_PROGRAM_EXECUTABLE",
                                                        "CL_INVALID_KERNEL_NAME",
                                                        "CL_INVALID_KERNEL_DEFINITION",
                                                        "CL_INVALID_KERNEL",
                                                        "CL_INVALID_ARG_INDEX",
                                                        "CL_INVALID_ARG_VALUE",
                                                        "CL_INVALID_ARG_SIZE",
                                                        "CL_INVALID_KERNEL_ARGS",
                                                        "CL_INVALID_WORK_DIMENSION",
                                                        "CL_INVALID_WORK_GROUP_SIZE",
                                                        "CL_INVALID_WORK_ITEM_SIZE",
                                                        "CL_INVALID_GLOBAL_OFFSET",
                                                        "CL_INVALID_EVENT_WAIT_LIST",
                                                        "CL_INVALID_EVENT",
                                                        "CL_INVALID_OPERATION",
                                                        "CL_INVALID_GL_OBJECT",
                                                        "CL_INVALID_BUFFER_SIZE",
                                                        "CL_INVALID_MIP_LEVEL",
                                                        "CL_INVALID_GLOBAL_WORK_SIZE",
                                                        "CL_INVALID_PROPERTY"});

    auto index = -opencl_error_code;
    if(index >= 0 && index < clErrorNames.size()) {
        return clErrorNames.at(index);
    } else {
        return std::string("Unknown OpenCL error code: ") + std::to_string(opencl_error_code);
    }
}

bool isValidChar(char c)
{
    switch(c) {
        case '\\':
        case '/':
        case ':':
        case '*':
        case '?':
        case '"':
        case '<':
        case '>':
        case '|':
        case ' ':
        case '@':
            return false;
        default:
            return true;
    }
}

std::string SanitisePathString(const std::string& str)
{
    std::string result;
    result.reserve(str.length());

    std::remove_copy_if(str.begin(), str.end(), std::back_inserter(result),
                        std::not1(std::function<bool(char)>(isValidChar)));

    return result;
}

#ifndef KERNEL_FILEPATH
static_assert(false, "No Kernel filepath defined!");
#endif

std::string GetSourcePath(std::string filename)
{
    std::string source_file_path(KERNEL_FILEPATH);
    auto source_dir = source_file_path.substr(0, source_file_path.find_last_of("/\\") + 1);
    return source_dir + filename;
}

/**
 * Takes a list of filenames and returns a list of c-strings containg their contents
 **/
cl::Program::Sources ReadOpenCLSourceCode(const std::vector<std::string>& filenames)
{
    cl::Program::Sources source_list;

    for(const auto& filename : filenames) {
        std::ifstream source_file;
        source_file.open(filename);
        if(!source_file.is_open()) {
            throw std::runtime_error(std::string(__func__)
                                     + ": Failed to open file when reading source files: " + filename);
        }

        std::stringstream source_stream;
        source_stream << source_file.rdbuf();

        source_list.push_back(source_stream.str());
    }

    return source_list;
}

/**
 * Takes a list of filenames and returns a list of binary vectors containg their contents
 **/
cl::Program::Binaries ReadOpenCLBinaries(const std::vector<std::string>& filenames)
{
    cl::Program::Binaries binary_list;

    bool success = true;
    std::string error_message;

    for(const auto& filename : filenames) {
        std::ifstream binary_file;
        binary_file.open(filename, std::ios::binary);
        if(!binary_file.is_open()) {
            throw std::runtime_error(std::string(__func__)
                                     + ": Failed to open file when reading binary files: " + filename);
        }

        binary_file.unsetf(std::ios::skipws);

        binary_list.emplace_back();
        std::vector<unsigned char>& binary_data = binary_list.back();
        binary_file.seekg(0, binary_file.end);
        size_t filesize = binary_file.tellg();
        binary_file.seekg(0, binary_file.beg);

        binary_data.reserve(filesize);

        binary_data.insert(binary_data.begin(), std::istream_iterator<unsigned char>(binary_file),
                           std::istream_iterator<unsigned char>());
    }

    return binary_list;
}

void LogOpenCLError(const Worker& worker,
                    std::string function_signature,
                    std::string error_message,
                    cl_int cl_error_code)
{
    std::string message = error_message + " (" + OpenCLErrorName(cl_error_code) + ")";

#ifdef OPENCL_DEBUG_TO_STDERR
    std::cerr << function_signature << ": " << error_message << std::endl;
#endif

    worker.logger().LogWorkerEvent(worker, function_signature, Logger::WorkloadEvent::Error,
                                   -1, // Need to expose something like get_current_work_id()
                                   message);
}

void LogOpenCLError(const Worker& worker, std::string function_signature, std::string error_message)
{
#ifdef OPENCL_DEBUG_TO_STDERR
    std::cerr << function_signature << ": " << error_message << std::endl;
#endif

    worker.logger().LogWorkerEvent(worker, function_signature, Logger::WorkloadEvent::Error, -1, error_message, -1);
}

void LogOpenCLMessage(const Worker& worker, std::string function_signature, std::string message)
{
    worker.logger().LogWorkerEvent(worker, function_signature, Logger::WorkloadEvent::MESSAGE, -1, message, -1);
}

} // namespace Re::OpenCL