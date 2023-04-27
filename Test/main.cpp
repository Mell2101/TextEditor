#include <stdio.h>
#include <FileManager.h>

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

    TextEditorCore::FileManager test;
    test.saveFile("~/nikita/TestFile.txt", "aspofsd,gplsdgp", true, [&](const std::string& fn)
                                           {
                                             int a = 10;                                            
                                           },
                                           [&](const std::string& fn)
                                           {
                                             int a = 10;
                                           },
                                           [&](const std::string& fn, TextEditorCore::FileManager::FileIOErrorsEnum error)
                                           {
                                             int a = 10;
                                           }
                                           );


    printf("\nTESTS END\n");
    printf("=======================================\n");
    return 0;
}
