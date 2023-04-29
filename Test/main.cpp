#include <stdio.h>

// =============================================================================

namespace TextManagerTest
{
void defaultContructorTest();
void stringContructorTest();
} //namespace TextManagerTest

// =============================================================================

int main(int argc, char const *argv[])
{
    printf("=======================================\n");
    printf("TESTS BEGIN\n\n");

    TextManagerTest::defaultContructorTest();
    TextManagerTest::stringContructorTest();



    printf("\nTESTS END\n");
    printf("=======================================\n");
    return 0;
}
