#include <iostream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
using std::shared_ptr;
int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}