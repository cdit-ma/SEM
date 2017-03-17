#ifndef TESTER_H
#define TESTER_H

#include <iostream>
#include <string>
#include <sstream>
#include <math.h>

#define RANDSEED 13520

#define EPS 1e-7
#define CHECK_FLOAT(x, y, eps) (fabs(x-y)<eps)

#define PI 3.1415926535897932384


namespace test {
 template <typename T>
    std::string to_string(T value)
    {
      //create an output string stream
      std::ostringstream os ;

      //throw the value into the string stream
      os << value ;

      //convert the string stream into a string and return
      return os.str() ;
    }

enum Result {
	UNKNOWN,
	PASS,
	FAIL,
	SKIPPED
};

std::string resultToString(Result res);

enum DebugLevel {
	SILENT = 0,
	QUIET = 1,
	NORMAL = 2,
	VERBOSE = 3,
	FULL = 4
};

class Tester {
public:
	Tester(DebugLevel dLevel = NORMAL) : passed(0), failed(0), skipped(0), verbosity(dLevel) {}
	unsigned int getTestsPassed() const { return passed;}
	unsigned int getTestsFailed() const {return failed;}
	unsigned int getTestsSkipped() const {return skipped;}
	unsigned int getTestsRun() const {return (passed+failed);}
	void testPassed() {passed++;}
	void testFailed() {failed++;}
	void recordTest(Result result, std::string description) {
		switch (result) {
		case PASS: testPassed(); break;
		case FAIL: testFailed(); break;
		case SKIPPED: break;
		}

		switch(verbosity) {
		case SILENT: return;
		case QUIET: if (result == PASS) return;
		default: break;
		}
		std::cout << "[" << resultToString(result) << "] " << description << std::endl;
	}

protected:
	DebugLevel verbosity;

private:
	unsigned int passed;
	unsigned int failed;
	unsigned int skipped;
};

}; // namespace test

#endif
