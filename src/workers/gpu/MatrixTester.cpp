

#include "MatrixTester.h"

#include <iostream>
#include <string>
#include <stdlib.h>
#include <limits>

using namespace std;
using namespace test;

MatrixTester::MatrixTester(DebugLevel dLevel) : Tester(dLevel) {
	worker = new Gpu_Worker(0, "");
	worker->Initialise(false);
	//ACE_Time_Value startTime = ACE_OS::gettimeofday();

	cout << "   Square 1x1..." << endl;
	niceSquareMult(1);
	cout << "   Square 2x2..." << endl;
	niceSquareMult(2);
	cout << "   Square 4x4..." << endl;
	niceSquareMult(4);

	cout << "   Square 32x32..." << endl;
	niceSquareMult(32, false);
	cout << "   Square 37x37..." << endl;
	niceSquareMult(37, false);
	cout << "   Square 64x64..." << endl;
	niceSquareMult(64, false);
	cout << "   Square 1023x1023..." << endl;
	niceSquareMult(1023, false);
	cout << "   Square 1024x1024..." << endl;
	niceSquareMult(1024, false);
	/*cout << "Square 4096x4096..." << endl;	// Will take some time...
	squareMult(4096, false);*/

	cout << "   Square 0x0..." << endl;
	niceSquareMult(0);

	cout << "   Rect 1x2, 2x1..." << endl;
	rectMult(1,2,2,1);
	cout << "   Rect 2x1, 1x2..." << endl;
	rectMult(2,1,1,2);
	cout << "   Rect 2x3, 3x2..." << endl;
	rectMult(2,3,3,2);
	cout << "   Rect 4x3, 3x2..." << endl;
	rectMult(4,3,3,2);

	// Following tests intended to target <= 32x32 tile boundaries, but also 64x64 to some degree
	cout << "   Rect 13x15, 15x17..." << endl;
	rectMult(13,15,15,17);
	cout << "   Rect 15x17, 17x19..." << endl;
	rectMult(15,17,17,19);

	cout << "   Rect 15x5, 5x11..." << endl;
	rectMult(15,5,5,11);

	cout << "   Rect 29x31, 31x33..." << endl;
	rectMult(29,31,31,33);
	cout << "   Rect 31x33, 33x35..." << endl;
	rectMult(31,33,33,35);

	cout << "   Rect 153x56, 56x87..." << endl;
	rectMult(153,56,56,87);
	cout << "   Rect 74x49, 49x169..." << endl;
	rectMult(74,49,49,169);

	cout << "   Rect 1021x1022, 1022x1023..." << endl;
	rectMult(1021,1022,1022,1023);

	cout << "   Rect 1x2, 3x4..." << endl;
	rectMult(1,2,3,4);

	//ACE_Time_Value endTime = ACE_OS::gettimeofday();
	//cout << "Tests Took: " << (endTime.get_msec() - startTime.get_msec())*1e-3 << " seconds" << endl;
	//cout << "   Too big..." << endl;
	//tooBig();
}

MatrixTester::~MatrixTester() {
	delete worker;
}


void MatrixTester::niceSquareMult(unsigned int length, bool verbose) {
	Result res;
	bool passed = true;
	unsigned int elements = length*length;

	std::vector<float> matA(elements);
	std::vector<float> matB(elements);
	std::vector<float> matC(elements);

	for (unsigned int index=0; index<elements; index++) matA[index]=0.0;
	for (unsigned int index=0; index<elements; index++) matB[index]=0.0;
	for (unsigned int index=0; index<elements; index++) matC[index]=-1.0;

	

	if (!worker->MatrixMult(matA, matB, matC)) {
		res = FAIL;
	} else {
		res = checkMultiplication(matA.data(), matB.data(), matC.data(), length, length, length);
	}
	recordTest(res, "Multiplication of zeroed square matrix of size " + test::to_string(length) + "x" + test::to_string(length));

	for (unsigned int index=0; index<elements; index++) matA[index]=(float)index;
	for (unsigned int index=0; index<elements; index++) matB[index]=(float)index;
	for (unsigned int index=0; index<elements; index++) matC[index]= numeric_limits<float>::signaling_NaN();

	if (!worker->MatrixMult(matA, matB, matC)) {
		res = FAIL;
	} else {
		res = checkMultiplication(matA.data(), matB.data(), matC.data(), length, length, length);
	}
	recordTest(res, "Multiplication of elemnt-wise-incremental square matrix of size " + test::to_string(length) + "x" + test::to_string(length));

	srand(RANDSEED);
	for (unsigned int index=0; index<elements; index++) matA[index]=(float)(rand()%1000000000)/100 - 1000000;
	for (unsigned int index=0; index<elements; index++) matB[index]=(float)(rand()%1000000000)/100 - 1000000;
	for (unsigned int index=0; index<elements; index++) matC[index]= numeric_limits<float>::signaling_NaN();

	if (!worker->MatrixMult(matA, matB, matC)) {
		res = FAIL;
	} else {
		res = checkMultiplication(matA.data(), matB.data(), matC.data(), length, length, length);
	}
	recordTest(res, "Multiplication of pseudo-randomized square matrix of size " + test::to_string(length) + "x" + test::to_string(length));

}


void MatrixTester::rectMult(unsigned int rowsA, unsigned int colsA, unsigned int rowsB, unsigned int colsB, bool verbose) {
	Result res;
	bool passed = true;
	unsigned int lenA = rowsA*colsA;
	unsigned int lenB = rowsB*colsB;
	unsigned int lenC = rowsA*colsB;

	std::vector<float> matA(lenA);
	std::vector<float> matB(lenB);
	std::vector<float> matC(lenC);


	for (unsigned int index=0; index<lenA; index++) matA[index]=0.0;
	for (unsigned int index=0; index<lenB; index++) matB[index]=0.0;
	//for (unsigned int index=0; index<lenC; index++) matC[index]=-1.0;
	for (unsigned int index=0; index<lenC; index++) matC[index]= numeric_limits<float>::signaling_NaN();

	
	//WE_UTE_Vector outVec = worker->MatrixMult(WE_UTE_Vector(matA, lenA), WE_UTE_Vector(matB, lenB), WE_UTE_Vector(matC, lenC));
	if (!worker->MatrixMult(matA, matB, matC)) {
		if (colsA != rowsB) {
			res = PASS;
		} else {
			res = FAIL;
		}
	} else {
		res = checkMultiplication(matA.data(), matB.data(), matC.data(), rowsA, colsA, colsB);
	}
	recordTest(res, "Multiplication of zeroed matrix of size " + test::to_string(rowsA) + "x" + test::to_string(colsA) + " by " + test::to_string(rowsB) + "x" + test::to_string(colsB));

	// Check incremental matrix
	for (unsigned int index=0; index<lenA; index++) matA[index]=(float)index;
	for (unsigned int index=0; index<lenB; index++) matB[index]=(float)index;
	for (unsigned int index=0; index<lenC; index++) matC[index]= numeric_limits<float>::signaling_NaN();

	if (!worker->MatrixMult(matA, matB, matC)) {
		if (colsA != rowsB) {
			res = PASS;
		} else {
			res = FAIL;
		}
	} else {
		res = checkMultiplication(matA.data(), matB.data(), matC.data(), rowsA, colsA, colsB);
	}
	recordTest(res, "Multiplication of element-wise-incrementing matrix of size " + test::to_string(rowsA) + "x" + test::to_string(colsA) + " by " + test::to_string(rowsB) + "x" + test::to_string(colsB));

	// Check randomly initialised matrix
	srand(RANDSEED);
	for (unsigned int index=0; index<lenA; index++) matA[index]=(float)(rand()%1000000000)/100 - 1000000;
	for (unsigned int index=0; index<lenB; index++) matB[index]=(float)(rand()%1000000000)/100 - 1000000;
	for (unsigned int index=0; index<lenC; index++) matC[index]= numeric_limits<float>::signaling_NaN();

	if (!worker->MatrixMult(matA, matB, matC)) {
		if (colsA != rowsB) {
			res = PASS;
		} else {
			res = FAIL;
		}
	} else {
		res = checkMultiplication(matA.data(), matB.data(), matC.data(), rowsA, colsA, colsB);
	}
	recordTest(res, "Multiplication of pseudo-random matrix of size " + test::to_string(rowsA) + "x" + test::to_string(colsA) + " by " + test::to_string(rowsB) + "x" + test::to_string(colsB));

}
/*
void MatrixTester::tooBig(bool verbose) {
	Result res;
	size_t gpuMem = worker->memCapacity();
	std::cout << "Available Memory: " << gpuMem / 1024.0 / 1024.0 << " Megabytes" << std::endl;
	unsigned int elements = (gpuMem/64)/sizeof(float)/2*64;
	//elements = elements*64;
	if (gpuMem == 0) {
		res = SKIPPED;
	} else {
		// Try to allocate a large enough matrix  that the GPU wont be able buffer it
		try {
			float* bigMat = new float[elements];
			float* outMat = new float[64*64];
			//WE_UTE_Vector bigVec = WE_UTE_Vector(bigMat, elements);
			//WE_UTE_Vector outVec = WE_UTE_Vector(outMat, 64*64);


			worker->MatrixMult(bigMat, bigMat, outMat);
			if (outMat == NULL) {
				res = PASS;
			} else {
				res = FAIL;
			}
		} catch (std::bad_alloc &allocError) {
			res = SKIPPED;
		}
	}
	recordTest(res, "Reports that matrices are too large for GPU");
}
*/

Result MatrixTester::checkMultiplication(float* matA, float* matB, float* matC, unsigned int m, unsigned int k, unsigned int n, bool verbose) {
	Result res = UNKNOWN;

	for (unsigned int col=0; col<n; col++) {
		for (unsigned int row=0; row<m; row++) {
			
			float accum = 0;
			for (unsigned int t=0; t<k; t++) {
				accum += matA[t + row*k]*matB[col + t*n];
			}

			if (!CHECK_FLOAT(accum, matC[col + row*n], EPS)) {
				if (verbose) {
					std::cout << "At pos [" << row << ',' << col << "] expected: " << accum << ", result: " << matC[col + row*n] << std::endl;
					std::cout << matA[row*k] << ", " << matB[col] << std::endl;
					res = FAIL;
				} else { // No need to keep checking if we're not printing the info...
					return FAIL;
				}
			}
		}
	}

	if (res != FAIL) res = PASS;

	return res;
}
