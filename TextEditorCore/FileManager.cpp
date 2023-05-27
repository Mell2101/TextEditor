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
    std::atomic_bool m_pausFlag = false;
    std::atomic_bool m_stopWorkFlag = false;
    std::atomic_bool m_isWorkingFlag = false;
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
        m_isWorkingFlag = true;
        listner.onIOStart(filePath);
        // get length of file:
        file.seekg (0, file.end);
        int length = static_cast<int>(file.tellg());
        file.seekg (0, file.beg);

        float percentage = static_cast<float>(m_chunkSize)  / (static_cast<float>(length) / 100.0);
        if(m_chunkSize > length)
            percentage = 100.0;

        float percentageRead = 0;


        dataBuffer.clear();
        dataBuffer.resize(length);

        size_t chunkSize = m_chunkSize;
        for(size_t i = 0; i < length; i += chunkSize)
        {
            if(m_stopWorkFlag)
                break;
            if(m_pausFlag)
            {
                m_isWorkingFlag = false;
                listner.onPause();
                std::unique_lock lock(m_fileManagerMutex);
                m_pauseCondition.wait(lock, [a = &m_stopWorkFlag , b = &m_pausFlag ]()
                    {return (a->load() || !b->load());});
                if(m_stopWorkFlag)
                    break;
                m_isWorkingFlag = true;
                listner.onResume();
                
            }
            if(chunkSize > length - i)
                chunkSize = length - i;
            file.read(dataBuffer.data() + i, chunkSize);

            if(file.fail())
            {
                m_isWorkingFlag.store(false);
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
        m_isWorkingFlag.store(false);
        listner.onIOError(filePath, FileIOListener::FileUnavailable);
        return;
    }
    
    if(m_stopWorkFlag){
        m_isWorkingFlag.store(false);
        m_stopWorkFlag.store(false);
        listner.onStop();
        return;
    }
    if(m_pausFlag)
    {
        m_pausFlag.store(false);
        listner.onIOError("pause()", FileIOListener::PauseError);
    }

    file.close();
    m_isWorkingFlag = false;
    listner.onLoadComplete(filePath, dataBuffer);
}

void FileManager::PImpl::pause(FileIOListener& listener)
{
    if(m_pausFlag == true || m_stopWorkFlag == true || m_isWorkingFlag == false)
    {
        listener.onIOError("pause", FileIOListener::PauseError);
        return;
    }
    m_pausFlag = true;
}

void FileManager::PImpl::resume(FileIOListener& listener)
{
    if(!m_pausFlag || m_stopWorkFlag || m_isWorkingFlag == true)
    {
        listener.onIOError("resume", FileIOListener::ResumeError);
        return;
    }
    if(m_pausFlag == false && m_isWorkingFlag == false)
    {
        listener.onIOError("resume", FileIOListener::ResumeError);
        return;
    }
    m_pausFlag = false;
    m_pauseCondition.notify_one();
}

void FileManager::PImpl::stopWork(FileIOListener&listener)
{
    if(m_stopWorkFlag == true || m_isWorkingFlag == false)
    {
        listener.onIOError("stopWork", FileIOListener::StopError);
        return;
    }
    m_pausFlag.store(false);
    m_stopWorkFlag.store(true);
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
        m_isWorkingFlag = false;
        listener.onIOError(filePath, FileIOListener::FileUnavailable);
        return;
    }
    m_isWorkingFlag = true;
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
            m_isWorkingFlag = false;
            listener.onPause();
            std::unique_lock lock(m_fileManagerMutex);
            m_pauseCondition.wait(lock, [a = &m_stopWorkFlag , b = &m_pausFlag ]()
                {return (a->load() || !b->load());});
            if(m_stopWorkFlag)
                break;
            listener.onResume();
            m_isWorkingFlag = true;
        }
        if(chunkSize > dataBuffer.size() - i)
            chunkSize = dataBuffer.size() - i;
            
        file.write(dataBuffer.data() + i, chunkSize);

        if (file.fail())
        {
            m_isWorkingFlag = false;
            listener.onIOError(filePath, FileIOListener::FileWriteError);
            return;
        }
        percentageRead += percentage;
        listener.onProgress(percentageRead);
    }

    if(m_stopWorkFlag)
    {
        m_isWorkingFlag = false;
        m_stopWorkFlag = false;
        listener.onStop();
        return;
    }
    if(m_pausFlag)
    {
        m_pausFlag.store(false);
        listener.onIOError("pause()", FileIOListener::PauseError);
    }
    file.close();
    m_isWorkingFlag = false;
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
