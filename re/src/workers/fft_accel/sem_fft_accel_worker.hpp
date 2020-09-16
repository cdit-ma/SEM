//
// Created by Jackson Michael on 19/8/20.
//

#ifndef SEM_SEM_FFT_ACCEL_WORKER_H
#define SEM_SEM_FFT_ACCEL_WORKER_H

#include "worker.hpp"
#include "network/udp_adapter.hpp"

// Expose the worker class to the global namespace as codegen doesn't currently support namespacing
using FFTAccelWorker = sem::fft_accel::worker<
        sem::fft_accel::network::udp_adapter<float>
        >;

#endif //SEM_SEM_FFT_ACCEL_WORKER_H
