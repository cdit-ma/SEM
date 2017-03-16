// WE_GPU.mpc  2015-08-17  Jackson Michael 

#include "WE_GPU_Impl.h" 
#include "WE_GPU.h"
#include "atmsp.h"
#include <iostream>
#include <sstream>

/**
 * The WE_GPU class acts is a wrapper for WE_GPU_Impl, allowing projects to be compiled
 * without having to link against the dependencies requied by the proper GPU worker
 * implementation.
 */

WE_GPU::WE_GPU (void) 
: impl_ (new WE_GPU_Impl ()) {
	initialise(false);
}

WE_GPU::~WE_GPU (void) {
	delete impl_;
}

void WE_GPU::release() {
	impl_->release();
}

void WE_GPU::initialise(bool forceGPU) {
	impl_->initialise(forceGPU);
}

void WE_GPU::sayGreeting () { 
  this->impl_->sayGreeting ();
}

unsigned int WE_GPU::numDevices() {
	return impl_->numDevices();
}

std::string WE_GPU::deviceName(unsigned int gpuNum) {
	return impl_->deviceName();
}

size_t WE_GPU::memCapacity(unsigned int gpuNum) {
	return impl_->memCapacity();
}

bool WE_GPU::bufferData(size_t bytes, bool forcedCopy, bool blocking, unsigned int gpuNum) {
	return impl_->bufferData(bytes, forcedCopy, blocking, gpuNum);
}

bool WE_GPU::releaseData(size_t bytes, bool forceCopy, bool blocking, unsigned int gpuNum) {
	return impl_->releaseData(bytes, forceCopy, blocking, gpuNum);
}

void WE_GPU::runParallel(double numThreads, double opsPerThread, unsigned int gpuNum) {

	this->impl_->runParallel((unsigned int)numThreads, (unsigned int)opsPerThread, gpuNum);
}

// void WE_GPU::FFT(WE_UTE_Vector data, unsigned int gpuNum) {
// 	this->impl_->performFFT_SP(data.memory, data.realSize*data.elementSize, gpuNum);
//}

void WE_GPU::matrixMultLazy(unsigned int n, unsigned int gpuNum) {
	this->impl_->matrixMult(n, gpuNum);
}

// WE_UTE_Vector WE_GPU::matrixMult(WE_UTE_Vector matrixA,
// 							WE_UTE_Vector matrixB,
// 							WE_UTE_Vector matrixC,
// 							unsigned int gpuNum) {
// 	bool success = this->impl_->matrixMult(matrixA.realSize, matrixB.realSize, matrixC.realSize,
// 		matrixA.memory, matrixB.memory , matrixC.memory,
// 							gpuNum);
// 	if (!success) {
// 		matrixC.memory = NULL;
// 		matrixC.boundedSize = 0;
// 		matrixC.elementSize = 0;
// 		matrixC.realSize = 0;
// 	}
// 	return matrixC;
// }

/*
double WE_GPU::evalComplexity(std::string complexity, va_list* argList) {
	ATMSP<double> parser;
	ATMSB<double> byteCode;

	std::string vars;
	std::string mathsSymbols = "()-+/*^0123456789 ";
	std::string functs[] = {"abs", "acos", "asin", "atan", "atan2", "cos", "cosh", 
						"exp", "log", "log10", "sin", "sinh", "sqrt", "tan", "tanh",
						"floor", "round", "min", "max", "sig", "log2", "$e", "$pi" };
	unsigned int functCount = sizeof(functs)/sizeof(std::string);


	// Generate comma seperated list of variables from complexity
	for (unsigned int i=0; i<complexity.length(); i++) {
		bool validVar = true;
		for (unsigned int j=0; j<mathsSymbols.length(); j++) {
			if (complexity[i] == mathsSymbols[j]) {
				validVar = false;
				break;
			}
		}

		for (unsigned int j=0; j<functCount; j++) {
			bool isFunct = true;
			for (unsigned int c=0; c<functs[j].length(); c++) {
				if (complexity[i+c] != functs[j][c] || (i+c)>complexity.length()) {
					isFunct = false;
					break;
				}
			}
			if (isFunct) {
				validVar = false;
				break;
			}
		}
		if (validVar == false) continue;

		if (vars.length() > 0)
			vars.append(",").push_back(complexity[i]);
		else
			vars.push_back(complexity[i]);
	}

	// Make sure that the variables are unique
	std::string uniqueVars;
	unsigned int numVars = 0;
	for (unsigned int i=0; i<vars.length(); i++) {
		if (vars[i] != ',') {
			if (vars.find(vars[i]) != i) { // if we'd already found a copy of the variable somewhere earlier
				i++;
				continue;
			} else {
				numVars++;
			}
		}
		uniqueVars.push_back(vars[i]);
	}
	
	size_t err = parser.parse(byteCode, complexity, uniqueVars);
	if ( err  )
		std::cerr << "Parsing failed with: " << parser.errMessage(err) << std::endl;

	// Associate the supplied values with their respective variables
	for (unsigned int i=0; i<numVars; i++) {
		byteCode.var[i] = va_arg(*argList, double);
	}

	double result = byteCode.run();

	if ( byteCode.fltErr )
		std::cerr << "Faced a NaN/inf error" <<  std::endl;

	return result;
}
*/