add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/cpu")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/utility")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/opencl")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/memory")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dis")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/tcp")
add_subdirectory(fft_accel)

include(CheckLanguage)
check_language(CUDA)
if (CMAKE_CUDA_COMPILER)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/cuda")
endif()