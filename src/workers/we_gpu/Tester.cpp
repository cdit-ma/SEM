
#include "Tester.h"

using namespace test;

std::string test::resultToString(Result res) {
	switch (res) {
	case UNKNOWN: return "UNKNOWN";
	case PASS: return "PASS";
	case FAIL: return "FAIL";
	case SKIPPED: return "SKIPPED";
	default: return "UNEXPECTED RESULT TYPE";
	}
}