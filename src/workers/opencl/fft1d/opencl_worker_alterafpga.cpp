/////////////////////////
// Adapted from the fft1d project in the Intel FPGA OpenCL Design Examples by Jackson Michael
// See comments below for original licensing information
/////////////////////////

// Copyright (C) 2013-2016 Altera Corporation, San Jose, California, USA. All rights reserved.
// Permission is hereby granted, free of charge, to any person obtaining a copy of this
// software and associated documentation files (the "Software"), to deal in the Software
// without restriction, including without limitation the rights to use, copy, modify, merge,
// publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to
// whom the Software is furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
// 
// This agreement shall be governed in all respects by the laws of the State of California and
// by the laws of the United States of America.

///////////////////////////////////////////////////////////////////////////////////
// This OpenCL application executs a 1D FFT transform on an Altera FPGA.
// The Altera Offline Compiler tool ('aoc') compiles the kernel source into a 'fft1d.aocx' 
// file containing a hardware programming image for the FPGA. The host program 
// provides the contents of the .aocx file to the clCreateProgramWithBinary OpenCL
// API for runtime programming of the FPGA.
//
// When compiling this application, ensure that the Intel(R) FPGA SDK for OpenCL(TM)
// is properly installed.
///////////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <cstring>
#include <iostream>
#include <limits.h>

#include "../openclutilities.h"
#include "../openclmanager.h"
#include "../opencl_worker.h"

#include "aocl_utils.h"
#include "fft_config.h"

// the above header defines log of the FFT size hardcoded in the kernel
// compute N as 2^LOGN
#define N (1 << LOGN)

//using namespace aocl_utils;

// FFT operates with complex numbers - store them in a struct
typedef struct {
    double x;
    double y;
} double2;

typedef struct {
	float x;
	float y;
} float2;

// Function prototypes
static void bit_reverse(float2* src, float2* dst, size_t num_floats);

/**
 * Re-arrange the elements of an array so that they are in bit reversed order:
 * [0,1,2,3,4,5,6,7] -> [0,4,2,6,1,5,3,7] where eg 3 = 011 -> 110 = 6
 **/
void bit_reverse(float2* src, float2* dst, size_t num_elements) {
    int bit_length = LOGN - 1;
    
    /* Re-enable for dynamically sized reversal
    int bit_length = 0;
    while (1 << bit_length < num_elements) bit_length++;
    bit_length -= 1;
    */
   
    for (unsigned int index=0; index<num_elements; index++) {
        unsigned int norm_index = index;
        unsigned int br_index = index & 1;
        //int s = sizeof(norm_index) * CHAR_BIT - 1;
        int s = bit_length;
        for (norm_index >>= 1; norm_index; norm_index >>= 1) {
            br_index <<= 1;
            br_index |= (norm_index &  1);
            s--;
        }
        br_index <<= s;
        
        dst[br_index].x = src[index].x;
        dst[br_index].y = src[index].y;
    }
}

/**
 * Performs a Fast Fourier Transform on the provided data, overwriting the input with the result
 * Takes in a vector of length 8192, containing 4096 complex numbers in interleaved format:
 * [r,i,r,i,r,i,...]
 **/
bool OpenCL_Worker::FFT(std::vector<float> &data) {
	cl_int status = 0;

	// Altera implementation requires exactly N complex numbers
	if (data.size() != N*2) {
         Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(),
                 "FFT implementation for FPGA requires data of length "+std::to_string(N)+", skipping calculation");
         return false;
    }
    
    size_t data_size = sizeof(float2) * data.size() / 2;
	float2* h_inData = (float2 *)aocl_utils::alignedMalloc(data_size);

	// For 0 padding if sizes < N are to be supported with fixed values of LOGN
    memset(h_inData, 0, data_size);
	
	float2* h_outData =  (float2 *)aocl_utils::alignedMalloc(data_size);
	memcpy(h_inData, data.data(), data_size);
	
	// Create device buffers - assign the buffers in different banks for more efficient
	// memory access 
	cl::Buffer d_inData(manager_->GetContext(), CL_MEM_READ_WRITE, data_size, NULL, &status);
	aocl_utils::checkError(status, "Failed to allocate input device buffer\n");
	cl::Buffer d_outData(manager_->GetContext(), CL_MEM_READ_WRITE | CL_MEM_BANK_2_ALTERA, data_size, NULL, &status);
	aocl_utils::checkError(status, "Failed to allocate output device buffer\n");

	// Ask the load balancer for the most available device
	const auto& device_index = load_balancer_->RequestDevice();

	auto& device = devices_.at(device_index).get();
	auto& send_queue = device.GetQueue().GetRef();
	auto& fetch_queue = fetch_queues_.at(device_index);

	// Copy data from host to device
	status = send_queue.enqueueWriteBuffer(d_inData, CL_TRUE, 0, data_size, h_inData, NULL, NULL);
	aocl_utils::checkError(status, "Failed to copy data to device");

	// Can't pass bool to device, so convert it to int
	int inverse_int = 0; //false
	int iterations = 1;

	// Take ownership of kernels associated with FFTs on that device
	auto fft_kernel_lock = fpga_fft_kernel_->AcquireLock();
	auto fetch_kernel_lock = fpga_fetch_kernel_->AcquireLock();

	// Set the kernel arguments
	fpga_fetch_kernel_->SetArgs(d_inData);
	fpga_fft_kernel_->SetArgs(d_outData, iterations, inverse_int);

	// Queue the FFT task
	status = send_queue.enqueueTask(fpga_fft_kernel_->GetBackingRef(), NULL, NULL);
	aocl_utils::checkError(status, "Failed to launch kernel");

	size_t ls = N/8;
	cl::NDRange fetch_local_range(ls);
	size_t gs = iterations * ls;
	cl::NDRange fetch_global_range(gs);
	status = fetch_queue.GetRef().enqueueNDRangeKernel(fpga_fetch_kernel_->GetBackingRef(),
													cl::NullRange, fetch_global_range, fetch_local_range);
	aocl_utils::checkError(status, "Failed to launch fetch kernel");

	// Wait for command queue to complete pending events
	send_queue.finish();
	aocl_utils::checkError(status, "Failed to finish");

	status = fetch_queue.GetRef().finish();
	aocl_utils::checkError(status, "Failed to finish queue1");

	fft_kernel_lock.unlock();
	fetch_kernel_lock.unlock();

	// Copy results from device to host
	status = send_queue.enqueueReadBuffer(d_outData, CL_TRUE, 0, data_size, h_outData, NULL, NULL);
	aocl_utils::checkError(status, "Failed to copy data from device");
  
	// Altera FPGA implementation leaves out the bit reversal, so it needs to be performed now on the CPU
    bit_reverse(h_outData, (float2*)data.data(), data.size()/2);

    aocl_utils::alignedFree(h_inData);
    aocl_utils::alignedFree(h_outData);
    h_inData = NULL;
    h_outData = NULL;

	return true;
}

bool OpenCL_Worker::InitFFT() {
    cl_int status;

	// Loop through all of the devices available to the worker for initialisation
	for (auto& d_ref : devices_) {
		auto& device = d_ref.get();

		// Create the fetch queue.
		fetch_queues_.emplace_back(*manager_, device);

		// Check if both required fft1d and fetch kernels are in the list of available kernels
		int kernels_found=0;
		for (auto& ref_wrapper : device.GetKernels()){
			auto& kernel_ = ref_wrapper.get();
			if (kernel_.GetName() == "fft1d" || kernel_.GetName() == "fetch") {
				kernels_found++;
			}
		}

		// Recreate the program from a binary if either kernel isn't present
		if (kernels_found < 2) {
			bool did_read_binary = device.LoadKernelsFromBinary(*this, "fft1d.aocx");
			if (!did_read_binary) {
				Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(),
						"Unable to load kernels from fft1d.aocx binary");
				return false;
			}
		}

		// Create the kernel - name passed in here must match kernel name in the
		// original CL file, that was compiled into an AOCX file using the AOC tool
		auto& kernels = device.GetKernels();
		for (OpenCLKernel& k : kernels) {
			if (k.GetName() == "fft1d") {
				fpga_fft_kernel_ = &k;
			}
			if (k.GetName() == "fetch") {
				fpga_fetch_kernel_ = &k;
			}
		}

	}

	return true;
}

// Free the resources allocated during initialization
bool OpenCL_Worker::CleanupFFT() {

	/*if(fetch_queue) 
		delete fetch_queue;*/

	return true;
}
