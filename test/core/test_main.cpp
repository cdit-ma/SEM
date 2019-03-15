#include "gtest/gtest.h"

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  auto testing_return_code = RUN_ALL_TESTS();
  // Specifically return 0 to allow for distinguishing between tests failing and faulty tests
  return 0;
}