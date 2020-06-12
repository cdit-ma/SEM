#include "openclexception.h"
#include "utilities.h"

using namespace Re::OpenCL;

OpenCLException::OpenCLException(std::string message, int opencl_error_code) :
    std::runtime_error(message),
    error_code(opencl_error_code){};

const char* OpenCLException::what() const noexcept
{
    std::string message = "[";
    message += ErrorCode();
    message += ": ";
    message += OpenCLErrorName(ErrorCode());
    message += "] ";
    message += std::runtime_error::what();
    return message.c_str();
}

int OpenCLException::ErrorCode() const { return error_code; }