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

#include <iostream>
#include <algorithm> 

class TestFileManagerListener: public FileIOListener
{
public:
    std::function<void (const std::string& )> onIOStartCallback = [](const std::string& ){printf("onIOStartCallback\n");};
    std::function<void (const std::string& , FileIOListener::FileIOErrorsEnum)> onIOErrorCallback = [](const std::string& , FileIOListener::FileIOErrorsEnum){printf("onIOErrorCallback\n");};
    std::function<void (const float)> onProgressCallback = [](const float ){printf("onProgressCallback\n");};
    std::function<void (void)> onPauseCallback = [](){printf("onPauseCallback\n");};
    std::function<void (void)> onResumeCallback = [](){printf("onResumeCallback\n");};
    std::function<void (void)> onStopCallback = [](){printf("onStopCallback\n");};
    std::function<void (const std::string&, std::string&)> onLoadCompleteCallback = [](const std::string&, std::string&){printf("onLoadCompleteCallback\n");};
    std::function<void (const std::string&)> onSaveCompleteCallback = [](const std::string&){printf("onSaveCompleteCallback\n");};

    void onIOStart(const std::string& filename) override { onIOStartCallback(filename);};
    void onIOError(const std::string& failedArguments, FileIOErrorsEnum error) override { onIOErrorCallback(failedArguments, error);};
    void onProgress(const float percent) override { onProgressCallback(percent);};
    void onPause() override { onPauseCallback();};
    void onResume() override { onResumeCallback();};
    void onStop() override { onStopCallback();};
    void onLoadComplete(const std::string& fileName, std::string& dataBuffer) override { onLoadCompleteCallback(fileName, dataBuffer);};
    void onSaveComplete(const std::string& fileName) override { onSaveCompleteCallback(fileName);};

    virtual ~TestFileManagerListener() override{};
    TestFileManagerListener(){};
};


bool isFileExist (const std::string& name)
{
    std::ifstream f(name.c_str());
    return f.good();
}

// test loadFile with file wich dosen`t exist 
TEST_CASE("FileManager::loadFile()--FileDNExist", "[FileManager::loadFile()--FileDNExist]")
{
    bool isSaveStarted = false;
    std::atomic_bool isEnded = false;
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

    std::condition_variable condition;

    TestFileManagerListener testListener;
    testListener.onIOStartCallback = 
        [&](const std::string& fileName)
        { 
            REQUIRE(false);
            isEnded = true;
            condition.notify_all();
        };
    testListener.onLoadCompleteCallback = 
        [&](const std::string& fileName, std::string& dataBuffer)
        {
            REQUIRE(false);
            isEnded = true;
            condition.notify_all();
        };
    testListener.onIOErrorCallback = 
        [&](const std::string& fileName, int errorCode)
        {
            REQUIRE(initFileName == fileName);
            REQUIRE(errorCode == FileIOListener::FileDNExist);
            isEnded = true;
            condition.notify_all();
        };
    
    TextEditorCore::FileManager fileManager;
    fileManager.loadFile(initFileName, dataBuffer, testListener);

    std::mutex mtx;
    std::unique_lock lock(mtx);
    condition.wait(lock, [&](){return isEnded.load();});
}

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
    bool isRewrite = true;
    const std::string initFileName = "saveFileTest.txt";
    const std::string dataBuffer = "1234567";

    std::mutex mtx;
    std::condition_variable condition;
    bool canStop = false;

    
    
    TestFileManagerListener testListener;
    testListener.onProgressCallback = [](const float progress)
    {
        REQUIRE(static_cast<int>(progress) == 100);

    };
    testListener.onIOStartCallback = [&](const std::string& fileName)
        {
            isSaveStarted = true;
            REQUIRE(initFileName == fileName);
            // condition.notify_all();
        };
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

    TextEditorCore::FileManager fileManager;
    for(size_t i = 0; i < 10; i++)
    {
        canStop = false;
        fileManager.saveFile(initFileName, dataBuffer, isRewrite, testListener);
        std::unique_lock lock(mtx);
        condition.wait(lock, [&](){return canStop;});
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

TEST_CASE("FileManager::loadFile()--loadSuccess", "[FileManager::loadFile()--loadSuccess]")
{
    bool isLoadStarted = false;
    std::string initFileName = "loadFileTest.txt";
    std::string initDataBuffer = "1234676";
    std::condition_variable condition;

    bool isCanStop = false;
    TestFileManagerListener testListener;
    testListener.onProgressCallback = [](const size_t){};

    testListener.onIOStartCallback = [&](const std::string& fileName)
    {
        isLoadStarted = true;
        REQUIRE(initFileName == fileName);
    };
    testListener.onLoadCompleteCallback = [&](const std::string& fileName, std::string& dataBuffer )
    {
        REQUIRE(initFileName == fileName);
        REQUIRE(isLoadStarted);   

        REQUIRE(initDataBuffer == dataBuffer);
        remove(fileName.c_str());
        isCanStop = true;
        condition.notify_all();
    };
    testListener.onIOErrorCallback = [&](const std::string& fileName, FileIOListener::FileIOErrorsEnum errorCode)
    {
        REQUIRE(false);
        isCanStop = true;
        condition.notify_all();
    };

    TextEditorCore::FileManager fileManager;
    for(size_t i = 0; i < 100; i++){

        std::ofstream testFile(initFileName);
        testFile << initDataBuffer;
        testFile.close();
        isCanStop = false;
        fileManager.loadFile(initFileName, initDataBuffer, testListener);

        std::mutex mtx;
        std::unique_lock lock(mtx);
        condition.wait(lock, [&](){return isCanStop;});
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

TEST_CASE("FileManager::pause()--loadSuccess", "[FileManager::pause()--loadSuccess]")
{
    std::atomic<int> testValue(0);
    std::atomic<bool> isContinueThread = false;
    std::atomic<bool> isContinueTest = false;
    std::condition_variable condition;
    std::mutex mtx;

    std::string initFileName = "loadFileTest.txt";
    std::string initDataBuffer = "1234567";
    
    TestFileManagerListener testListener;
    testListener.onIOStartCallback =
        [&](const std::string& filename)
        {
            testValue = 0;
            isContinueTest.store(true);

            condition.notify_one();
            std::unique_lock lock(mtx);
            condition.wait(lock, [&](){return isContinueThread.load() == true;});
            isContinueThread = false;
        };
    testListener.onPauseCallback = 
        [&]()
        {
            REQUIRE(testValue == 0);
            testValue = 1;
            isContinueTest.store(true);
            condition.notify_one();
        };
    testListener.onResumeCallback = 
        [&]()
        {
            REQUIRE(testValue == 1);
            testValue = 2;
        };
    testListener.onLoadCompleteCallback = 
        [&](const std::string& fileName, std::string& dataBuffer)
        {
            remove(fileName.c_str());
            REQUIRE(testValue == 2);
            isContinueTest.store(true);
            condition.notify_one();
        };
    testListener.onIOErrorCallback = 
        [&](const std::string& failedArguments, FileIOListener::FileIOErrorsEnum error)
        {
            REQUIRE(false);
            condition.notify_one();
            exit(0);
        };
    testListener.onProgressCallback = [](const size_t){};

    TextEditorCore::FileManager fileManager;

    for(size_t i = 0; i < 10; i++){
        std::ofstream testFile(initFileName);
        testFile << initDataBuffer;
        testFile.close();

        fileManager.loadFile(initFileName, initDataBuffer, testListener);
        
        {
            //whait until onIOStartCallback
            std::unique_lock lock(mtx);
            condition.wait(lock, [&](){return isContinueTest.load() == true;});
            isContinueTest.store(false);
        }

        fileManager.pause(testListener);
        isContinueThread.store(true);
        condition.notify_one();

        {
            //whait until onIOLoadPauseCallback 
            std::unique_lock lock(mtx);
            condition.wait(lock, [&](){return isContinueTest.load() == true;});
            isContinueTest.store(false);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        fileManager.resume(testListener);

        {
            //whait until onIOLoadCompliteCallback 
            std::unique_lock lock(mtx);
            condition.wait(lock, [&](){return isContinueTest == true;});
            isContinueTest.store(false);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

TEST_CASE("FileManager::pause()--saveSuccess", "[FileManager::pause()--saveSuccess]")
{
    bool isOverWriteAllowed = true;
    const std::string initFileName = "saveFileTest.txt";
    std::string dataBuffer = "1234567";

    std::atomic<int> testValue(0);
    std::atomic<bool> isContinueThread = false;
    std::atomic<bool> isContinueTest = false;
    std::condition_variable condition;
    std::mutex mtx;
    
    TestFileManagerListener testListener;
    testListener.onProgressCallback = [](const size_t){};
    testListener.onIOStartCallback =
        [&](const std::string& filename)
        {
            testValue = 0;
            isContinueTest.store(true);

            condition.notify_one();
            std::unique_lock lock(mtx);
            condition.wait(lock, [&](){return isContinueThread.load() == true;});
            isContinueThread = false;
        };
    testListener.onPauseCallback = 
        [&]()
        {
            REQUIRE(testValue == 0);
            testValue = 1;
            isContinueTest.store(true);
            condition.notify_one();
        };
    testListener.onResumeCallback = 
        [&]()
        {
            REQUIRE(testValue == 1);
            testValue = 2;
        };
    testListener.onSaveCompleteCallback = 
        [&](const std::string& fileName)
        {
            remove(fileName.c_str());
            REQUIRE(testValue == 2);
            isContinueTest.store(true);
            condition.notify_one();
        };
    testListener.onIOErrorCallback = 
        [&](const std::string& failedArguments, FileIOListener::FileIOErrorsEnum error)
        {
            REQUIRE(false);
            isContinueTest.store(true);
            condition.notify_one();
            exit(1);
        };


    TextEditorCore::FileManager fileManager;

    for(size_t i = 0; i < 10; i++)
    {
        fileManager.saveFile(initFileName, dataBuffer, isOverWriteAllowed, testListener);
        {
            //whait until onIOStartCallback
            std::unique_lock lock(mtx);
            condition.wait(lock, [&](){return isContinueTest.load() == true;});
            isContinueTest = false;
        }

        fileManager.pause(testListener);
        isContinueThread.store(true);
        condition.notify_one();

        {
            //whait until onIOLoadPauseCallback 
            std::unique_lock lock(mtx);
            condition.wait(lock, [&](){return isContinueTest.load() == true;});
            isContinueTest = false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        fileManager.resume(testListener);

        {
            //whait until onIOLoadCompliteCallback 
            std::unique_lock lock(mtx);
            condition.wait(lock, [&](){return isContinueTest == true;});
            isContinueTest.store(false);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

TEST_CASE("FileManager::saveFile()--ProgressValue", "[FileManager::saveFile()--ProgressValue]")
{
    bool isOverWriteAllowed = true;
    const std::string initFileName = "saveFileTest.txt";
    const size_t chunkSize = 4096;
    const size_t textSize = chunkSize * 5;
    std::string dataBuffer;
    dataBuffer.resize(textSize, 'a');


    std::vector<size_t> progressCheckValues;
    progressCheckValues.resize(5, 0);
    const float percentage = static_cast<float>(chunkSize)  / (static_cast<float>(textSize) / 100.0);
    
    for(size_t i = 0, value = percentage; i < progressCheckValues.size(); i++ , value += percentage)
    {
        progressCheckValues[i] = value;
    }
    auto itr = progressCheckValues.begin();

    std::atomic<bool> isContinueTest = false;
    std::condition_variable condition;
    std::mutex mtx;
    
    TestFileManagerListener testListener;
    testListener.onProgressCallback = [&](const size_t progres)
{
        if(itr == progressCheckValues.end())
        {
            remove(initFileName.c_str());
            REQUIRE(false);
            return;
        }
        REQUIRE(static_cast<size_t>(*itr) == static_cast<size_t>(progres));
        itr += 1;
    };
    testListener.onSaveCompleteCallback = 
        [&](const std::string& fileName)
        {
            remove(fileName.c_str());
            REQUIRE(itr == progressCheckValues.end());
            isContinueTest.store(true);
            condition.notify_one();
        };
    testListener.onIOErrorCallback = 
        [&](const std::string& failedArguments, FileIOListener::FileIOErrorsEnum error)
        {
            REQUIRE(false);
            isContinueTest.store(true);
            condition.notify_one();
        };

    TextEditorCore::FileManager fileManager;
    fileManager.saveFile(initFileName, dataBuffer, isOverWriteAllowed, testListener);
    //whait until onIOLoadCompliteCallback 
    std::unique_lock lock(mtx);
    condition.wait(lock, [&](){return isContinueTest == true;});
    
}

TEST_CASE("FileManager::loadFile()--ProgressValue", "[FileManager::pause()--ProgressValue]")
{

    std::atomic_bool isContinueTest = false;
    std::condition_variable condition;
    std::mutex mtx;

    const std::string initFileName = "loadFileTestProgress.txt";
    const size_t chunkSize = 4096;
    const size_t textSize = chunkSize * 2;
    std::string initDataBuffer;
    initDataBuffer.resize(textSize, 'a');

    std::vector<float> progressCheckValues;
    progressCheckValues.resize(2, 0);
    const float percentage = static_cast<float>(chunkSize)  / (static_cast<float>(textSize) / 100.0);
    
    for(size_t i = 0, value = percentage; i < progressCheckValues.size(); i++ , value += percentage)
    {
        progressCheckValues[i] = value;
    }
    auto itr = progressCheckValues.begin();
    
    TestFileManagerListener testListener;
    testListener.onSaveCompleteCallback =
        [&](const std::string& fileName)
        {
            isContinueTest.store(true);
            condition.notify_one();
        };
    testListener.onLoadCompleteCallback = 
        [&](const std::string& fileName, std::string& dataBuffer)
        {
            remove(fileName.c_str());
            REQUIRE(itr == progressCheckValues.end());
            isContinueTest.store(true);
            condition.notify_one();
        };
    testListener.onIOErrorCallback = 
        [&](const std::string& failedArguments, FileIOListener::FileIOErrorsEnum error)
        {
            remove(initFileName.c_str());
            REQUIRE(false);
            condition.notify_one();
            return;
        };
    testListener.onStopCallback =
        [&](){ REQUIRE(false);};
    TextEditorCore::FileManager fileManager;
    fileManager.saveFile(initFileName, initDataBuffer, false, testListener);
    {
        std::unique_lock lock(mtx);
        condition.wait(lock, [&](){return isContinueTest.load() == true;});
        isContinueTest.store(false);
    }

    testListener.onProgressCallback = [&](const size_t progres)
        {
            if(itr == progressCheckValues.end())
            {
                remove(initFileName.c_str());
                REQUIRE(false);
                return;
            }
            REQUIRE(static_cast<size_t>(*itr) == static_cast<size_t>(progres));
            itr += 1;
        };
    initDataBuffer.resize(0);
    fileManager.loadFile(initFileName, initDataBuffer, testListener);
    //whait until onIOLoadCompliteCallback 
    {
        std::unique_lock lock(mtx);
        condition.wait(lock, [&](){return isContinueTest.load() == true;});
    }
}

TEST_CASE("FileManager::pause()--pauseError__loadFile", "[FileManager::pause()--pauseError__loadFile]")
{
    TestFileManagerListener testListener;
        testListener.onProgressCallback = [&](const size_t progres)
        {
            REQUIRE(false);
        };
    testListener.onSaveCompleteCallback =
        [&](const std::string& fileName)
        {
           REQUIRE(false);
        };
    testListener.onLoadCompleteCallback = 
        [&](const std::string& fileName, std::string& dataBuffer)
        {
            REQUIRE(false);
        };
    testListener.onIOErrorCallback = 
        [&](const std::string& failedArguments, FileIOListener::FileIOErrorsEnum error)
        {
            REQUIRE(error == FileIOListener::PauseError);
        };
    
    TextEditorCore::FileManager fileManager;
    // fileManager.pause(testListener);
}