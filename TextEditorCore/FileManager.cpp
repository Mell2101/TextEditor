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
    std::atomic<bool> m_isPauseAtomic = false;
    std::atomic<bool> m_isStopAtomic = false;
    std::atomic<bool> m_isWorkingAtomic = false;
    std::mutex m_pauseMutex;
    std::condition_variable m_pauseCondition;
    const size_t m_chunkSize;

    // TODO: ADD DEFINES
    std::chrono::milliseconds m_testDelayTime;
public:
    PImpl(const size_t chunkSize): m_chunkSize(chunkSize){}

private:
    
    void loadFileFunction(const std::string& filePath,
                        std::string& dataBuffer,
                        IFileIOListener& listener
                        );

    void saveFileFunction(const std::string& filePath,
                        const std::string& dataBuffer,
                        IFileIOListener& listener
                        );

public:
    void loadFile(const std::string& filePath,
                std::string& dataBuffer,
                IFileIOListener& listner
                );

    void resume(const std::string& fileName, IFileIOListener&);
    
    void pause(const std::string&, IFileIOListener&);

    void stopWork(const std::string& fileName, IFileIOListener&);
    
    void saveFile(const std::string& filePath,
                const std::string& dataBuffer,
                const bool isRewrite,
                IFileIOListener& listener
                );
                  
    IFileIOListener::FileIOErrorsEnum checkPath(const std::string& filePath, bool isLoad, bool isExist);
    
};


void FileManager::PImpl::loadFile(const std::string& filePath,
                                std::string& dataBuffer,
                                IFileIOListener& listner
                                )
{
    if(!std::filesystem::exists(filePath))
    {
        listner.onIOError(filePath, IFileIOListener::FileDNExist);
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
                                        IFileIOListener& listner
                                        )
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        listner.onIOError(filePath, IFileIOListener::FileUnavailable);
        return;
    }

    m_isWorkingAtomic.store(true);
    listner.onIOStart(filePath);
    // get length of file:

    file.seekg (0, file.end);
    int64_t fileSize = static_cast<int>(file.tellg());
    file.seekg (0, file.beg);

    dataBuffer.clear();
    dataBuffer.resize(fileSize);

    int64_t pos = 0;
    while(!file.eof() && pos < fileSize)
    {
        if(m_isStopAtomic.load())
        {
            break;
        }
        if(m_isPauseAtomic.load())
        {
            listner.onPause(filePath);
            std::unique_lock lock(m_pauseMutex);
            m_pauseCondition.wait(lock, 
                [&]()
                {
                    return (!m_isPauseAtomic.load()|| m_isStopAtomic.load());
                }
            );
            listner.onResume(filePath);
            continue;     
        }
        file.read(dataBuffer.data() + file.tellg(), m_chunkSize);
        if(file.fail() && !file.eof())
        {
            m_isWorkingAtomic.store(false);
            file.close();
            listner.onIOError(filePath, IFileIOListener::FileReadError);
            return;
        }
        pos = file.tellg();
        listner.onProgress(static_cast<float>(pos) / fileSize);
#ifdef  TESTING
        std::this_thread::sleep_for(std::chrono::milliseconds(m_testDelayTime)); 
#endif //  TESTING
    }


    file.close();
    m_isWorkingAtomic.store(false);

    if(m_isStopAtomic){
        m_isStopAtomic.store(false);
        listner.onStop(filePath);
        return;
    }
    if(m_isPauseAtomic)
    {
        m_isPauseAtomic.store(false);
        listner.onIOError(filePath, IFileIOListener::PauseError);
    }
    listner.onLoadComplete(filePath, dataBuffer);
}

void FileManager::PImpl::pause(const std::string& fileName, IFileIOListener& listener)
{
    if(m_isPauseAtomic.load() || m_isStopAtomic.load() || !m_isWorkingAtomic.load())
    {
        listener.onIOError(fileName, IFileIOListener::PauseError);
        return;
    }
    m_isPauseAtomic.store(true);
}

void FileManager::PImpl::resume(const std::string& fileName, IFileIOListener& listener)
{
    if(!m_isPauseAtomic.load() || m_isStopAtomic.load())
    {
        listener.onIOError(fileName, IFileIOListener::ResumeError);
        return;
    }
    m_isPauseAtomic.store(false);
    m_pauseCondition.notify_one();
}

void FileManager::PImpl::stopWork(const std::string& fileName,IFileIOListener&listener)
{
    if(m_isStopAtomic.load() || !m_isWorkingAtomic.load())
    {
        listener.onIOError(fileName, IFileIOListener::StopError);
        return;
    }
    m_isPauseAtomic.store(false);
    m_isStopAtomic.store(true);
    m_pauseCondition.notify_one();
}

void FileManager::PImpl::saveFile(const std::string& filePath,
                                const std::string& dataBuffer,
                                const bool isOverWriteAllowed,
                                IFileIOListener& listener
                                )
{    
    if(isOverWriteAllowed == false && std::filesystem::exists(filePath))
    {
        listener.onIOError(filePath, IFileIOListener::FileReWriteTaboo);
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
                                  IFileIOListener& listener
                                  )
{
    std::ofstream file(filePath, std::ios::out | std::ios::trunc);

    if (!file.is_open())
    {
        listener.onIOError(filePath, IFileIOListener::FileUnavailable);
        return;
    }
    m_isWorkingAtomic.store(true);
    listener.onIOStart(filePath);

    size_t chunkSize = m_chunkSize;
    size_t pos = 0;
    while(pos < dataBuffer.size())
    {
        if(m_isStopAtomic.load())
        {
            break;
        }
        if(m_isPauseAtomic.load())
        {
            listener.onPause(filePath);
            std::unique_lock lock(m_pauseMutex);
            m_pauseCondition.wait(lock,
                [&]()
                {
                    return (m_isStopAtomic.load() || !m_isPauseAtomic.load());
                }
            );
            listener.onResume(filePath);
            continue;
        }
        if(chunkSize > dataBuffer.size() - pos)
            chunkSize = dataBuffer.size() - pos;
        file.write(dataBuffer.data() + pos, chunkSize);

        if (file.fail())
        {
            m_isWorkingAtomic = false;
            file.close();
            listener.onIOError(filePath, IFileIOListener::FileWriteError);
            return;
        }

        pos += chunkSize;
        listener.onProgress(static_cast<float>(pos) / dataBuffer.size());
    }

    file.close();
    m_isWorkingAtomic.store(false);
    
    if(m_isStopAtomic.load())
    {
        m_isStopAtomic.store(false);
        listener.onStop(filePath);
        return;
    }
    if(m_isPauseAtomic.load())
    {
        m_isPauseAtomic.store(false);
        listener.onIOError(filePath, IFileIOListener::PauseError);
    }
    
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
    IFileIOListener& listner
)
{
    pimpl->loadFile(filePath, dataBuffer, listner);
}

void FileManager::pause(const std::string& fileName,IFileIOListener& listener)
{
    pimpl->pause(fileName, listener);
}

void FileManager::resume(const std::string& fileName, IFileIOListener& listener)
{
    pimpl->resume(fileName, listener);
}

void FileManager::stopWork(const std::string& fileName, IFileIOListener& listener)
{
    pimpl->stopWork(fileName, listener);
}

void FileManager::saveFile( const std::string& filePath,
                            const std::string& dataBuffer,
                            const bool isRewrite,
                            IFileIOListener& listener
                        )
{
    pimpl->saveFile(filePath, dataBuffer, isRewrite, listener);
}


}// namespace TextEditorCore


    // for(size_t i = 0; i < dataBuffer.size(); i += chunkSize)
    // {
    //     if(m_isStopAtomic.load())
    //     {
    //         break;
    //     }
    //     if(m_isPauseAtomic.load())
    //     {
    //         listener.onPause(filePath);
    //         std::unique_lock lock(m_pauseMutex);
    //         m_pauseCondition.wait(lock,
    //             [&]()
    //             {
    //                 return (m_isStopAtomic.load() || !m_isPauseAtomic.load());
    //             }
    //         );
    //         if(m_isStopAtomic.load())
    //             break;
    //         listener.onResume(filePath);
    //     }
    //     if(chunkSize > dataBuffer.size() - i)
    //         chunkSize = dataBuffer.size() - i;
            
    //     file.write(dataBuffer.data() + i, chunkSize);

    //     if (file.fail())
    //     {
    //         m_isWorkingAtomic = false;
    //         file.close();
    //         listener.onIOError(filePath, FileIOListener::FileWriteError);
    //         return;
    //     }
        
    //     listener.onProgress(static_cast<float>(file.tellp()) / dataBuffer.size());
    // }
