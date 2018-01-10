#ifndef OPENCLEXCEPTION_H
#define OPENCLEXCEPTION_H

#include <stdexcept>
#include <sstream>

#include "openclutilities.h"

class OpenCLException : public std::runtime_error {
public:
    OpenCLException(std::string message, int opencl_error_code) :
        std::runtime_error(message),
        error_code(opencl_error_code) {};

    const char* what() const noexcept {
        std::string message = "[";
        message += OpenCLErrorName(ErrorCode());
        message += "] ";
        message += std::runtime_error::what();
        return message.c_str();
    }

    int ErrorCode() const {
        return error_code;
    }

    friend std::ostream& operator<<(std::ostream& ostream, const OpenCLException& ocle) {
        ostream << ocle.what();
        return ostream;
    }

private:
    int error_code;
};

#endif