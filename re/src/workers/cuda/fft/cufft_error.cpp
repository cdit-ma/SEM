//
// Created by Jackson Michael on 6/5/20.
//

#include "cufft_error.h"

#include <iostream>

namespace cditma::cuda::cufft {

void throw_on_error(cufftResult result_value)
{
    switch(result_value) {
        case CUFFT_SUCCESS:
            return;
        case CUFFT_INVALID_PLAN:
            throw invalid_plan();
        case CUFFT_ALLOC_FAILED:
            throw allocation_failed();
        case CUFFT_INVALID_TYPE:
            throw invalid_type();
        case CUFFT_INVALID_VALUE:
            throw invalid_value();
        case CUFFT_INTERNAL_ERROR:
            throw internal_error();
        case CUFFT_EXEC_FAILED:
            throw exec_failed();
        case CUFFT_SETUP_FAILED:
            throw setup_failed();
        case CUFFT_INVALID_SIZE:
            throw invalid_size();
        case CUFFT_UNALIGNED_DATA:
            throw unaligned_data();
        case CUFFT_INCOMPLETE_PARAMETER_LIST:
            throw incomplete_parameter_list();
        case CUFFT_INVALID_DEVICE:
            throw invalid_device();
        case CUFFT_PARSE_ERROR:
            throw parse_error();
        case CUFFT_NO_WORKSPACE:
            throw no_workspace();
        case CUFFT_NOT_IMPLEMENTED:
            throw not_implemented();
        case CUFFT_LICENSE_ERROR:
            throw license_error();
        case CUFFT_NOT_SUPPORTED:
            throw not_supported();
    }
}

} // namespace cditma::cuda::cufft