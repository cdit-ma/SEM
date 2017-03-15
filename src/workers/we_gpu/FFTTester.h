#ifndef FFTTESTER_H
#define FFTTESTER_H

#include "Tester.h"
#include "WE_GPU.h"

class FFTTester : public test::Tester {
public:
	FFTTester(test::DebugLevel dLevel= test::NORMAL);
	~FFTTester();

	void newThreadFFT(WE_UTE_Vector vec);

	void testEmpty();
	void testConstant(float amplitude);
	void testImpulse(float amplitude, unsigned int impulseIndex);
	void testSingleAlignedFrequency(unsigned int frequency, float amplitude, float phase);
	//void testSingleMisalignedFrequency(float frequency, float amplitude, float phase);
	//void testMultipleFrequencies(float* frequencies, float* amplitude, float* phaseShifts);
	
private:
	WE_GPU *worker;
};

#endif
