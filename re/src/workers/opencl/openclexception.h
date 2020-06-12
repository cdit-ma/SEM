#ifndef RE_OPENCL_EXCEPTION_H
#define RE_OPENCL_EXCEPTION_H

#include <sstream>
#include <stdexcept>

namespace Re {

namespace OpenCL {

class OpenCLException : public std::runtime_error {
    public:
    OpenCLException(std::string message, int opencl_error_code);

    const char* what() const noexcept;
    int ErrorCode() const;

    friend std::ostream& operator<<(std::ostream& ostream, const OpenCLException& ocle)
    {
        ostream << ocle.what();
        return ostream;
    }

    private:
    int error_code;
};

} // namespace OpenCL

} // namespace Re

#endif // RE_OPENCL_EXCEPTION_H
