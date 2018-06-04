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
#include "aocl_utils.h"
#include "fft_config.h"

#include "../opencl_worker.h"

// the above header defines log of the FFT size hardcoded in the kernel
// compute N as 2^LOGN
#define N (1 << LOGN)

using namespace aocl_utils;

#define STRING_BUFFER_LEN 1024

Component component("FpgaTestComponent");
Worker worker(component, "testWorker", "testWorker");

// ACL runtime configuration
//static cl_platform_id platform = NULL;
//static std::vector<cl::Platform> platforms;
//static cl_device_id device = NULL;
//static OpenCLManager* manager = NULL;
//static cl_command_queue queue = NULL;
//static cl_command_queue queue1 = NULL;
//static cl_kernel kernel = NULL;
//static cl_kernel kernel1 = NULL;
//static cl_program program = NULL;
//static cl_int status = 0;

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
//bool InitFFT();
//bool CleanupFFT();
static void bit_reverse(float2* src, float2* dst, size_t num_floats);
static void test_fft(int iterations, bool inverse);
static int coord(int iteration, int i);
static void fourier_transform_gold(bool inverse, int lognr_points, double2 * data);
static void fourier_stage(int lognr_points, double2 * data);

// Host memory buffers
//float2 *h_inData, *h_outData;
//double2 *h_verify;

// Device memory buffers
//cl_mem d_inData, d_outData;

/*bool OpenCL_Worker::InitFFT() {
	return InitFFT();
}*/

/*bool OpenCL_Worker::CleanupFFT() {
	return CleanupFFT();
}*/

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

bool OpenCL_Worker::FFT(std::vector<float> &data) {
  cl_int status = 0;

	if (data.size() != N*2) {
         Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(),
                 "FFT implementation for FPGA requires data of length "+std::to_string(N)+", skipping calculation");
         return false;
    }
    
    size_t data_size = sizeof(float2) * data.size() / 2;
	float2* h_inData = (float2 *)alignedMalloc(data_size);
    memset(h_inData, 0, data_size);
	float2* h_outData =  (float2 *)alignedMalloc(data_size);//h_inData;
    //h_verify = (double2 *)alignedMalloc(data_size);
	memcpy(h_inData, data.data(), data_size);
	
  // Create device buffers - assign the buffers in different banks for more efficient
  // memory access 
  cl_mem d_inData = clCreateBuffer(manager_->GetContext()(), CL_MEM_READ_WRITE, data_size, NULL, &status);
  checkError(status, "Failed to allocate input device buffer\n");
  cl_mem d_outData = clCreateBuffer(manager_->GetContext()(), CL_MEM_READ_WRITE | CL_MEM_BANK_2_ALTERA, data_size, NULL, &status);
  checkError(status, "Failed to allocate output device buffer\n");

  auto& device = manager_->GetDevices(*this).at(0);
  auto& send_queue = device->GetQueue().GetRef()();

  // Copy data from host to device
  status = clEnqueueWriteBuffer(send_queue, d_inData, CL_TRUE, 0, data_size, h_inData, 0, NULL, NULL);
  checkError(status, "Failed to copy data to device");

  // Can't pass bool to device, so convert it to int
  int inverse_int = 0; //false
  int iterations = 1;

  // Set the kernel arguments
  status = clSetKernelArg(fpga_fetch_kernel_->GetBackingRef()(), 0, sizeof(cl_mem), (void *)&d_inData);
  checkError(status, "Failed to set kernel arg 0");

  status = clSetKernelArg(fpga_fft_kernel_->GetBackingRef()(), 0, sizeof(cl_mem), (void *)&d_outData);
  checkError(status, "Failed to set kernel arg 0");
  status = clSetKernelArg(fpga_fft_kernel_->GetBackingRef()(), 1, sizeof(cl_int), (void*)&iterations);
  checkError(status, "Failed to set kernel arg 1");
  status = clSetKernelArg(fpga_fft_kernel_->GetBackingRef()(), 2, sizeof(cl_int), (void*)&inverse_int);
  checkError(status, "Failed to set kernel arg 2");

  // Queue the FFT task
  status = clEnqueueTask(send_queue, fpga_fft_kernel_->GetBackingRef()(), 0, NULL, NULL);
  checkError(status, "Failed to launch kernel");

  size_t ls = N/8;
  size_t gs = iterations * ls;
  status = clEnqueueNDRangeKernel(fetch_queue->GetRef()(), fpga_fetch_kernel_->GetBackingRef()(), 1, NULL, &gs, &ls, 0, NULL, NULL);
  checkError(status, "Failed to launch fetch kernel");
 
  // Wait for command queue to complete pending events
  status = clFinish(send_queue);
  checkError(status, "Failed to finish");
  status = clFinish(fetch_queue->GetRef()());
  checkError(status, "Failed to finish queue1");

  // Copy results from device to host
  status = clEnqueueReadBuffer(send_queue, d_outData, CL_TRUE, 0, data_size, h_outData, 0, NULL, NULL);
  checkError(status, "Failed to copy data from device");

	//test_fft(1, false);
	//memcpy(data.data(), h_outData, data_size);
    bit_reverse(h_outData, (float2*)data.data(), data.size()/2);

    clReleaseMemObject(d_inData);
    clReleaseMemObject(d_outData);
    alignedFree(h_inData);
    alignedFree(h_outData);
    h_inData = NULL;
    h_outData = NULL;

	return true;
}

#define PI 3.1415926535897932384

// Entry point.
int main(int argc, char **argv) {

	Component component("testComp");
	OpenCL_Worker worker(component, "ocl_worker");
    
    worker.GetAttribute("platform_id").lock()->set_Integer(0);
    worker.GetAttribute("device_id").lock()->set_Integer(0);

	bool did_configure = worker.Configure();
    if (!did_configure) {
        std::cerr << "Failed to configure opencl worker for fft testing" << std::endl;
        return false;
    }

    float frequency = 13;
    float amplitude = 1;
	std::vector<float> fft_data(N*2, 0);

    for (unsigned int i=0; i<N; i++) {
        fft_data[i*2] = amplitude * (float)(cos((float)frequency*((float)i/(float)N)*2*PI));
    } 

    std::cout << "about to do the FFT" << std::endl;

	bool performed_fft = worker.FFT(fft_data);
	if (performed_fft) {
		std::cout << "Success!!!" << std::endl;
	}

    std::cout << "just after FFT" << std::endl;

    for (unsigned int i=0; i<N; i++) {
        if (fft_data[i*2] < 5) continue;
        
        std::cout << i << ": " << fft_data[i*2] << ", " << fft_data[i*2+1] << std::endl;
    }
    
    for (unsigned int i=0; i<N; i++) {
        if (fft_data[i*2+1] < 5) continue;
        
        std::cout << i << ": " << fft_data[i*2] << ", " << fft_data[i*2+1] << std::endl;
    }

	return false;

  /*if(!InitFFT()) {
    return false;
  }*/
  int iterations = 2000;

  // Allocate host memory
  /*h_inData = (float2 *)alignedMalloc(sizeof(float2) * N * iterations);
  h_outData = (float2 *)alignedMalloc(sizeof(float2) * N * iterations);
  h_verify = (double2 *)alignedMalloc(sizeof(double2) * N * iterations);
  if (!(h_inData && h_outData && h_verify)) {
    printf("ERROR: Couldn't create host buffers\n");
    return false;
  }*/

  test_fft(iterations, false); // test FFT transform running a sequence of iterations x 4k points transforms
  test_fft(iterations, true); // test inverse FFT transform - same setup as above

  // Free the resources allocated
  //CleanupFFT();

  return 0;
}

/*
void test_fft(int iterations, bool inverse) {
  printf("Launching");
  if (inverse) 
	printf(" inverse");
  printf(" FFT transform for %d iterations\n", iterations);

  // Initialize input and produce verification data
  for (int i = 0; i < iterations; i++) {
    for (int j = 0; j < N; j++) {
      h_verify[coord(i, j)].x = h_inData[coord(i, j)].x = (float)((double)rand() / (double)RAND_MAX);
      h_verify[coord(i, j)].y = h_inData[coord(i, j)].y = (float)((double)rand() / (double)RAND_MAX);
    }
  }

  // Create device buffers - assign the buffers in different banks for more efficient
  // memory access 
 
  d_inData = clCreateBuffer(manager->GetContext()(), CL_MEM_READ_WRITE, sizeof(float2) * N * iterations, NULL, &status);
  checkError(status, "Failed to allocate input device buffer\n");
  d_outData = clCreateBuffer(manager->GetContext()(), CL_MEM_READ_WRITE | CL_MEM_BANK_2_ALTERA, sizeof(float2) * N * iterations, NULL, &status);
  checkError(status, "Failed to allocate output device buffer\n");

  // Copy data from host to device

  status = clEnqueueWriteBuffer(queue, d_inData, CL_TRUE, 0, sizeof(float2) * N * iterations, h_inData, 0, NULL, NULL);
  checkError(status, "Failed to copy data to device");

  // Can't pass bool to device, so convert it to int
  int inverse_int = inverse;

  // Set the kernel arguments
  status = clSetKernelArg(kernel1, 0, sizeof(cl_mem), (void *)&d_inData);
  checkError(status, "Failed to set kernel arg 0");
  
  status = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&d_outData);
  checkError(status, "Failed to set kernel arg 0");
  status = clSetKernelArg(kernel, 1, sizeof(cl_int), (void*)&iterations);
  checkError(status, "Failed to set kernel arg 1");
  status = clSetKernelArg(kernel, 2, sizeof(cl_int), (void*)&inverse_int);
  checkError(status, "Failed to set kernel arg 2");

  printf(inverse ? "\tInverse FFT" : "\tFFT");
  printf(" kernel initialization is complete.\n");

  // Get the iterationstamp to evaluate performance
  double time = getCurrentTimestamp();

  // Launch the kernel - we launch a single work item hence enqueue a task
  status = clEnqueueTask(queue, kernel, 0, NULL, NULL);
  checkError(status, "Failed to launch kernel");

  std::cout << "run kernel, about to enqueue kernel1" << std::endl;

  size_t ls = N/8;
  size_t gs = iterations * ls;
  status = clEnqueueNDRangeKernel(queue1, kernel1, 1, NULL, &gs, &ls, 0, NULL, NULL);
  checkError(status, "Failed to launch fetch kernel");
 
  std::cout << "done gone ran the second kernel (kernel1)" << std::endl;
 
  // Wait for command queue to complete pending events
  status = clFinish(queue);
  checkError(status, "Failed to finish");
  //status = clFinish(queue1);
  //checkError(status, "Failed to finish queue1");
  
  std::cout << "and the queue finished as well" << std::endl;

  // Record execution time
  time = getCurrentTimestamp() - time;

  // Copy results from device to host
  status = clEnqueueReadBuffer(queue, d_outData, CL_TRUE, 0, sizeof(float2) * N * iterations, h_outData, 0, NULL, NULL);
  checkError(status, "Failed to copy data from device");

  printf("\tProcessing time = %.4fms\n", (float)(time * 1E3));
  double gpoints_per_sec = ((double) iterations * N / time) * 1E-9;
  double gflops = 5 * N * (log((float)N)/log((float)2))/(time / iterations * 1E9);
  printf("\tThroughput = %.4f Gpoints / sec (%.4f Gflops)\n", gpoints_per_sec, gflops);

  // Pick randomly a few iterations and check SNR

  double fpga_snr = 200;
  for (int i = 0; i < iterations; i+= rand() % 20 + 1) {
    fourier_transform_gold(inverse, LOGN, h_verify + coord(i, 0));
    double mag_sum = 0;
    double noise_sum = 0;
    for (int j = 0; j < N; j++) {
      double magnitude = (double)h_verify[coord(i, j)].x * (double)h_verify[coord(i, j)].x +  
                              (double)h_verify[coord(i, j)].y * (double)h_verify[coord(i, j)].y;
      double noise = (h_verify[coord(i, j)].x - (double)h_outData[coord(i, j)].x) * (h_verify[coord(i, j)].x - (double)h_outData[coord(i, j)].x) +  
                          (h_verify[coord(i, j)].y - (double)h_outData[coord(i, j)].y) * (h_verify[coord(i, j)].y - (double)h_outData[coord(i, j)].y);

      mag_sum += magnitude;
      noise_sum += noise;
    }
    double db = 10 * log(mag_sum / noise_sum) / log(10.0);
    // find minimum SNR across all iterations
    if (db < fpga_snr) fpga_snr = db;
  }
  printf("\tSignal to noise ratio on output sample: %f --> %s\n\n", fpga_snr, fpga_snr > 125 ? "PASSED" : "FAILED");
}
*/

/////// HELPER FUNCTIONS ///////

// provides a linear offset in the input array
int coord(int iteration, int i) {
  return iteration * N + i;
}


// Reference Fourier transform
void fourier_transform_gold(bool inverse, const int lognr_points, double2 *data) {
   const int nr_points = 1 << lognr_points;

   // The inverse requires swapping the real and imaginary component
   
   if (inverse) {
      for (int i = 0; i < nr_points; i++) {
         double tmp = data[i].x;
         data[i].x = data[i].y;
         data[i].y = tmp;;
      }
   }
   // Do a FT recursively
   fourier_stage(lognr_points, data);

   // The inverse requires swapping the real and imaginary component
   if (inverse) {
      for (int i = 0; i < nr_points; i++) {
         double tmp = data[i].x;
         data[i].x = data[i].y;
         data[i].y = tmp;;
      }
   }

   // Do the bit reversal

   double2 *temp = (double2 *)alloca(sizeof(double2) * nr_points);
   for (int i = 0; i < nr_points; i++) temp[i] = data[i];
   for (int i = 0; i < nr_points; i++) {
      int fwd = i;
      int bit_rev = 0;
      for (int j = 0; j < lognr_points; j++) {
         bit_rev <<= 1;
         bit_rev |= fwd & 1;
         fwd >>= 1;
      }
      data[i] = temp[bit_rev];
   }
}

void fourier_stage(int lognr_points, double2 *data) {
   int nr_points = 1 << lognr_points;
   if (nr_points == 1) return;
   double2 *half1 = (double2 *)alloca(sizeof(double2) * nr_points / 2);
   double2 *half2 = (double2 *)alloca(sizeof(double2) * nr_points / 2);
   for (int i = 0; i < nr_points / 2; i++) {
      half1[i] = data[2 * i];
      half2[i] = data[2 * i + 1];
   }
   fourier_stage(lognr_points - 1, half1);
   fourier_stage(lognr_points - 1, half2);
   for (int i = 0; i < nr_points / 2; i++) {
      data[i].x = half1[i].x + cos (2 * M_PI * i / nr_points) * half2[i].x + sin (2 * M_PI * i / nr_points) * half2[i].y;
      data[i].y = half1[i].y - sin (2 * M_PI * i / nr_points) * half2[i].x + cos (2 * M_PI * i / nr_points) * half2[i].y;
      data[i + nr_points / 2].x = half1[i].x - cos (2 * M_PI * i / nr_points) * half2[i].x - sin (2 * M_PI * i / nr_points) * half2[i].y;
      data[i + nr_points / 2].y = half1[i].y + sin (2 * M_PI * i / nr_points) * half2[i].x - cos (2 * M_PI * i / nr_points) * half2[i].y;
   }
}

bool OpenCL_Worker::InitFFT() {
  cl_int status;

  /*if(!setCwdToExeDir()) {
    return false;
  }*/

  // Get the OpenCL platform.
  // /manager = manager_;
//  OpenCLManager* manager = OpenCLManager::GetReferenceByPlatformName(worker, "Intel(R) FPGA SDK for OpenCL(TM)");
  //std::vector<cl::Platform> platforms = OpenCLManager::GetPlatforms(worker);
  //cl::Platform* platform = NULL;
  //for(auto& pform : platforms) {
  //  if (pform
  //}
  //platform = findPlatform("Intel(R) FPGA");
  //if(platform == NULL) {
  //  printf("ERROR: Unable to find Intel(R) FPGA OpenCL platform\n");
  //  return false;
  //}

  // Query the available OpenCL devices.
  //scoped_array<cl_device_id> devices;
  //cl_uint num_devices;

  //devices.reset(getDevices(platform, CL_DEVICE_TYPE_ALL, &num_devices));
  auto& devices = manager_->GetDevices(worker);

  // We'll just use the first device.
  auto& device = devices.at(0);

  // Create the context.
  //auto call = (void(_stdcall*) (const char *, const void*, size_t, void *)) &oclContextCallback; 
  //context = clCreateContext(NULL, 1, &device, call, NULL, &status);
  //context = clCreateContext(NULL, 1, &(device.GetRef()()), &oclContextCallback, NULL, &status);
  //checkError(status, "Failed to create context");

  // Create the command queue.
  //queue = clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &status);
  //queue = device->GetQueue().GetRef()();
  //checkError(status, "Failed to create command queue");
  //queue1 = clCreateCommandQueue(manager_->GetContext()(), device->GetRef()(), CL_QUEUE_PROFILING_ENABLE, &status);
  fetch_queue = new OpenCLQueue(*manager_, *device);
  checkError(status, "Failed to create command queue");

    int kernels_found=0;
    for (auto& ref_wrapper : device->GetKernels()){
        auto& kernel_ = ref_wrapper.get();
        if (kernel_.GetName() == "fft1d" || kernel_.GetName() == "fetch") {
            kernels_found++;
        }
    }

  // Create the program.
  //std::string binary_file = getBoardBinaryFile("fft1d", device);
  if (kernels_found < 2) {
    bool did_read_binary = device->LoadKernelsFromBinary(worker, "fft1d.aocx");
  }
  //printf("Using AOCX: %s\n\n", binary_file.c_str());
  //program = createProgramFromBinary(context, binary_file.c_str(), &device, 1);

  // Build the program that was just created.
  //status = clBuildProgram(program, 0, NULL, "", NULL, NULL);
  //checkError(status, "Failed to build program");

  // Create the kernel - name passed in here must match kernel name in the
  // original CL file, that was compiled into an AOCX file using the AOC tool
  auto& kernels = device->GetKernels();
  for (OpenCLKernel& k : kernels) {
    //auto& kernel_ = ref_wrapper.get();
    if (k.GetName() == "fft1d") {
      fpga_fft_kernel_ = &k;
      //kernel = k.GetBackingRef()();
    }
    if (k.GetName() == "fetch") {
      fpga_fetch_kernel_ = &k;
      //kernel1 = k.GetBackingRef()();
    }
  }
  //kernel = clCreateKernel(program, "fft1d", &status);
  //checkError(status, "Failed to create kernel");

  //kernel1 = clCreateKernel(program, "fetch", &status);
  //checkError(status, "Failed to create fetch kernel");

  return true;
}

// Free the resources allocated during initialization
bool OpenCL_Worker::CleanupFFT() {
    
  if(fetch_queue) 
    delete fetch_queue;
    //clReleaseCommandQueue(queue1);
  /*if (h_verify) {
	  alignedFree(h_verify);
  }*/
  /*if (d_inData) {      
	clReleaseMemObject(d_inData);
    d_inData = NULL;
  }
  if (d_outData) {
	clReleaseMemObject(d_outData);
    d_outData = NULL;
  }*/

  return true;
}



