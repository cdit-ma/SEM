#ifndef MATRIXTESTER_H
#define MATRIXTESTER_H

#include "Tester.h"
#include "WE_GPU.h"

class MatrixTester : public test::Tester {
public:
	MatrixTester(test::DebugLevel dLevel= test::NORMAL);
	~MatrixTester();
	void niceSquareMult(unsigned int length, bool verbose=true);
	void rectMult(unsigned int rowsA, unsigned int colsA, unsigned int rowsB, unsigned int colsB, bool verbose=true);
	void tooBig(bool verbose=true);
	test::Result checkMultiplication(float* matA, float* matB, float* matC, unsigned int m, unsigned int k, unsigned int n, bool verbose = true);
	
private:
	WE_GPU *worker;
};

#endif
