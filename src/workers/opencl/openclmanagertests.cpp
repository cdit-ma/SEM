
#define OPENCL_DEBUG_TO_STDERR

#include <iostream>
#include <vector>
#include <algorithm>

#include <math.h>
#include <limits>
#include <random>
#include <iomanip>
#include <chrono>

#include "openclmanager.h"
#include "openclkernel.hpp"
#include "opencl_worker.h"
#include "openclutilities.h"
#include "openclloadbalancer.h"

#define RANDSEED 13520

#define EPS 1e-5
#define CHECK_FLOAT(x, y, eps) (fabs(1-x/y)<eps)

bool compare_float(float x, float y, float epsilon) {
	if (x == y) return true;
	
	if (y == 0) return false;

	return fabs(1-x/y) < epsilon;
};

enum Result {
	UNKNOWN,
	PASS,
	FAIL,
	SKIPPED
};

std::string resultToString(Result res) {
	switch (res) {
	case UNKNOWN: return "UNKNOWN";
	case PASS: return "PASS";
	case FAIL: return "FAIL";
	case SKIPPED: return "SKIPPED";
	default: return "UNEXPECTED RESULT TYPE";
	}
}

void printPass(std::string description) {
	std::cout << /*"\033[5m" << */ "\033[32m" << "[PASS] " << "\033[0m";
	std::cout << description << std::endl;
}

void printFail(std::string description) {
	std::cout << "\033[31m" << "[FAIL] " << "\033[0m";
	std::cout << description << std::endl;
}

void printSkipped(std::string description) {
	std::cout << "\033[34m" << "[SKIPPED] " << "\033[0m";
	std::cout << description << std::endl;
}

void printWarning(std::string description) {
	std::cout << "\033[33m" << "[WARNING] " << "\033[0m";
	std::cout << description << std::endl;
}

void printInfo(std::string description) {
	std::cout << "\033[36m" << "[INFO] " << "\033[0m";
	std::cout << description << std::endl;
}

unsigned int total_tests=0;
unsigned int tests_passed=0;
unsigned int tests_failed=0;
unsigned int tests_skipped=0;
unsigned int tests_unknown=0;
void recordTest(Result result, std::string description) {
	total_tests++;
	switch (result) {
	case PASS:
		tests_passed++;
		printPass(description);
		break;
	case FAIL:
		tests_failed++;
		printFail(description);
		break;
	case SKIPPED:
		tests_skipped++;
		printSkipped(description);
		break;
	default:
		tests_unknown++;
		printWarning("Unknown test result! Description: "+description);
	}

	/*switch(verbosity) {
	case SILENT: return;
	case QUIET: if (result == PASS) return;
	default: break;
	}*/
	//std::cout << "[" << resultToString(result) << "] " << description << std::endl;
}

void testLoadBalancer(OpenCLManager& manager);

void testBufferReadWrite(OpenCLManager& manager, OpenCLDevice& device);
void testKernelPassthrough(OpenCLManager& manager, OpenCLDevice& device);

OpenCL_Worker* testWorkerConstruction(Component& component, int device_id);
void testWorkerDestruction(OpenCL_Worker& worker);
void testWorkerCreateBuffer(OpenCL_Worker& worker);
void testWorkerReleaseBuffer(OpenCL_Worker& worker);
void testWorkerBufferDataTransfers(OpenCL_Worker& worker);
void testWorkerRunParallel(OpenCL_Worker& worker);
void testWorkerMatrixMult(OpenCL_Worker& worker);
	void testWorkerSquareMult(OpenCL_Worker& worker, unsigned int length, bool expect_failure=false);
	void testWorkerRectMult(OpenCL_Worker& worker, unsigned int rowsA, unsigned int colsA,
			unsigned int rowsB, unsigned int colsB, bool expect_failure=false);
	Result checkMultiplication(float* matA, float* matB, float* matC,
								unsigned int m, unsigned int k, unsigned int n);

void testWorkerKMeans(OpenCL_Worker& worker);

int main(int argc, char** argv) {
	auto start_time = std::chrono::steady_clock::now();
	Result res = UNKNOWN;

	printInfo("Beginning OpenCL worker Tester...");

	Component test_component("testComponent");
	Worker test_worker(test_component, "opencl_worker", "testWorker");

	// List out the avaialble platforms
	auto platforms = OpenCLManager::GetPlatforms(test_worker);
	if (platforms.size() == 0) {
		res = FAIL;
	} else {
		res = PASS;
	}
	recordTest(res, "Check that at least one OpenCL platform is available");
	if (res == FAIL) {
		return 1;
	}

	printInfo("Listing OpenCL platforms:");
	for (auto platform : platforms) {
		std::cout << " - " << platform.getInfo<CL_PLATFORM_NAME>() << std::endl;
	}

	OpenCLManager* manager;
	try{
		manager = OpenCLManager::GetReferenceByPlatformID(test_worker, 0);
		res = PASS;
	} catch (const std::exception& e) {
		std::cerr << "Exception when obtaining manager reference: " << e.what() << std::endl;
		res = FAIL;
	}
	recordTest(res, "Retrieved first available OpenCLManager without an exception being thrown");

	if (manager != NULL) {
		res = PASS;
	} else {
		res = FAIL;
	}
	recordTest(res, "Attempt to retreive reference to first available OpenCLManager did not return null");
	if (manager->IsValid()) {
		res = PASS;
	}
	else {
		res = FAIL;
	}
	recordTest(res, "Reference to first available OpenCLManager reports itself as being valid");
	if (res == FAIL) {
		return 1;
	}

	printInfo("Listing available devices for platform:");
	for (const auto& device : manager->GetDevices(test_worker)) {
		std::cout << " - " << device->GetName() << std::endl;
		/*for (auto& kernel : device.GetKernels()) {
			std::cout << "   - " << kernel.get().GetName() << std::endl;
		}*/
	}

	/*
	// Generate the list of source files
	// For now, just add the "kernel" and "matrixmult" OpenCL files in the source directory
	std::string source_file_path(__FILE__);
	auto source_dir = source_file_path.substr(0, source_file_path.find_last_of("/\\")+1);
	std::vector<std::string> filenames = {
		source_dir + "kernel.cl",
		source_dir + "matrixmult.cl"
	};

	printInfo("Loading the following kernel source files:");
	for (auto fn : filenames) {
		std::cout << " - " << fn << std::endl;
	}

	// Generate the kernels for the provided source files
	auto kernels = manager->CreateKernels(filenames);
	if (kernels.size() == 0) {
		res = FAIL;
	} else {
		res = PASS;
	}
	recordTest(res, "Create kernels from OpenCL source code");

	if (kernels.size() != 0) {
		printInfo("Listing built kernels:");
		for (auto k : kernels) {
			std::cout << " - " << k.GetName() << std::endl;
		}
	}*/

	for (auto& device : manager->GetDevices(test_worker)) {
		testBufferReadWrite(*manager, *device);
	}
	
	bool firstTime = true;
	for (auto& device : manager->GetDevices(test_worker)) {
		if (firstTime) {
			firstTime = false;
		} else {
			continue;
		}
		testKernelPassthrough(*manager, *device);
	}

	testLoadBalancer(*manager);


	for (int i=0; i< manager->GetDevices(test_worker).size(); i++) {
		OpenCL_Worker* worker = testWorkerConstruction(test_component, i);

		// Run worker tests conditional on worker having been successfully constructed
		if (worker != NULL) {
			testWorkerCreateBuffer(*worker);
			testWorkerRunParallel(*worker);
			testWorkerMatrixMult(*worker);
			testWorkerKMeans(*worker);
		} else {
			recordTest(SKIPPED, "testWorkerCreateBuffer");
			recordTest(SKIPPED, "testWorkerRunParallel");
			recordTest(SKIPPED, "testWorkerMatrixMult");
			recordTest(SKIPPED, "testWorkerKMeans");
		}
	}
	
	auto end_time = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time-start_time).count();
	std::cout << "All tests completed in " << duration << " milliseconds" << std::endl;
	std::cout << "Total tests run: " << total_tests << std::endl;
	printPass(std::to_string(tests_passed));
	printFail(std::to_string(tests_failed));
	printSkipped(std::to_string(tests_skipped));
	printWarning(std::to_string(tests_unknown));

	printInfo("Finished");

	// Report whether or not all tests pased successfully
	if (tests_failed > 0) {
		return 1;
	} else {
		return 0;
	}
}

void testLoadBalancer(OpenCLManager& manager) {
	std::vector<unsigned int> device_ids = {1,2,3};
	OpenCLLoadBalancer load_balancer(device_ids);

	unsigned int job1_dev = load_balancer.RequestDevice();
	unsigned int job2_dev = load_balancer.RequestDevice();
	unsigned int job3_dev = load_balancer.RequestDevice();
	Result res;
	if (job1_dev != job2_dev && job1_dev != job3_dev && job2_dev != job3_dev) {
		res = PASS;
	} else {
		res = FAIL;
	}
	recordTest(res, "requesting 3 jobs across 3 devices results in each job on a separate device");

	unsigned int job4_dev = load_balancer.RequestDevice();
	unsigned int job5_dev = load_balancer.RequestDevice();
	unsigned int job6_dev = load_balancer.RequestDevice();
	if (job4_dev != job5_dev && job4_dev != job6_dev && job5_dev != job6_dev) {
		res = PASS;
	} else {
		res = FAIL;
	}
	recordTest(res, "requesting another 3 jobs across 3 devices results in each job on a separate device");

	load_balancer.ReleaseDevice(job4_dev);
	unsigned int job7_dev = load_balancer.RequestDevice();
	if (job4_dev == job7_dev) {
		res = PASS;
	} else {
		res = FAIL;
		std::cout << "4: " << job4_dev << ", 7: " << job7_dev << std::endl; 
	}
	recordTest(res, "releasing and re-aquiring when job numbers are blananced returns same device");

}

void testBufferReadWrite(OpenCLManager& manager, OpenCLDevice& device) {
	Result res = UNKNOWN;

	Component test_component("testBufferReadWriteComponent");
	Worker test_worker(test_component, "opencl_worker", "testBufferReadWriteWorker");

	// Create a test buffer
	printInfo("Creating a buffer of length 4...");
	auto buffer = manager.CreateBuffer<float>(test_worker, 4);
	if (buffer->is_valid()) {
		res = PASS;
	} else {
		res = FAIL;
	}
	recordTest(res, "Created valid OCLBuffer of floats");
	if (res == FAIL) return;
	//OCLBuffer<std::string>* stringBuffer = manager->CreateBuffer<std::string>(1024);

	// Send some data through
	printInfo("Writing data to a buffer (4 elements containing numbers 10~13)...");
	auto in_data = std::vector<float>(4);
	for (int i=0; i<in_data.size(); i++) {
		in_data[i] = (float)i+10;
	}
	bool did_write_data = buffer->WriteData(test_worker, in_data, device);
	if (did_write_data) {
		res = PASS;
	} else {
		res = FAIL;
	}
	recordTest(res, "Write operation to buffer reported success");

	// Read it back
	printInfo("Reading the data back...");
	auto out_data = buffer->ReadData(test_worker, device);
	bool vectors_match = true;
	/*if (in_data.size() != out_data.size()) {
		vectors_match = false;
	} else {
		for (int i=0; i<in_data.size(); i++) {
			if (in_data[i] != out_data[i]) {
				std::cout << "Mismatch at element " << i << ": expected " << in_data[i] << ", got " << out_data[i] << std::endl;
				vectors_match = false;
			}
		}
	}*/
	
	//if (vectors_match) {
	if (in_data == out_data) { 
		res = PASS;
	} else {
		res = FAIL;
	}
	recordTest(res, "Write to buffer of length 4 then reading returns the correct result");

	auto out_data2 = buffer->ReadData(test_worker, device);
	vectors_match = true;
	//std::cout << in_data.size() << ", " << out_data.size() <<std::endl;
	if (out_data.size() != out_data2.size()) {
		vectors_match = false;
	} else {
		for (int i=0; i<in_data.size(); i++) {
			if (out_data[i] != out_data2[i]) {
				std::cout << "Mismatch at element " << i << ": expected " << out_data[i] << ", got " << out_data2[i] << std::endl;
				vectors_match = false;
			}
		}
	}
	
	if (vectors_match) {
		res = PASS;
	} else {
		res = FAIL;
	}
	recordTest(res, "Second read gives the same result");

	delete buffer;
}


void testKernelPassthrough(OpenCLManager& manager, OpenCLDevice& device) {
	Result res = UNKNOWN;
	printInfo("Running kernel passthrough test...");

	Component passthroughComponent("passthroughComponent");
	Worker passthroughWorker(passthroughComponent, "OpenCL_Worker", "passthroughWorker");

	auto in_data = std::vector<float>(4, 0.3f);
	auto in_buffer = manager.CreateBuffer<float>(passthroughWorker, in_data, device);
	if (in_buffer->is_valid()) {
		res = PASS;
	} else {
		res = FAIL;
	}
	recordTest(res, "Created valid OCLBuffer to write data to");

	auto out_buffer = manager.CreateBuffer<float>(passthroughWorker, 4);
	if (out_buffer->is_valid()) {
		res = PASS;
	} else {
		res = FAIL;
	}
	recordTest(res, "Created valid OCLBuffer to read data from");

	std::vector<std::string> filenames;
	filenames.push_back(GetSourcePath("kernel.cl"));
	if (device.LoadKernelsFromSource(passthroughWorker, filenames)) {
		res = PASS;
	} else {
		res = FAIL;
	}
	recordTest(res, "Loading passthrough kernel file onto device reported success");

	OpenCLKernel* passthrough_kernel = NULL;
	for (auto& kernel : device.GetKernels()) {
		if (kernel.get().GetName() == "dataPassthroughTest") {
			passthrough_kernel = &kernel.get();
		}
	}
	if (passthrough_kernel != NULL) {
		res = PASS;
	} else {
		res = FAIL;
	}
	recordTest(res, "Was able to find passthrough kernel in the list of available kernels for the device");

	printInfo(passthrough_kernel->GetName());

	printInfo("Setting passthrough kernel arguments...");
	
	try {
		passthrough_kernel->SetArgs((*in_buffer), (*out_buffer));
		res = PASS;
	} catch (OpenCLException ocle) {
		std::cerr << ocle << std::endl;
		res = FAIL;
	}	
	recordTest(res, "Setting arguments of passthrough kernel did report success");

	printInfo("Running passthrough kernel...");
	try {
		passthrough_kernel->Run(device, true, cl::NullRange, cl::NDRange(4), cl::NDRange(4));
		res = PASS;
	} catch (OpenCLException ocle) {
		std::cerr << ocle << std::endl;
		res = FAIL;
	}
	recordTest(res, "Passthrough kernel that copies write buffer contents to read buffer reported successful run");

	printInfo("Reading back result...");
	auto out_data = out_buffer->ReadData(passthroughWorker, device);
	if (out_data.size() > 0) {
		res = PASS;
	} else {
		res = FAIL;
	}
	recordTest(res, "Data was successfully read back into the read buffer (data vector was not empty)");

	bool vectors_match = true;
	for (int i=0; i<in_data.size(); i++) {
		if (in_data[i] != out_data[0]) {
			std::cout << "Mismatch at element " << i << ": " << in_data[i] << "/" << out_data[i] << std::endl;
			vectors_match = false;
		}
	}
	if (vectors_match) {
		res = PASS;
	} else {
		res = FAIL;
	}
	recordTest(res, "Data read back from the read buffer is identical to the data written to the write buffer");

	manager.ReleaseBuffer(passthroughWorker, in_buffer);
	manager.ReleaseBuffer(passthroughWorker, out_buffer);
}

OpenCL_Worker* testWorkerConstruction(Component& component, int device_id) {
	Result res = UNKNOWN;

	printInfo("Creating an OpenCL_Worker...");

	OpenCL_Worker* worker = new OpenCL_Worker(component, "OpenCL_Worker_ConstructionTest");

	auto platform_attr = worker->GetAttribute("platform_id").lock();
	if (platform_attr == NULL) {
		res = FAIL;
	} else {
		res = PASS;
	}
	recordTest(res, "Successfully retrieved 'platform_id' attribute for OpenCL_Worker");

	auto device_attr = worker->GetAttribute("device_id").lock();
	if (device_attr == NULL) {
		res = FAIL;
	} else {
		res = PASS;
	}
	recordTest(res, "Successfully retrieved 'device_id' attribute for OpenCL_Worker");

	platform_attr->set_Integer(0);
	device_attr->set_Integer(device_id);

	worker->Configure();

	if (worker->IsValid()) {
		res = PASS;
	} else {
		res = FAIL;
	}
	recordTest(res, "OpenCL_Worker reports itself to be valid after configuration");

	if (res == FAIL) {
		delete worker;
		worker = NULL;
	}

	return worker;
}

template <typename T>
void testWorkerCreateBufferOfType(OpenCL_Worker& worker, std::string type_name, std::vector<T> vec_4) {
	Result res = UNKNOWN;
	std::vector<T> vec_empty;
	OCLBuffer<T>* buffer_empty = worker.CreateBuffer(vec_empty, true);
	if (buffer_empty != NULL) {
		res = PASS;
	} else {
		res = FAIL;
	}
	recordTest(res, "Create buffer for empty "+type_name+" vector did not return null");
	if (!buffer_empty->is_valid()) {
		res = PASS;
	} else {
		res = FAIL;
	}
	recordTest(res, "Create buffer for empty "+type_name+" does signal that it is invalid");
	if (worker.ReadBuffer(*buffer_empty).empty()) {
		res = PASS;
	} else {
		res = FAIL;
	}
	recordTest(res, "Create buffer for empty "+type_name+" vector returned empty vector when read from");

	worker.ReleaseBuffer(buffer_empty);
	

	OCLBuffer<T>* buffer_4 = worker.CreateBuffer(vec_4);
	if (buffer_4 != NULL) {
		res = PASS;
	} else {
		res = FAIL;
	}
	recordTest(res, "Create buffer for length 4 "+type_name+" vector did not return null");
	if (buffer_4->is_valid()) {
		res = PASS;
	} else {
		res = FAIL;
	}
	recordTest(res, "Create buffer for length 4 "+type_name+" does signal that it is valid");
	const auto& vec_4_out = worker.ReadBuffer(*buffer_4);
	bool contents_equal = true;
	if (vec_4_out.size() != vec_4.size()) contents_equal = false;
	for (int i=0; i<vec_4.size(); i++) {
		if (vec_4_out[i] != vec_4[i]) contents_equal = false;
	}
	//if (worker->ReadBuffer(*buffer_4) == vec_4) {
	if (contents_equal) {
		res = PASS;
	} else {
		res = FAIL;
	}
	recordTest(res, "Create buffer for length 4 "+type_name+" vector returned data equal to that with which it was constructed");
	auto vec_5(vec_4);
	vec_5.push_back(0);
	if (worker.WriteBuffer(*buffer_4, vec_5, true)) {
		res = FAIL;
	} else {
		res = PASS;
	}
	recordTest(res, "Writing 5 elements to "+type_name+" buffer of length 4 reported failure");
	auto vec_4_rev(vec_4);
	std::reverse(vec_4_rev.begin(), vec_4_rev.end());
	if (worker.WriteBuffer(*buffer_4, vec_4_rev, true)) {
		
	} else {
		res = FAIL;
	}
	recordTest(res, "Writing 4 elements to"+type_name+" buffer of length 4 reported success");
	/*if (buffer_4->ReadData(true, worker) == vec_4_rev) {
		res = PASS;			
	} else {
		res = FAIL;
	}
	recordTest(res, "Read back the correct elements from the underlying buffer");*/
	if (worker.ReadBuffer(*buffer_4, true) == vec_4_rev) {
		res = PASS;			
	} else {
		res = FAIL;
	}
	recordTest(res, "Read back the correct elements from the buffer");

	worker.ReleaseBuffer(buffer_4);
}

void testWorkerCreateBuffer(OpenCL_Worker& worker) {
	
	testWorkerCreateBufferOfType<int>(worker, "int", {1,2,3,4});
	testWorkerCreateBufferOfType<float>(worker, "float", {1.0, 2.0, 3.0, 4.0});
	testWorkerCreateBufferOfType<short>(worker, "short", {4,5,6,7});
	//testWorkerCreateBufferOfType<bool>(worker, "bool", {true, false, false, true});	// Should throw static assertion!
	//testWorkerCreateBufferOfType<std::string>(worker, "string", {"This", "is", "a", "test"});	// Should throw static assertion!
}

void individualParallelRun(OpenCL_Worker& worker, int threads, cl_long ops) {
	Result res = UNKNOWN;
	//std::cout << "Calling OpenCL_Worker function RunParallel with "<<threads<<" thread, "<<ops<<" operation... " << std::endl;
	auto start = std::chrono::steady_clock::now();
	if (worker.RunParallel(threads, ops)) {
		res = PASS;
	} else {
		res = FAIL;
	}
	auto end = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
	recordTest(res, "Execute RunParallel with "+std::to_string(threads)+" thread, "+std::to_string(ops)+" operations");
	printInfo("Completed in "+std::to_string(duration)+" milliseconds");
}

void testWorkerRunParallel(OpenCL_Worker& worker) {
	cl_long small_ops = 1;
	cl_long large_ops = 1LL<<22LL;
	int small_threads = 1;
	int med_threads = 64;
	int large_threads = 1<<14;

	individualParallelRun(worker, small_threads, small_ops);
	individualParallelRun(worker, small_threads, large_ops);
	individualParallelRun(worker, med_threads, small_ops);
	individualParallelRun(worker, med_threads, large_ops);
	individualParallelRun(worker, large_threads, small_ops);
	individualParallelRun(worker, large_threads, large_ops);
}

void testWorkerMatrixMult(OpenCL_Worker& worker) {

	testWorkerSquareMult(worker, 1);
	testWorkerSquareMult(worker, 1);
	testWorkerSquareMult(worker, 2);
	testWorkerSquareMult(worker, 4);

	testWorkerSquareMult(worker, 32);
	testWorkerSquareMult(worker, 37);
	testWorkerSquareMult(worker, 64);
	testWorkerSquareMult(worker, 1023);
	testWorkerSquareMult(worker, 1024);
	
	//squareMult(4096, false);	// Will take some time...

	testWorkerSquareMult(worker, 0, true);

	testWorkerRectMult(worker, 1,2,2,1);
	testWorkerRectMult(worker, 2,1,1,2);
	testWorkerRectMult(worker, 2,3,3,2);
	testWorkerRectMult(worker, 4,3,3,2);

	// Following tests intended to target <= 32x32 tile boundaries, but also 64x64 to some degree
	testWorkerRectMult(worker, 13,15,15,17);
	testWorkerRectMult(worker, 15,17,17,19);

	testWorkerRectMult(worker, 15,5,5,11);

	testWorkerRectMult(worker, 29,31,31,33);
	testWorkerRectMult(worker, 31,33,33,35);

	testWorkerRectMult(worker, 153,56,56,87);
	testWorkerRectMult(worker, 74,49,49,169);

	testWorkerRectMult(worker, 1021,1022,1022,1023);

	testWorkerRectMult(worker, 1,2,3,4, true);
	//cout << "Tests Took: " << (endTime.get_msec() - startTime.get_msec())*1e-3 << " seconds" << endl;
	//cout << "   Too big..." << endl;
}

void testWorkerSquareMult(OpenCL_Worker& worker, unsigned int length, bool expect_failure) {
	printInfo("Square "+std::to_string(length)+"x"+std::to_string(length)+"...");
	Result res;
	bool calculation_finished = false;
	unsigned int elements = length*length;

	std::vector<float> matA(elements);
	std::vector<float> matB(elements);
	std::vector<float> matC(elements);

	// std::fill(matA.begin(), matA.end(), 0.0);
	// std::fill(matB.begin(), matB.end(), 0.0);
	// std::fill(matC.begin(), matC.end(), -1.0);
	for (auto& e : matA) e = 0.0;
	for (auto& e : matB) e = 0.0;
	for (auto& e : matC) e = 0.0;

	calculation_finished = worker.MatrixMult(matA, matB, matC);
	if (calculation_finished != expect_failure) {
		res = PASS;
	} else {
		res = FAIL;
	}
	recordTest(res, "MatrixMult kernel did report success for running with size " +
		std::to_string(length) + "x" + std::to_string(length));
	
	if (calculation_finished) {
		res = checkMultiplication(matA.data(), matB.data(), matC.data(), length, length, length);
	}
	recordTest(res, "Correct values for multiplication of zeroed square matrix of size " +
		std::to_string(length) + "x" + std::to_string(length));

	for (unsigned int index=0; index<elements; index++) matA[index]=(float)index;
	for (unsigned int index=0; index<elements; index++) matB[index]=(float)index;
	for (unsigned int index=0; index<elements; index++) matC[index]= std::numeric_limits<float>::signaling_NaN();

	calculation_finished = worker.MatrixMult(matA, matB, matC);
	if (!calculation_finished) {
		if (expect_failure) {
			res = PASS;
		} else {
			res = FAIL;
		}
	} else {
		res = checkMultiplication(matA.data(), matB.data(), matC.data(), length, length, length);
	}
	recordTest(res, "Correct values for multiplication of element-wise-incremental square matrix of size " +
		std::to_string(length) + "x" + std::to_string(length));

	srand(RANDSEED);
	for (unsigned int index=0; index<elements; index++) matA[index]=(float)(rand()%1000000000)/100 - 1000000;
	for (unsigned int index=0; index<elements; index++) matB[index]=(float)(rand()%1000000000)/100 - 1000000;
	for (unsigned int index=0; index<elements; index++) matC[index]= std::numeric_limits<float>::signaling_NaN();

	calculation_finished = worker.MatrixMult(matA, matB, matC);
	if (!calculation_finished) {
		if (expect_failure) {
			res = PASS;
		} else {
			res = FAIL;
		}
	} else {
		res = checkMultiplication(matA.data(), matB.data(), matC.data(), length, length, length);
	}
	recordTest(res, "Correct values for multiplication of pseudo-randomized square matrix of size " +
		std::to_string(length) + "x" + std::to_string(length));
	
	
}

void testWorkerRectMult(OpenCL_Worker& worker, unsigned int rowsA, unsigned int colsA,
	unsigned int rowsB, unsigned int colsB, bool expect_failure) {
	printInfo("Rect "+ std::to_string(rowsA) + "x" + std::to_string(colsA) + " by " +
		std::to_string(rowsB) + "x" + std::to_string(colsB));
		
	Result res;
	bool calculation_finished = false;
	unsigned int lenA = rowsA*colsA;
	unsigned int lenB = rowsB*colsB;
	unsigned int lenC = rowsA*colsB;

	std::vector<float> matA(lenA);
	std::vector<float> matB(lenB);
	std::vector<float> matC(lenC);


	for (unsigned int index=0; index<lenA; index++) matA[index]=0.0;
	for (unsigned int index=0; index<lenB; index++) matB[index]=0.0;
	//for (unsigned int index=0; index<lenC; index++) matC[index]=-1.0;
	for (unsigned int index=0; index<lenC; index++) matC[index]= std::numeric_limits<float>::signaling_NaN();

	
	calculation_finished = worker.MatrixMult(matA, matB, matC);
	if (calculation_finished != expect_failure) {
		res = PASS;
	} else {
		res = FAIL;
	}
	recordTest(res, "MatrixMult kernel did report success for running with size " +
		std::to_string(rowsA) + "x" + std::to_string(colsA) + " by " +
		std::to_string(rowsB) + "x" + std::to_string(colsB));
	
	if (calculation_finished) {
		res = checkMultiplication(matA.data(), matB.data(), matC.data(), rowsA, colsA, colsB);
	}
	recordTest(res, "Correct values for multiplication of zeroed matrix of size " +
		std::to_string(rowsA) + "x" + std::to_string(colsA) + " by " +
		std::to_string(rowsB) + "x" + std::to_string(colsB));

	// Check incremental matrix
	for (unsigned int index=0; index<lenA; index++) matA[index]=(float)index;
	for (unsigned int index=0; index<lenB; index++) matB[index]=(float)index;
	for (unsigned int index=0; index<lenC; index++) matC[index]= std::numeric_limits<float>::signaling_NaN();

	calculation_finished = worker.MatrixMult(matA, matB, matC);
	if (!calculation_finished) {
		if (expect_failure) {
			res = PASS;
		} else {
			res = FAIL;
		}
	} else {
		res = checkMultiplication(matA.data(), matB.data(), matC.data(), rowsA, colsA, colsB);
	}
	recordTest(res, "Correct values for multiplication of element-wise-incrementing matrix of size " +
		std::to_string(rowsA) + "x" + std::to_string(colsA) + " by " +
		std::to_string(rowsB) + "x" + std::to_string(colsB));

	// Check randomly initialised matrix
	srand(RANDSEED);
	for (unsigned int index=0; index<lenA; index++) matA[index]=(float)(rand()%1000000000)/100 - 1000000;
	for (unsigned int index=0; index<lenB; index++) matB[index]=(float)(rand()%1000000000)/100 - 1000000;
	for (unsigned int index=0; index<lenC; index++) matC[index]= std::numeric_limits<float>::signaling_NaN();

	calculation_finished = worker.MatrixMult(matA, matB, matC);
	if (!calculation_finished) {
		if (expect_failure) {
			res = PASS;
		} else {
			res = FAIL;
		}
	} else {
		res = checkMultiplication(matA.data(), matB.data(), matC.data(), rowsA, colsA, colsB);
	}
	recordTest(res, "Correct values for multiplication of pseudo-random matrix of size " +
		std::to_string(rowsA) + "x" + std::to_string(colsA) + " by " +
		std::to_string(rowsB) + "x" + std::to_string(colsB));

}

Result checkMultiplication(float* matA, float* matB, float* matC, unsigned int m, unsigned int k, unsigned int n) {
	Result res = UNKNOWN;
	std::cout << std::setprecision(10);	
	for (unsigned int col=0; col<n; col++) {
		for (unsigned int row=0; row<m; row++) {
			
			float accum = 0;
			for (unsigned int t=0; t<k; t++) {
				accum += matA[t + row*k]*matB[col + t*n];
			}

			if (!compare_float(accum, matC[col + row*n], EPS)) {
				if (m*n*k > 100) return FAIL;
				//if (verbose) {
					std::cout << "At pos [" << row << ',' << col << "] expected: " << accum << ", result: " << matC[col + row*n] << std::endl;
					std::cout << matA[row*k] << ", " << matB[col] << std::endl;
					res = FAIL;
			//	} else { // No need to keep checking if we're not printing the info... 
			//		return FAIL;
				//}
			}
		}
	}

	if (res != FAIL) res = PASS;

	return res;
}

void testWorkerKMeans(OpenCL_Worker& worker) {
	Result res;

	std::vector<float> point_vec = {
		10, 4, 5, 7,
		19, 3, 6, 5,
		 2,11, 0,-3,
		 4, 9, 1, 0,
		13, 5, 4, 9,
		 7, 7, 2, 2
	};
	std::vector<float> center_vec = {
		0, 10, 0, 0,
		10, 1, 1, 7
	};
	std::vector<int> class_vec(6);

	bool calculation_finished = worker.KmeansCluster(point_vec, center_vec, class_vec, 5);
	if (calculation_finished) {
		res = PASS;
	} else {
		res = FAIL;
	}
	recordTest(res, "Does KMeans clustering algorithm report success after running small case.");

	std::vector<int> expected_classes = {
		1, 1, 0, 0, 1, 0
	};
	if (class_vec == expected_classes) {
		res = PASS;
	} else {
		res = FAIL;
	}
	recordTest(res, "Does KMeans clustering algorithm correctly classify after 5 iterations for small case.");

	point_vec.clear();
	for (int i=0; i<2000; i++) {
		point_vec.push_back(300 + (float)(rand()%50));
		point_vec.push_back(900 + (float)(rand()%50));
		point_vec.push_back(100 + (float)(rand()%50));
		point_vec.push_back(100 + (float)(rand()%50));
	}
	for (int i=0; i<4000; i++) {
		point_vec.push_back(700 + (float)(rand()%50));
		point_vec.push_back(200 + (float)(rand()%50));
		point_vec.push_back(600 + (float)(rand()%50));
		point_vec.push_back(700 + (float)(rand()%50));
	}
	for (int i=0; i<3000; i++) {
		point_vec.push_back(100 + (float)(rand()%50));
		point_vec.push_back(400 + (float)(rand()%50));
		point_vec.push_back(900 + (float)(rand()%50));
		point_vec.push_back(300 + (float)(rand()%50));
	}

	center_vec.clear();
	center_vec = {
		250, 350, 900, 400,
		450, 750, 200, 300,
		650, 300, 750, 900
	};

	class_vec.clear();           
	class_vec.resize(9000);

	calculation_finished = worker.KmeansCluster(point_vec, center_vec, class_vec, 2);
	if (calculation_finished) {
		res = PASS;
	} else {
		res = FAIL;
	}
	recordTest(res, "Does KMeans clustering algorithm report success after running large case.");

	res = PASS;
	for (int i=0; i<2000; i++) {
		if (class_vec[i] != 1) {
			std::cout << "at position " << i << ": " << class_vec[i] << std::endl;
			std::cout << point_vec[i*4] << ',' << point_vec[i*4+1] << ',' << point_vec[i*4+2] << ',' << point_vec[i*4+3] << ',' << std::endl;
			res = FAIL;
			break;
		}
	}
	for (int i=2000; i<6000; i++) {
		if (class_vec[i] != 2) {
			res = FAIL;
			break;
		}
	}
	for (int i=6000; i<9000; i++) {
		if (class_vec[i] != 0) {
			res = FAIL;
			break;
		}
	}
	recordTest(res, "Does KMeans clustering algorithm correctly classify after 5 iterations for large case.");

	/*
	std::cout << "centers:" << std::endl; 
	int val_counter=0;
	for(auto e : center_vec) {
		std::cout << e <<',';
		val_counter++;
		if (val_counter >= 4) {
			val_counter=0;
			std::cout << std::endl;
		}
	}
	std::cout << "classifications: " << std::endl;
	for (auto e : class_vec) {
		std::cout << e << std::endl;
	}*/


}
