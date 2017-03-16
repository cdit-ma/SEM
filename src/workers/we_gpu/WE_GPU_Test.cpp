
#include <iostream>

#include "MatrixTester.h"
#include "FFTTester.h"

using namespace std;
using namespace test;

void printUsageInfo(const char* progName) {
	cerr << "Usage: " << progName << " [options]" << endl;
			cerr << "  options:" << endl;
			cerr << "    -h    Displays this help text" << endl;
			cerr << "    -s    Enables silent output (only reports failures)" << endl;
			cerr << "    -q    Enables quiet output (only reports failures and overall statistics)" << endl;
			cerr << "    -v    Enables verbose output (provides some extra failure information where available)" << endl;
			cerr << "    -vfull    Enables fully verbose output (provides all available information)" << endl;
}

int main(int argc, char** argv) {
	DebugLevel verbosity = NORMAL;

	for (int i=1; i<argc; i++) {

		string arg(argv[i]);
		if (arg.compare("-s") == 0) {
			verbosity = SILENT;
		}
		else if (arg.compare("-q") == 0) {
			verbosity = QUIET;
		}
		else if (arg.compare("-v") == 0) {
			verbosity = VERBOSE;
		}
		else if (arg.compare("-vfull") == 0) {
			verbosity = FULL;
		}
		else if (arg.compare("-h") == 0) {
			printUsageInfo(argv[0]);
			return 0;
		}
		else {
			printUsageInfo(argv[0]);
			return 1;
		}
	}

	int testsFailed = 0;

	MatrixTester* mTester = new MatrixTester(verbosity);
	//return 1;
	testsFailed += mTester->getTestsFailed();
	cout << "Matrix tests passed: " << mTester->getTestsPassed() << '/' << mTester->getTestsRun() << endl;
	cout << "Tests skipped: " << mTester->getTestsSkipped() << endl;
	delete mTester;

	FFTTester* fftTester = new FFTTester(verbosity);
	// testsFailed += fftTester->getTestsFailed();
	// cout << "FFT tests passed: " << fftTester->getTestsPassed() << '/' << fftTester->getTestsRun() << endl;
	// cout << "Tests skipped: " << fftTester->getTestsSkipped() << endl;
	// delete fftTester;
	return testsFailed;
}