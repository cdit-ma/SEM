

#include <iostream>
#include <vector>

#include <math.h>
#include <limits>
#include <random>

#include <chrono>

#include "openclmanager.h"
#include "openclkernel.hpp"
#include "openclworker.h"

#define RANDSEED 13520

#define EPS 1e-7
#define CHECK_FLOAT(x, y, eps) (fabs(x-y)<eps)

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
	std::cout << "\033[5m" << "\033[32m" << "[PASS] " << "\033[0m";
	std::cout << description << std::endl;
}

void printFail(std::string description) {
	std::cout << "\033[31m" << "[FAIL] " << "\033[0m";
	std::cout << description << std::endl;
}

void printSkipped(std::string description) {
	std::cerr << "\033[34m" << "[SKIPPED] " << "\033[0m";
	std::cout << description << std::endl;
}

void printWarning(std::string description) {
	std::cerr << "\033[33m" << "[WARNING] " << "\033[0m";
	std::cout << description << std::endl;
}

void printInfo(std::string description) {
	std::cerr << "\033[36m" << "[INFO] " << "\033[0m";
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
void testBufferReadWrite(OpenCLManager& manager);
void testKernelPassthrough(OpenCLManager& manager, OpenCLKernel& passthrough_kernel);

OpenCLWorker* testWorkerConstruction();
void testWorkerDestruction(OpenCLWorker* worker);
void testWorkerRunParallel(OpenCLWorker* worker);
void testWorkerMatrixMult(OpenCLWorker* worker);
	void testWorkerSquareMult(OpenCLWorker* worker, unsigned int length, bool expect_failure=false);
	void testWorkerRectMult(OpenCLWorker* worker, unsigned int rowsA, unsigned int colsA,
			unsigned int rowsB, unsigned int colsB, bool expect_failure=false);
	Result checkMultiplication(float* matA, float* matB, float* matC,
								unsigned int m, unsigned int k, unsigned int n);

int main(int argc, char** argv) {
	auto start_time = std::chrono::steady_clock::now();
	Result res = UNKNOWN;

	printInfo("Beginning OpenCL worker Tester...");

	// List out the avaialble platforms
	auto platforms = OpenCLManager::GetPlatforms();
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
	
	auto manager = OpenCLManager::GetReferenceByPlatform(0);
	if (manager->IsValid()) {
		res = PASS;
		printPass("Successfully retrieved OpenCLManager (first available)");
	}
	else {
		res = FAIL;
	}
	recordTest(res, "Initialise and return reference to first available OpenCLManager");
	if (res == FAIL) {
		return 1;
	}

	printInfo("Listing available devices for platform:");
	for (auto device : manager->GetDevices()) {
		std::cout << " - " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
	}

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
	}

	testBufferReadWrite(*manager);

	OpenCLKernel* pt_kernel;
	bool found_pt_kernel = false;
	for (auto& k : kernels) {
		if (k.GetName() == "dataPassthroughTest") {
			pt_kernel = &k;
			found_pt_kernel = true;
		}
	}
	if (!found_pt_kernel) {
		printFail("Unable to find dataPasshtoughTest kernel");
	}

	testKernelPassthrough(*manager, *pt_kernel);


	OpenCLWorker* worker = testWorkerConstruction();

	// Run worker tests conditional on worker having been successfully constructed
	testWorkerRunParallel(worker);
	testWorkerMatrixMult(worker);
	
	
	auto end_time = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time-start_time).count();
	std::cout << "All tests completed in " << duration << " milliseconds" << std::endl;
	std::cout << "Total tests run: " << total_tests << std::endl;
	printPass(std::to_string(tests_passed));
	printFail(std::to_string(tests_failed));
	printSkipped(std::to_string(tests_skipped));
	printWarning(std::to_string(tests_unknown));

	printInfo("Finished");

	//system("pause");
}

void testBufferReadWrite(OpenCLManager& manager) {
	Result res = UNKNOWN;

	// Create a test buffer
	printInfo("Creating a buffer of length 4...");
	auto buffer = manager.CreateBuffer<float>(4);
	//OCLBuffer<std::string>* stringBuffer = manager->CreateBuffer<std::string>(1024);

	// Send some data through
	printInfo("Writing data to a buffer (4 elements containing 0.5f)...");
	auto in_data = std::vector<float>(4, 0.5f);
	buffer->WriteData(in_data);

	// Read it back
	printInfo("Reading the data back...");
	auto out_data = buffer->ReadData();
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
	recordTest(res, "Write to buffer of length 4 then read back the result");

	delete buffer;
}


void testKernelPassthrough(OpenCLManager& manager, OpenCLKernel& passthrough_kernel) {
	Result res = UNKNOWN;

	auto in_data = std::vector<float>(4, 0.5f);
	auto in_buffer = manager.CreateBuffer<float>(in_data);
	auto out_buffer = manager.CreateBuffer<float>(4);

	printInfo("Setting passthrough kernel arguments...");
	passthrough_kernel.SetArgs((*in_buffer), (*out_buffer));
	//kernel->SetArgs(*in_buffer, 2);

	printInfo("Running passthrough kernel...");
	passthrough_kernel.Run(0, true, cl::NullRange, cl::NDRange(4), cl::NDRange(4));
	/*cl::Event kernelEvent;
	err = manager.GetQueues()[0].enqueueNDRangeKernel(passthrough_kernel.GetBackingRef(), 
			cl::NullRange, cl::NDRange(4), cl::NDRange(4), NULL, &kernelEvent);
	if (err != CL_SUCCESS) {
		LogOpenCLError(NULL, __func__, "Couldnt EnqueueNDRangeKernel", err);
	}
	kernelEvent.wait();*/

	printInfo("Reading back result...");
	auto out_data = out_buffer->ReadData();
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
	recordTest(res, "Send float buffer to kernel, copy to another buffer and read back the data");

	manager.ReleaseBuffer(in_buffer);
	manager.ReleaseBuffer(out_buffer);
}

OpenCLWorker* testWorkerConstruction() {
	Result res = UNKNOWN;

	printInfo("Creating an OpenCLWorker...");

	Component test_component("TestComponent");

	OpenCLWorker* worker = new OpenCLWorker(&test_component, "OpenCLWorker_ConstructionTest");

	if (worker->IsValid()) {
		res = PASS;
	} else {
		res = FAIL;
	}
	recordTest(res, "Construct a valid OpenCLWorker");

	if (res == FAIL) {
		delete worker;
		worker = NULL;
	}

	return worker;
}

void individualParallelRun(OpenCLWorker* worker, int threads, cl_long ops) {
	Result res = UNKNOWN;
	std::cout << "Calling OpenCLWorker function RunParallel with "<<threads<<" thread, "<<ops<<" operation... " << std::endl;
	auto start = std::chrono::steady_clock::now();
	if (worker->RunParallel(threads, ops)) {
		res = PASS;
	} else {
		res = FAIL;
	}
	auto end = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
	recordTest(res, "Execute RunParallel with "+std::to_string(threads)+" thread, "+std::to_string(ops)+" operations");
	printInfo("Completed in "+std::to_string(duration)+" milliseconds");
}

void testWorkerRunParallel(OpenCLWorker* worker) {
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

void testWorkerMatrixMult(OpenCLWorker* worker) {

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

void testWorkerSquareMult(OpenCLWorker* worker, unsigned int length, bool expect_failure) {
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
	for (auto& e : matC) e = -1.0;

	calculation_finished = worker->MatrixMult(matA, matB, matC);
	if (!calculation_finished) {
		if (expect_failure) {
			res = PASS;
		} else {
			res = FAIL;
		}
	} else {
		res = checkMultiplication(matA.data(), matB.data(), matC.data(), length, length, length);
	}

	recordTest(res,
		"Multiplication of zeroed square matrix of size " + std::to_string(length) + "x" + std::to_string(length));

	for (unsigned int index=0; index<elements; index++) matA[index]=(float)index;
	for (unsigned int index=0; index<elements; index++) matB[index]=(float)index;
	for (unsigned int index=0; index<elements; index++) matC[index]= std::numeric_limits<float>::signaling_NaN();

	calculation_finished = worker->MatrixMult(matA, matB, matC);
	if (!calculation_finished) {
		if (expect_failure) {
			res = PASS;
		} else {
			res = FAIL;
		}
	} else {
		res = checkMultiplication(matA.data(), matB.data(), matC.data(), length, length, length);
	}
	recordTest(res,
		"Multiplication of element-wise-incremental square matrix of size " + std::to_string(length) + "x" + std::to_string(length));

	srand(RANDSEED);
	for (unsigned int index=0; index<elements; index++) matA[index]=(float)(rand()%1000000000)/100 - 1000000;
	for (unsigned int index=0; index<elements; index++) matB[index]=(float)(rand()%1000000000)/100 - 1000000;
	for (unsigned int index=0; index<elements; index++) matC[index]= std::numeric_limits<float>::signaling_NaN();

	calculation_finished = worker->MatrixMult(matA, matB, matC);
	if (!calculation_finished) {
		if (expect_failure) {
			res = PASS;
		} else {
			res = FAIL;
		}
	} else {
		res = checkMultiplication(matA.data(), matB.data(), matC.data(), length, length, length);
	}
	recordTest(res, "Multiplication of pseudo-randomized square matrix of size " + std::to_string(length) + "x" + std::to_string(length));
	
}

void testWorkerRectMult(OpenCLWorker* worker, unsigned int rowsA, unsigned int colsA,
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

	
	calculation_finished = worker->MatrixMult(matA, matB, matC);
	if (!calculation_finished) {
		if (expect_failure) {
			res = PASS;
		} else {
			res = FAIL;
		}
	} else {
		res = checkMultiplication(matA.data(), matB.data(), matC.data(), rowsA, colsA, colsB);
	}
	recordTest(res, "Multiplication of zeroed matrix of size " +
		std::to_string(rowsA) + "x" + std::to_string(colsA) + " by " +
		std::to_string(rowsB) + "x" + std::to_string(colsB));

	// Check incremental matrix
	for (unsigned int index=0; index<lenA; index++) matA[index]=(float)index;
	for (unsigned int index=0; index<lenB; index++) matB[index]=(float)index;
	for (unsigned int index=0; index<lenC; index++) matC[index]= std::numeric_limits<float>::signaling_NaN();

	calculation_finished = worker->MatrixMult(matA, matB, matC);
	if (!calculation_finished) {
		if (expect_failure) {
			res = PASS;
		} else {
			res = FAIL;
		}
	} else {
		res = checkMultiplication(matA.data(), matB.data(), matC.data(), rowsA, colsA, colsB);
	}
	recordTest(res, "Multiplication of element-wise-incrementing matrix of size " +
		std::to_string(rowsA) + "x" + std::to_string(colsA) + " by " +
		std::to_string(rowsB) + "x" + std::to_string(colsB));

	// Check randomly initialised matrix
	srand(RANDSEED);
	for (unsigned int index=0; index<lenA; index++) matA[index]=(float)(rand()%1000000000)/100 - 1000000;
	for (unsigned int index=0; index<lenB; index++) matB[index]=(float)(rand()%1000000000)/100 - 1000000;
	for (unsigned int index=0; index<lenC; index++) matC[index]= std::numeric_limits<float>::signaling_NaN();

	calculation_finished = worker->MatrixMult(matA, matB, matC);
	if (!calculation_finished) {
		if (expect_failure) {
			res = PASS;
		} else {
			res = FAIL;
		}
	} else {
		res = checkMultiplication(matA.data(), matB.data(), matC.data(), rowsA, colsA, colsB);
	}
	recordTest(res, "Multiplication of pseudo-random matrix of size " +
		std::to_string(rowsA) + "x" + std::to_string(colsA) + " by " +
		std::to_string(rowsB) + "x" + std::to_string(colsB));

}

Result checkMultiplication(float* matA, float* matB, float* matC, unsigned int m, unsigned int k, unsigned int n) {
	Result res = UNKNOWN;
	
	for (unsigned int col=0; col<n; col++) {
		for (unsigned int row=0; row<m; row++) {
			
			float accum = 0;
			for (unsigned int t=0; t<k; t++) {
				accum += matA[t + row*k]*matB[col + t*n];
			}

			if (!CHECK_FLOAT(accum, matC[col + row*n], EPS)) {
				/*if (verbose) {
					std::cout << "At pos [" << row << ',' << col << "] expected: " << accum << ", result: " << matC[col + row*n] << std::endl;
					std::cout << matA[row*k] << ", " << matB[col] << std::endl;
					res = FAIL;
				} else { // No need to keep checking if we're not printing the info... */
					return FAIL;
				//}
			}
		}
	}

	if (res != FAIL) res = PASS;

	return res;
}