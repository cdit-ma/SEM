#ifndef FFTTESTER_H
#define FFTTESTER_H

#include "Tester.h"
#include "gpu_worker.h"

class FFTTester : public test::Tester {
public:
	FFTTester(test::DebugLevel dLevel= test::NORMAL);
	~FFTTester();

	void newThreadFFT(std::vector<float> vec);

	void testEmpty();
	void testConstant(float amplitude);
	void testImpulse(float amplitude, unsigned int impulseIndex);
	void testSingleAlignedFrequency(unsigned int frequency, float amplitude, float phase);
	//void testSingleMisalignedFrequency(float frequency, float amplitude, float phase);
	//void testMultipleFrequencies(float* frequencies, float* amplitude, float* phaseShifts);
	
private:
	Gpu_Worker *worker;
};

#endif
