#include <gtest/gtest.h>

int main (int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    int returnValue;

    // Setup tests
    // TODO

    returnValue =  RUN_ALL_TESTS();

    // Teardown
    // TODO

    return returnValue;
}
