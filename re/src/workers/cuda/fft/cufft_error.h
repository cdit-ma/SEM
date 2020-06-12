//
// Created by Jackson Michael on 6/5/20.
//

#ifndef RE_CUFFT_ERROR_H
#define RE_CUFFT_ERROR_H

#include <cufft.h>
#include <type_traits>
#include <utility>

#include <stdexcept>

namespace cditma {
namespace cuda {
namespace cufft {

/**
 * Takes in the result of any CUFFT call and throws the appropriate specialised exception for the given error
 * @param result_value A cufftResult value returned by and CUFFT call
 */
extern void throw_on_error(cufftResult result_value);

class exception : public std::exception {
public:
    /**
     * Retrieves the error code that resulted in the given exception being thrown
     * @return
     */
    virtual cufftResult get_code() const = 0;
};

class invalid_plan : public exception {
public:
    cufftResult get_code() const override { return code; };
    const char* what() const noexcept override
    {
        return "cuFFT was passed an invalid plan handle";
    }

private:
    static constexpr cufftResult code = CUFFT_INVALID_PLAN;
};

class allocation_failed : public exception {
public:
    cufftResult get_code() const override { return code; };
    const char* what() const noexcept override
    {
        return "cuFFT failed to allocate GPU or CPU memory";
    }

private:
    static constexpr cufftResult code = CUFFT_ALLOC_FAILED;
};

class invalid_type : public exception {
public:
    cufftResult get_code() const override { return code; };
    const char* what() const noexcept override { return "No longer used"; }

private:
    static constexpr cufftResult code = CUFFT_INVALID_TYPE;
};

class invalid_value : public exception {
public:
    cufftResult get_code() const override { return code; };
    const char* what() const noexcept override
    {
        return "User specified an invalid pointer or parameter";
    }

private:
    static constexpr cufftResult code = CUFFT_INVALID_VALUE;
};

class internal_error : public exception {
public:
    cufftResult get_code() const override { return code; };
    const char* what() const noexcept override
    {
        return "Driver or internal cuFFT library error";
    }

private:
    static constexpr cufftResult code = CUFFT_INTERNAL_ERROR;
};

class exec_failed : public exception {
public:
    cufftResult get_code() const override { return code; };
    const char* what() const noexcept override
    {
        return "Failed to execute an FFT on the GPU";
    }

private:
    static constexpr cufftResult code = CUFFT_EXEC_FAILED;
};

class setup_failed : public exception {
public:
    cufftResult get_code() const override { return code; };
    const char* what() const noexcept override
    {
        return "The cuFFT library failed to initialize";
    }

private:
    static constexpr cufftResult code = CUFFT_SETUP_FAILED;
};

class invalid_size : public exception {
public:
    cufftResult get_code() const override { return code; };
    const char* what() const noexcept override
    {
        return "User specified an invalid transform size";
    }

private:
    static constexpr cufftResult code = CUFFT_INVALID_SIZE;
};

class unaligned_data : public exception {
public:
    cufftResult get_code() const override { return code; };
    const char* what() const noexcept override { return "No longer used"; }

private:
    static constexpr cufftResult code = CUFFT_UNALIGNED_DATA;
};

class incomplete_parameter_list : public exception {
public:
    cufftResult get_code() const override { return code; };
    const char* what() const noexcept override { return "Missing parameters in call"; }

private:
    static constexpr cufftResult code = CUFFT_INCOMPLETE_PARAMETER_LIST;
};

class invalid_device : public exception {
public:
    cufftResult get_code() const override { return code; };
    const char* what() const noexcept override
    {
        return "Execution of a plan was on different GPU than plan creation";
    }

private:
    static constexpr cufftResult code = CUFFT_INVALID_DEVICE;
};

class parse_error : public exception {
public:
    cufftResult get_code() const override { return code; };
    const char* what() const noexcept override { return "Internal plan database error"; }

private:
    static constexpr cufftResult code = CUFFT_PARSE_ERROR;
};

class no_workspace : public exception {
public:
    cufftResult get_code() const override { return code; };
    const char* what() const noexcept override
    {
        return "No workspace has been provided prior to plan execution";
    }

private:
    static constexpr cufftResult code = CUFFT_NO_WORKSPACE;
};

class not_implemented : public exception {
public:
    cufftResult get_code() const override { return code; };
    virtual const char* what() const noexcept override
    {
        return "Function does not implement functionality for parameters given.";
    }

private:
    static constexpr cufftResult code = CUFFT_NOT_IMPLEMENTED;
};

class license_error : public exception {
public:
    cufftResult get_code() const override { return code; };
    const char* what() const noexcept override { return "Used in previous versions"; }

private:
    static constexpr cufftResult code = CUFFT_LICENSE_ERROR;
};

class not_supported : public exception {
public:
    cufftResult get_code() const override { return code; };
    const char* what() const noexcept override
    {
        return "Operation is not supported for parameters given";
    }

private:
    static constexpr cufftResult code = CUFFT_NOT_SUPPORTED;
};

} // namespace cufft
} // namespace cuda
} // namespace cditma

#endif // RE_CUFFT_ERROR_H
