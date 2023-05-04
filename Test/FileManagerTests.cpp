#include <FileManager.h>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <catch2/catch_all.hpp>

static bool isFileExist (const std::string& name)
{
    std::ifstream f(name.c_str());
    return f.good();
}

TEST_CASE("FileManager::loadFile()--FileDNExist", "[FileManager::loadFile()--FileDNExist]")
{
    bool isSaveStarted = false;
    std::string initFileName = "errorFileDNExistTest.txt";
    std::string dataBuffer = "1234676";

    if (isFileExist (initFileName))
    {
        if (remove(initFileName.c_str()))
        {
            REQUIRE(false);
            return;
        }
    }

    TextEditorCore::FileManager fileManager;

    fileManager.loadFile(initFileName, dataBuffer,
                        [&](const std::string& fileName)
                        {
                            REQUIRE(false);
                        },
                        [&](const std::string& fileName, std::string& dataBuffer)
                        {
                            REQUIRE(false);
                        },
                        [&](const std::string& fileName, int errorCode)
                        {
                            REQUIRE(initFileName == fileName);
                            REQUIRE(errorCode == TextEditorCore::FileManager::FileIOErrorsEnum::FileDNExist);

                        });
}

TEST_CASE("FileManager::saveFile()--FileUnavailable", "[FileManager::saveFile()--FileUnavailable]")
{
    bool isSaveStarted = false;
    std::string initFileName = "errorFileUnavailableTest.txt";
    std::string dataBuffer = "1234676";
    static std::ifstream fileHolder(initFileName);

    if (!fileHolder.is_open())
    {
        REQUIRE(false);
        return;
    }

    TextEditorCore::FileManager fileManager;

    fileManager.saveFile(initFileName, dataBuffer, false,
                        [&](const std::string& fileName)
                        {
                            fileHolder.close();
                            REQUIRE(false);
                        },
                        [&](const std::string& fileName)
                        {
                            fileHolder.close();
                            REQUIRE(false);
                        },
                        [&](const std::string& fileName, TextEditorCore::FileManager::FileIOErrorsEnum errorCode)
                        {
                            fileHolder.close();
                            REQUIRE(initFileName == fileName);
                            REQUIRE(errorCode == TextEditorCore::FileManager::FileIOErrorsEnum::FileUnavailable);
                        });

}

TEST_CASE("FileManager::saveFile()--saveSuccess", "[FileManager::saveFile()--saveSuccess]")
{
    bool isSaveStarted = false;
    std::string initFileName = "saveFileTest.txt";
    std::string dataBuffer = "1234676";

    TextEditorCore::FileManager fileManager;

    fileManager.saveFile(initFileName, dataBuffer, false,
                        [&](const std::string& fileName)
                        {
                                isSaveStarted = true;
                                REQUIRE(initFileName == fileName);
                        },
                        [&](const std::string& fileName)
                        {
                                REQUIRE(isSaveStarted);
                                REQUIRE(initFileName == fileName);
                                std::ofstream file(fileName);
                                
                                if (!file.is_open())
                                {
                                    REQUIRE(false);
                                }                             
                              
                                std::stringstream buffer;
                                buffer << file.rdbuf();
                                file.close();                               
                                remove(fileName.c_str());
                                REQUIRE(buffer.str() == dataBuffer);

                        },
                        [&](const std::string& fileName, TextEditorCore::FileManager::FileIOErrorsEnum errorCode)
                        {
                            REQUIRE(initFileName == fileName);
                            REQUIRE(false);
                        });
}

TEST_CASE("FileManager::loadFile()--loadSuccess", "[FileManager::loadFile()--loadSuccess]")
{
    bool isLoadStarted = false;
    std::string initFileName = "loadFileTest.txt";
    std::string initDataBuffer = "1234676";

    std::ofstream testFile(initFileName);
    testFile << initDataBuffer;
    testFile.close();

    TextEditorCore::FileManager fileManager;

    fileManager.loadFile(initFileName, initDataBuffer,
                        [&](const std::string& fileName)
                        {
                            isLoadStarted = true;
                            REQUIRE(initFileName == fileName);
                        },
                        [&](const std::string& fileName, std::string& dataBuffer )
                        {
                            REQUIRE(initFileName == fileName);
                            REQUIRE(isLoadStarted);                      
                            REQUIRE(initDataBuffer == dataBuffer);
                            remove(fileName.c_str());
                        },
                        [&](const std::string& fileName, TextEditorCore::FileManager::FileIOErrorsEnum errorCode)
                        {
                            REQUIRE(false);
                        });
}

