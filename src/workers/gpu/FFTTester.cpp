#include "FFTTester.h"

#include <boost/thread.hpp>
#include <boost/math/special_functions/round.hpp>
#include <iostream>
#include <stdlib.h>

#define FFT_EPS 1e-5

using namespace std;
using namespace test;

/**
 * Testing is done by checking magnitudes of the results (ie amplitude vs frequency graph)
 * so theres nothing checking that phase information is there (ie properly validating complex
 * output is properly phase shifted). As the library currently backing the FFT worker passes
 * its own set of tests, wrapper testing can afford to be a little lighter (for the moment)
 */
FFTTester::FFTTester(DebugLevel dLevel) : Tester(dLevel) {
	worker = new Gpu_Worker(0, "");

	verbosity = dLevel;
	//ACE_Time_Value startTime = ACE_OS::gettimeofday();


	testEmpty();

	testConstant(0);
	testConstant(1);
	testConstant(5);
	testConstant(-1);

	testImpulse(1.0, 0);
	testImpulse(4.0, 0);
	testImpulse(-1.0, 0);

	testImpulse(1.0, 1);
	testImpulse(1.0, 2);

	testSingleAlignedFrequency(1, 1.0, 0.0);
	testSingleAlignedFrequency(5, 1.0, 0.0);
	
	testSingleAlignedFrequency(1, 5.0, 0.0);
	testSingleAlignedFrequency(1, 2.7, 0.0);
	testSingleAlignedFrequency(1, -1.0, 0.0);

	testSingleAlignedFrequency(1, 1.0, 1);
	testSingleAlignedFrequency(1, 1.0, 5);
	testSingleAlignedFrequency(1, 1.0, 2.9);
	testSingleAlignedFrequency(1, 1.0, -1);

	testSingleAlignedFrequency(31, 1.0, 0);

 	//ACE_Time_Value endTime = ACE_OS::gettimeofday();
    //    cout << "Tests Took: " << (endTime.get_msec() - startTime.get_msec())*1e-3 << " seconds" << endl;

	/*testSingleMisalignedFrequency(1.0, 1.0, 0.0);
	testSingleMisalignedFrequency(1.5, 1.0, 0.0);*/

	//testSingleAlignedFrequency(1, 1.0, 0.0);
	//testSingleAlignedFrequency(1, 1.0, 0.0);
}

FFTTester::~FFTTester() {
	delete worker;
}

void FFTTester::newThreadFFT(std::vector<float> vec) {
	worker->FFT(vec);
}

void FFTTester::testEmpty() {
	Result res = UNKNOWN;

	std::vector<float> data;

	worker->FFT(data);

	

	// boost::thread emptyThread(&FFTTester::newThreadFFT, this, data);

	// if (emptyThread.timed_join(boost::posix_time::seconds(1))) {
	// 	res = PASS;
	// }
	// else {
	// 	res = FAIL; 
	// }

	recordTest(res, "Performing FFT on zero length vector");
}

void FFTTester::testConstant(float amplitude) {
	Result res = UNKNOWN;
	unsigned int length = 8;

	std::vector<float> data(8);
	for (unsigned int i=0; i< length; i++) {
		data[i] = amplitude;
	}

	worker->FFT(data);

	if (!CHECK_FLOAT(data[0], amplitude*length, FFT_EPS)){
		res = FAIL;
	}

	for (unsigned int i=1; i<length; i++) {
		if (data[i] != 0)  {
			res = FAIL;
			//cout << i << ": " << data[i] << endl;
		}
	}
	
	if (res != FAIL) {
		res = PASS;
	}

	recordTest(res, "Performing FFT on zeroed vector of length 8");
}

void FFTTester::testImpulse(float amplitude, unsigned int impulseIndex) {
	Result res = UNKNOWN;

	unsigned int length = 8;
	std::vector<float> data(8);
	for (unsigned int i=0; i<length; i++) {
		data[i] = (float) ((i==impulseIndex)? amplitude : 0.0);
	}

	worker->FFT(data);

	for (unsigned int i=0; i<length; i+=2) {
		if (!CHECK_FLOAT(sqrt(data[i]*data[i] + data[i+1]*data[i+1]), abs(amplitude), EPS))  {
			res = FAIL;
			cout << i << ": " << data[i] << " " << data[i+1] << "i" << endl;
		}
	}
	
	if (res != FAIL) {
		res = PASS;
	}

	recordTest(res, "Performing FFT on impulse vector of length 8 with amplitude " + test::to_string(amplitude));
}

void addFrequency(std::vector<float> &data, float frequency, float amplitude, float phaseShift) {
	for (unsigned int i=0; i<data.size(); i++) {
		data[i] += amplitude * (float)(cos((float)frequency*((float)i/data.size())*2*PI + phaseShift));
	}
}

void FFTTester::testSingleAlignedFrequency(unsigned int frequency, float amplitude, float phaseShift) {
	Result res = UNKNOWN;
	unsigned int length = 64;

	if (frequency==0 || frequency >= length/2) {
		if (verbosity >= NORMAL) cout << " !! FFT aligned frequency tests of waves should have freq > 0 and < length/2, check your test cases..." << endl;
		recordTest(res, "Performing FFT on " + test::to_string(frequency) + "Hz wave, " +test::to_string(length) + " samples, amplitude of " + test::to_string(amplitude));
		return;
	}

	std::vector<float> data(length);
	
	for (unsigned int i=0; i<length; i++) data[i]=0;
	addFrequency(data, (float)frequency, amplitude, phaseShift);
	worker->FFT(data);

	// Verify that the intended frequency bin has the right magnitude, while all others are 0
	for (unsigned int i=0; i<length; i+=2) {

		float magnitude = sqrt(data[i]*data[i] + data[i+1]*data[i+1]);
		float expected;

		if (verbosity==FULL) cout << i/2 << ": " << setprecision(5) << scientific << data[i] << " " << data[i+1] << "i,     mag = " << std::fixed << magnitude << endl;

		if (i/2 == frequency) {
			expected = abs(amplitude*length/2);
		} else {
			expected = 0;
		}

		if (!CHECK_FLOAT(magnitude, expected, FFT_EPS)) {
			res = FAIL;
			if (verbosity>=VERBOSE) {
				cout << "  At " << i/2 << "Hz: expected " << fixed << expected << ", got " << magnitude
					 << ", difference is " << magnitude - expected << endl;
			}
		}

	}
	
	if (res != FAIL) {
		res = PASS;
	}

	recordTest(res, "Performing FFT on aligned " + test::to_string(frequency) + "Hz wave, " +test::to_string(length) + " samples, amplitude of " + test::to_string(amplitude));
}

/*
// Hann function to reduce the effects of spectral leakage from misalisgned FFT
void windowFuncHann(unsigned int length, float* data) {
	for (unsigned int i=0; i<length; i++) {
		data[i] = data[i] * (1 - cos(2*PI*i / (length-1)) )/2;
	}
}

void FFTTester::testSingleMisalignedFrequency(float frequency, float amplitude, float phaseShift) {
	Result res = UNKNOWN;
	unsigned int length = 1024;

	if (frequency==0 || frequency >= length/2) {
		if (verbosity >= NORMAL) cout << " !! FFT aligned frequency tests of waves should have freq > 0 and < length/2, check your test cases..." << endl;
		recordTest(res, "Performing FFT on " + test::to_string(frequency) + "Hz wave, " +test::to_string(length) + " samples, amplitude of " + test::to_string(amplitude));
		return;
	}

	float* data = new float[length];
	float* wave = new float[length];
	
	WE_UTE_Vector waveVec(data, length);
	for (unsigned int i=0; i<length; i++) data[i]=0;
	addFrequency(length, data, (float)frequency, amplitude, phaseShift);
	windowFuncHann(length, data);
	for (unsigned int i=0; i<length; i++) wave[i]=data[i];
	worker->FFT(waveVec);

	float* reconstruction = new float[length];
	for (unsigned int i=0; i<length; i++) reconstruction[i]=0;
	
	// Reconstruct original samples from result
	for (unsigned int freq=0; freq<length/2; freq++) {
		float real = data[2*freq];
		float imaginary = data[2*freq + 1];
		float mag = sqrt(real*real + imaginary*imaginary);
		float phase = atan2f(imaginary, real);

		//cout << "amp = " << mag << ", phase = " << phase << endl;

		addFrequency(length, reconstruction, frequency, mag / (length/2), phase);
	}

	for (unsigned int i=0; i<length; i++) {

		if (!CHECK_FLOAT(wave[i], reconstruction[i], FFT_EPS)) {
			res = FAIL;
			if (verbosity>=VERBOSE) {
				cout << "  At index " << i << ": expected " << fixed << wave[i] << ", got " << reconstruction[i] << endl;
			}
		}

	}
	
	if (res != FAIL) {
		res = PASS;
	}

	recordTest(res, "Performing FFT on aligned " + test::to_string(frequency) + "Hz wave, " +test::to_string(length) + " samples, amplitude of " + test::to_string(amplitude));

	delete data;
}*/
