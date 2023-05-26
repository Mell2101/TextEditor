#include "FileManager.h"


/* -------------------------------------------------------------------------- */
//PIMPL
#include <iostream>
#include <fstream>
#include <filesystem>
#include <unordered_map>
 

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

#include <chrono>

namespace TextEditorCore
{

struct FileManager::PImpl
{
private:
    std::atomic<bool> m_pausFlag = false;
    std::atomic<bool> m_stopWorkFlag = false;
    std::mutex m_fileManagerMutex;
    std::condition_variable m_pauseCondition;
    const size_t m_chunkSize;

    // TODO: ADD DEFINES
    std::chrono::milliseconds m_testDelayTime;
public:
    PImpl(const size_t chunkSize): m_chunkSize(chunkSize){}

private:
    
    void loadFileFunction(const std::string& filePath,
                        std::string& dataBuffer,
                        FileIOListener& listener
                        );

    void saveFileFunction(const std::string& filePath,
                        const std::string& dataBuffer,
                        FileIOListener& listener
                        );

public:
    void loadFile(const std::string& filePath,
                std::string& dataBuffer,
                FileIOListener& listner
                );

    void resume(FileIOListener&);
    
    void pause(FileIOListener&);

    void stopWork(FileIOListener&);
    
    void saveFile(const std::string& filePath,
                const std::string& dataBuffer,
                const bool isRewrite,
                FileIOListener& listener
                );
                  
    FileIOListener::FileIOErrorsEnum checkPath(const std::string& filePath, bool isLoad, bool isExist);
    
};


void FileManager::PImpl::loadFile(const std::string& filePath,
                                std::string& dataBuffer,
                                FileIOListener& listner
                                )
{
    if(!std::filesystem::exists(filePath))
    {
        listner.onIOError(filePath, FileIOListener::FileDNExist);
        return;
    }
    std::thread loadThread(&FileManager::PImpl::loadFileFunction,
                            this,
                            std::ref(filePath),
                            std::ref(dataBuffer),
                            std::ref(listner)
                        );
    loadThread.detach();
}



void FileManager::PImpl::loadFileFunction(const std::string& filePath,
                                        std::string& dataBuffer,
                                        FileIOListener& listner
                                        )
{

    std::ifstream file(filePath);

    if (file.is_open())
    {
        listner.onIOStart(filePath);
        // get length of file:
        file.seekg (0, file.end);
        int length = static_cast<int>(file.tellg());
        file.seekg (0, file.beg);
        const float percentage = static_cast<float>(m_chunkSize)  / (static_cast<float>(length) / 100.0);
        float percentageRead = 0;

        dataBuffer.clear();
        dataBuffer.resize(length);

        while(!file.eof())
        {                
            if(m_stopWorkFlag)
                break;
            if(m_pausFlag)
            {
                listner.onPause();
                std::unique_lock lock(m_fileManagerMutex);
                m_pauseCondition.wait(lock, [a = &m_stopWorkFlag , b = &m_pausFlag ]()
                    {return (a->load() || !b->load());});
                listner.onResume();
                if(m_stopWorkFlag)
                    break;
            }
            file.read(dataBuffer.data() + file.tellg(), m_chunkSize);
            if(file.fail() && !file.eof())
            {
                listner.onIOError(filePath, FileIOListener::FileReadError);
                return;
            }
            percentageRead += percentage;
            listner.onProgress(percentageRead);
#ifdef  TESTING
            std::this_thread::sleep_for(std::chrono::milliseconds(m_testDelayTime)); 
#endif //  TESTING
        }
    }
    else
    {
        listner.onIOError(filePath, FileIOListener::FileUnavailable);
        return;
    }
    
    if(m_stopWorkFlag){
        listner.onStop();
        return;
    }
    listner.onLoadComplete(filePath, dataBuffer);
}

void FileManager::PImpl::pause(FileIOListener& listener)
{
    if(m_pausFlag == true || m_stopWorkFlag)
    {
        listener.onIOError("", FileIOListener::FileWriteError);
        return;
    }
    m_pausFlag = true;
}

void FileManager::PImpl::resume(FileIOListener& listener)
{
    if(!m_pausFlag && m_stopWorkFlag)
    {
        listener.onIOError("", FileIOListener::FileUnavailable);
        return;
    }
    m_pausFlag = false;
    m_pauseCondition.notify_one();
}

void FileManager::PImpl::stopWork(FileIOListener&listener)
{
    m_pausFlag = true;
    m_pauseCondition.notify_one();
}

void FileManager::PImpl::saveFile(const std::string& filePath,
                                const std::string& dataBuffer,
                                const bool isOverWriteAllowed,
                                FileIOListener& listener
                                )
{    
    if(isOverWriteAllowed == false && std::filesystem::exists(filePath))
    {
        listener.onIOError(filePath, FileIOListener::FileReWriteTaboo);
        return;
    }

    std::thread SaveThread(&FileManager::PImpl::saveFileFunction,
                            this,
                            std::ref(filePath),
                            std::ref(dataBuffer),
                            std::ref(listener)
                        );
    SaveThread.detach();
}

void FileManager::PImpl::saveFileFunction(const std::string& filePath,
                                  const std::string& dataBuffer,
                                  FileIOListener& listener
                                  )
{
    std::ofstream file(filePath, std::ios::out | std::ios::trunc);

    if (!file.is_open())
    {
        listener.onIOError(filePath, FileIOListener::FileUnavailable);
        return;
    }

    listener.onIOStart(filePath);

    size_t chunkSize = m_chunkSize;
    if(dataBuffer.size() < m_chunkSize)
        chunkSize = dataBuffer.size();

    const float percentage = static_cast<float>(chunkSize) / (static_cast<float>(dataBuffer.size()) / 100.0);
    float percentageRead = 0;

    for(size_t i = 0; i < dataBuffer.size(); i += chunkSize)
    {
        if(m_stopWorkFlag)
            break;
        if(m_pausFlag)
        {
            listener.onPause();
            std::unique_lock lock(m_fileManagerMutex);
            m_pauseCondition.wait(lock, [a = &m_stopWorkFlag , b = &m_pausFlag ]()
                {return (a->load() || !b->load());});
            listener.onResume();
            if(m_stopWorkFlag)
                break;
        }
        if(chunkSize > dataBuffer.size() - i)
            chunkSize = dataBuffer.size() - i;
            
        file.write(dataBuffer.data() + i, chunkSize);

        if (file.fail())
        {
            listener.onIOError(filePath, FileIOListener::FileWriteError);
            return;
        }
        percentageRead += percentage;
        listener.onProgress(percentageRead);
    }

    if(m_stopWorkFlag)
    {
        listener.onStop();
        return;
    }
    file.close();
    listener.onSaveComplete(filePath);
}




// --------------------------------------------------------------------------



FileManager::FileManager(const size_t chunkSize)
{
    pimpl = std::make_unique<PImpl>(chunkSize);
}

FileManager::~FileManager()
{

}

void FileManager::loadFile(
    const std::string& filePath,
    std::string& dataBuffer,
    FileIOListener& listner
)
{
    pimpl->loadFile(filePath, dataBuffer, listner);
}

void FileManager::pause(FileIOListener& listener)
{
    pimpl->pause(listener);
}

void FileManager::resume(FileIOListener& listener)
{
    pimpl->resume(listener);
}

void FileManager::stopWork(FileIOListener& listener)
{
    pimpl->stopWork(listener);
}

void FileManager::saveFile( const std::string& filePath,
                            const std::string& dataBuffer,
                            const bool isRewrite,
                            FileIOListener& listener
                        )
{
    pimpl->saveFile(filePath, dataBuffer, isRewrite, listener);
}


}// namespace TextEditorCore
