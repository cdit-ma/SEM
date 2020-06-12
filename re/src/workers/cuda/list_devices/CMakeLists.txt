

add_executable(list_cuda_devices "")

set_target_properties(list_cuda_devices
        PROPERTIES
            CUDA_SEPARABLE_COMPILATION ON
            POSITION_INDEPENDENT_CODE ON
            CUDA_STANDARD 14)

target_link_libraries(list_cuda_devices
        PUBLIC
            CUDA::cudart
            cuda-api-wrappers)

target_sources(list_cuda_devices
        PRIVATE
            listdevices.cpp
            jsonwriter.cpp
            jsonwriter.h
)
