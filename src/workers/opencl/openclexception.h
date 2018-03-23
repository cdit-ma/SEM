#ifndef OPENCLEXCEPTION_H
#define OPENCLEXCEPTION_H

#include <stdexcept>
#include <sstream>

class OpenCLException : public std::runtime_error {
public:
    OpenCLException(std::string message, int opencl_error_code);

    const char* what() const noexcept;
    int ErrorCode() const;

    friend std::ostream& operator<<(std::ostream& ostream, const OpenCLException& ocle) {
        ostream << ocle.what();
        return ostream;
    }

private:
    int error_code;
};

#endif
