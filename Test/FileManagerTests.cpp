#include <FileManager.h>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <catch2/catch_all.hpp>
#include "../TextEditorCore/FileIOListener.h"

#include <functional>
#include <thread>
#include <chrono>
#include <atomic>
#include <condition_variable>
#include <mutex>

class TestFileManagerListener: public FileIOListener
{
public:
    std::function<void (const std::string& )> onIOStartCallback;
    std::function<void (const std::string& , FileIOListener::FileIOErrorsEnum)> onIOErrorCallback;
    std::function<void (const size_t )> onProgressCallback;
    std::function<void (void)> onPauseCallback;
    std::function<void (void)> onResumeCallback;
    std::function<void (void)> onStopCallback;
    std::function<void (const std::string&, std::string&)> onLoadCompleteCallback;
    std::function<void (const std::string&)> onSaveCompleteCallback;

    void onIOStart(const std::string& filename) override { onIOStartCallback(filename);};
    void onIOError(const std::string& failedArguments, FileIOErrorsEnum error) override { onIOErrorCallback(failedArguments, error);};
    void onProgress(const size_t percent) override { onProgressCallback(percent);};
    void onPause() override { onPauseCallback();};
    void onResume() override { onResumeCallback();};
    void onStop() override { onStopCallback();};
    void onLoadComplete(const std::string& fileName, std::string& dataBuffer) override { onLoadCompleteCallback(fileName, dataBuffer);};
    void onSaveComplete(const std::string& fileName) override { onSaveCompleteCallback(fileName);};

    ~TestFileManagerListener() override{};
    TestFileManagerListener(){};
};


bool isFileExist (const std::string& name)
{
    std::ifstream f(name.c_str());
    return f.good();
}

// test loadFile with file wich dosen`t exist 
// TEST_CASE("FileManager::loadFile()--FileDNExist", "[FileManager::loadFile()--FileDNExist]")
// {
//     bool isSaveStarted = false;
//     std::string initFileName = "errorFileDNExistTest.txt";
//     std::string dataBuffer = "1234676";

//     if (isFileExist (initFileName))
//     {
//         if (remove(initFileName.c_str()))
//         {
//             REQUIRE(false);
//             return;
//         }
//     }

//     std::condition_variable condition;

//     TestFileManagerListener testListener;
//     testListener.onIOStartCallback = 
//         [&](const std::string& fileName)
//         { 
//             REQUIRE(false);
//             condition.notify_all();
//         };
//     testListener.onLoadCompleteCallback = 
//         [&](const std::string& fileName, std::string& dataBuffer)
//         {
//             REQUIRE(false);
//             condition.notify_all();
//         };
//     testListener.onIOErrorCallback = 
//         [&](const std::string& fileName, int errorCode)
//         {
//             REQUIRE(initFileName == fileName);
//             REQUIRE(errorCode == FileIOListener::FileDNExist);
//             condition.notify_all();
//         };
    
//     TextEditorCore::FileManager fileManager;
//     fileManager.loadFile(initFileName, dataBuffer, testListener);
//     std::mutex mtx;
//     std::unique_lock lock(mtx);
//     condition.wait(lock, [](){return true;});
// }


TEST_CASE("FileManager::saveFile()--FileUnavailable", "[FileManager::saveFile()--FileUnavailable]")
{
    bool isSaveStarted = false;
    bool isRewrite  = true;
    std::string initFileName = "errorFileUnavailableTest.txt";
    std::string dataBuffer = "1234676";
    std::ifstream fileHolder(initFileName);
    std::condition_variable condition;

    if (fileHolder.is_open())
    {
        REQUIRE(false);
        return;
    }

    bool canStop = false;
    TextEditorCore::FileManager fileManager;
    TestFileManagerListener testListener;
    testListener.onIOStartCallback = [&](const std::string& fileName)
    {
        REQUIRE(false);
        canStop = true;
        condition.notify_all();
    };
    testListener.onSaveCompleteCallback = [&](const std::string& fileName)
    {
        REQUIRE(false);
        canStop = true;
        condition.notify_all();
    };
    testListener.onIOErrorCallback = [&](const std::string& fileName, FileIOListener::FileIOErrorsEnum errorCode)
    {
        REQUIRE(initFileName == fileName);
        REQUIRE(errorCode == FileIOListener::FileUnavailable);
        canStop = true;
        condition.notify_all();
    };
    
    fileManager.saveFile(initFileName, dataBuffer, isRewrite, testListener);
    std::mutex mtx;
    std::unique_lock lock(mtx);
    condition.wait(lock, [&](){return canStop;});
}


TEST_CASE("FileManager::saveFile()--saveSuccess", "[FileManager::saveFile()--saveSuccess]")
{
    bool isSaveStarted = false;
    bool isRewrite = false;
    const std::string initFileName = "saveFileTest.txt";
    std::string dataBuffer = "1234567";
    
    std::condition_variable condition;

    TextEditorCore::FileManager fileManager;
    TestFileManagerListener testListener;
    bool canStop = false;
    testListener.onIOStartCallback = [&](const std::string& fileName)
        {
            isSaveStarted = true;
            REQUIRE(initFileName == fileName);
            // condition.notify_all();
        };
    testListener.onProgressCallback = [](const size_t){};
    testListener.onSaveCompleteCallback = [&](const std::string& fileName)
        {
            REQUIRE(isSaveStarted);
            REQUIRE(initFileName == fileName);
            std::fstream file(fileName);
            
            if (!file.is_open())
            {
                REQUIRE(false);
            }                             
            
            std::string buffer;
            file >> buffer;
            file.close();                               
            remove(fileName.c_str());

            REQUIRE(buffer == dataBuffer);
            canStop = true;
            condition.notify_all();
        };
    testListener.onIOErrorCallback = 
        [&](const std::string& fileName, FileIOListener::FileIOErrorsEnum errorCode)
        {
            REQUIRE(initFileName == fileName);
            REQUIRE(false);
            canStop = true;
            condition.notify_all();
        };

    fileManager.saveFile(initFileName, dataBuffer, isRewrite, testListener);
    std::mutex mtx;
    std::unique_lock lock(mtx);
    condition.wait(lock, [&](){return canStop;});
}

// TEST_CASE("FileManager::loadFile()--loadSuccess", "[FileManager::loadFile()--loadSuccess]")
// {
//     bool isLoadStarted = false;
//     std::string initFileName = "loadFileTest.txt";
//     std::string initDataBuffer = "1234676";

//     std::condition_variable condition;

//     std::ofstream testFile(initFileName);
//     testFile << initDataBuffer;
//     testFile.close();

//     TextEditorCore::FileManager fileManager;
//     TestFileManagerListener testListener;

//     testListener.onIOStartCallback = [&](const std::string& fileName)
//     {
//         isLoadStarted = true;
//         REQUIRE(initFileName == fileName);
//         condition.notify_all();
//     };
//     testListener.onLoadCompleteCallback = [&](const std::string& fileName, std::string& dataBuffer )
//     {
//         REQUIRE(initFileName == fileName);
//         REQUIRE(isLoadStarted);                      
//         REQUIRE(initDataBuffer == dataBuffer);
//         remove(fileName.c_str());
//         condition.notify_all();
//     };
//     testListener.onIOErrorCallback = [&](const std::string& fileName, FileIOListener::FileIOErrorsEnum errorCode)
//     {
//         REQUIRE(false);
//         condition.notify_all();
//     };

//     fileManager.loadFile(initFileName, initDataBuffer, testListener);
//     std::mutex mtx;
//     std::unique_lock lock(mtx);
//     condition.wait(lock, [](){return true;});
// }


// TEST_CASE("FileManager::pause()--loadSuccess", "[FileManager::pause()--loadSuccess]")
// {
//     bool isLoadStarted = false;
//     std::string initFileName = "loadFileTest.txt";
//     std::string initDataBuffer = "1234676";

//     std::condition_variable condition;

//     std::ofstream testFile(initFileName);
//     testFile << initDataBuffer;
//     testFile.close();

//     TextEditorCore::FileManager fileManager;
//     TestFileManagerListener testListener;

//     std::atomic<int> testValue(0);
//     REQUIRE(testValue == 0);
//     testListener.onIOStartCallback =
//         [&](const std::string& filename)
//         {
//             // std::this_thread::sleep_for(std::chrono::microseconds(100));
//         };
//     testListener.onPauseCallback = 
//         [&]()
//         {
//             // testValue = 1;
//             // condition.notify_one();
//         };
//     testListener.onResumeCallback = 
//         [&]()
//         {
//             // REQUIRE(testValue == 1);
//             // testValue = 2;
//         };
//     testListener.onSaveCompleteCallback = 
//         [&](const std::string& fileName)
//         {
//             // REQUIRE(testValue == 2);
//             condition.notify_one();
//         };
//     testListener.onIOErrorCallback = 
//         [&](const std::string& failedArguments, FileIOListener::FileIOErrorsEnum error)
//         {
//             REQUIRE(false);
//         };
    

//     TextEditorCore::FileManager fileManager;
//     fileManager.loadFile(initFileName, initDataBuffer, testListener);
//     // fileManager.pause(testListener);
//     std::mutex mtx;
//     std::unique_lock lock(mtx);
//     // condition.wait(lock, [](){return true;});
//     // std::this_thread::sleep_for(std::chrono::microseconds(100));
//     // fileManager.resume(testListener);
//     condition.wait(lock, [](){return true;});
// }
