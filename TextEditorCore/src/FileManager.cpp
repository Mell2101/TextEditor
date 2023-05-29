#include <TextEditorCore/FileManager.h>

#include <iostream>
#include <fstream>
#include <filesystem>
#include <unordered_map>

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

#include <chrono>

/* -------------------------------------------------------------------------- */

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
    
    std::string m_filePath;
    std::string* m_dataBuffer;
    
    std::mutex m_listenerMutex;
    IFileIOListener* m_listener;
    
    std::thread m_thread;
    
#ifdef TESTING
        // delay for testing
        std::chrono::milliseconds m_testDelayTime;
#endif // TESTING
    
public:
    PImpl(const size_t chunkSize) : m_chunkSize(chunkSize), m_dataBuffer(nullptr), m_listener(nullptr) {}
    ~PImpl()
    {
        if (m_thread.joinable())
        {
            setListener(nullptr);
            stopWork();
            m_thread.join();
        }
    }
    
public:
    bool setFilePath(const std::string& filePath)
    {
        if (m_isWorkingAtomic.load())
        {
            std::scoped_lock sLock(m_listenerMutex);
            if (m_listener)
            {
                m_listener->onIOError(m_filePath, IOInProgress);
            }
            return false;
        }
        
        m_filePath = filePath;
        return true;
    }
    
    bool setDataBuffer(std::string& dataBuffer)
    {
        if (m_isWorkingAtomic.load())
        {
            {
                std::scoped_lock sLock(m_listenerMutex);
                if (m_listener)
                {
                    m_listener->onIOError(m_filePath, IOInProgress);
                }
            }
            return false;
        }
        
        m_dataBuffer = &dataBuffer;
        return true;
    }
    
    void setListener(IFileIOListener* listener)
    {
        std::scoped_lock sLock(m_listenerMutex);
        m_listener = listener;
    }
    
    void loadFile()
    {
        if (m_filePath.empty())
        {
            std::scoped_lock sLock(m_listenerMutex);
            if (m_listener)
            {
                m_listener->onIOError(m_filePath, FilePathIsEmpty);
            }
            return;
        }
        
        if (m_dataBuffer == nullptr)
        {
            std::scoped_lock sLock(m_listenerMutex);
            if (m_listener)
            {
                m_listener->onIOError(m_filePath, DataBufferIsNotSet);
            }
            return;
        }
        
        if (!std::filesystem::exists(m_filePath))
        {
            std::scoped_lock sLock(m_listenerMutex);
            if (m_listener)
            {
                m_listener->onIOError(m_filePath, FileDNExist);
            }
            return;
        }
        
        if (m_isWorkingAtomic.load())
        {
            std::scoped_lock sLock(m_listenerMutex);
            if (m_listener)
            {
                m_listener->onIOError(m_filePath, IOInProgress);
            }
            return;
        }
        
        if (m_thread.joinable())
        {
            m_thread.join();
        }
        
        m_thread = std::thread(&FileManager::PImpl::loadFileThreadFunction, this);
    }
    
    void saveFile(const bool isRewrite)
    {
        if (m_filePath.empty())
        {
            std::scoped_lock sLock(m_listenerMutex);
            if (m_listener)
            {
                m_listener->onIOError(m_filePath, FilePathIsEmpty);
            }
            return;
        }
        
        if (m_dataBuffer == nullptr)
        {
            std::scoped_lock sLock(m_listenerMutex);
            if (m_listener)
            {
                m_listener->onIOError(m_filePath, DataBufferIsNotSet);
            }
            return;
        }
        
        if (m_isWorkingAtomic.load())
        {
            std::scoped_lock sLock(m_listenerMutex);
            if (m_listener)
            {
                m_listener->onIOError(m_filePath, IOInProgress);
            }
            return;
        }
        
        if (m_thread.joinable())
        {
            m_thread.join();
        }
        
        if (isRewrite == false && std::filesystem::exists(m_filePath))
        {
            std::scoped_lock sLock(m_listenerMutex);
            if (m_listener)
            {
                m_listener->onIOError(m_filePath, FileReWriteTaboo);
            }
            return;
        }
        
        std::thread SaveThread(&FileManager::PImpl::saveFileThreadFunction, this);
    }
    
    void pause()
    {
        if (m_isPauseAtomic.load() || m_isStopAtomic.load() || !m_isWorkingAtomic.load())
        {
            std::scoped_lock sLock(m_listenerMutex);
            if (m_listener)
            {
                m_listener->onIOError(m_filePath, PauseError);
            }
            return;
        }
        m_isPauseAtomic.store(true);
    }
    
    void resume()
    {
        if (!m_isPauseAtomic.load() || m_isStopAtomic.load())
        {
            std::scoped_lock sLock(m_listenerMutex);
            if (m_listener)
            {
                m_listener->onIOError(m_filePath, ResumeError);
            }
            return;
        }
        m_isPauseAtomic.store(false);
        m_pauseCondition.notify_one();
    }
    
    void stopWork()
    {
        if (m_isStopAtomic.load() || !m_isWorkingAtomic.load())
        {
            std::scoped_lock sLock(m_listenerMutex);
            if (m_listener)
            {
                m_listener->onIOError(m_filePath, StopError);
            }
            return;
        }
        m_isPauseAtomic.store(false);
        m_isStopAtomic.store(true);
        m_pauseCondition.notify_one();
    }
    
private:
    void loadFileThreadFunction()
    {
        std::ifstream file(m_filePath);
        if (!file.is_open())
        {
            std::scoped_lock sLock(m_listenerMutex);
            if (m_listener)
            {
                m_listener->onIOError(m_filePath, FileUnavailable);
            }
            return;
        }
        
        m_isWorkingAtomic.store(true);
        
        {
            std::scoped_lock sLock(m_listenerMutex);
            if (m_listener)
            {
                m_listener->onIOStart(m_filePath);
            }
        }
        // get length of file:
        
        file.seekg (0, file.end);
        int64_t fileSize = static_cast<int>(file.tellg());
        file.seekg (0, file.beg);
        
        m_dataBuffer->clear();
        m_dataBuffer->resize(fileSize);
        
        int64_t pos = 0;
        while (!file.eof() && pos < fileSize)
        {
            if (m_isStopAtomic.load())
            {
                break;
            }
            if (m_isPauseAtomic.load())
            {
                if (m_listener)
                {
                    m_listener->onPause(m_filePath);
                }
                std::unique_lock lock(m_pauseMutex);
                m_pauseCondition.wait(lock, 
                    [&]()
                    {
                        return (!m_isPauseAtomic.load() || m_isStopAtomic.load());
                    }
                );
                
                {
                    std::scoped_lock sLock(m_listenerMutex);
                    if (m_listener)
                    {
                        m_listener->onResume(m_filePath);
                    }
                }
                continue;
            }
            file.read(m_dataBuffer->data() + file.tellg(), m_chunkSize);
            if (file.fail() && !file.eof())
            {
                m_isWorkingAtomic.store(false);
                file.close();
                
                {
                    std::scoped_lock sLock(m_listenerMutex);
                    if (m_listener)
                    {
                        m_listener->onIOError(m_filePath, FileReadError);
                    }
                }
                return;
            }
            pos = file.tellg();
            
            {
                std::scoped_lock sLock(m_listenerMutex);
                if (m_listener)
                {
                    m_listener->onProgress(static_cast<float>(pos) / fileSize);
                }
            }
    #ifdef TESTING
            std::this_thread::sleep_for(std::chrono::milliseconds(m_testDelayTime)); 
    #endif // TESTING
        }
        
        file.close();
        
        if (m_isStopAtomic.load())
        {
            m_isStopAtomic.store(false);
            m_isWorkingAtomic.store(false);
            
            {
                std::scoped_lock sLock(m_listenerMutex);
                if (m_listener)
                {
                    m_listener->onStop(m_filePath);
                }
            }
            return;
        }
        if (m_isPauseAtomic.load())
        {
            m_isPauseAtomic.store(false);
            
            std::scoped_lock sLock(m_listenerMutex);
            {
                if (m_listener)
                {
                    m_listener->onIOError(m_filePath, PauseError);
                }
            }
        }
        
        std::scoped_lock sLock(m_listenerMutex);
        {
            if (m_listener)
            {
                m_listener->onLoadComplete(m_filePath, *m_dataBuffer);
            }
        }
        
        m_isWorkingAtomic.store(false);
    }
    
    void saveFileThreadFunction()
    {
        std::ofstream file(m_filePath, std::ios::out | std::ios::trunc);
        
        if (!file.is_open())
        {
            std::scoped_lock sLock(m_listenerMutex);
            if (m_listener)
            {
                m_listener->onIOError(m_filePath, FileUnavailable);
            }
            return;
        }
        
        m_isWorkingAtomic.store(true);
        
        {
            std::scoped_lock sLock(m_listenerMutex);
            if (m_listener)
            {
                m_listener->onIOStart(m_filePath);
            }
        }
        
        size_t chunkSize = m_chunkSize;
        size_t pos = 0;
        while (pos < m_dataBuffer->size())
        {
            {
                std::scoped_lock sLock(m_listenerMutex);
                if (m_isStopAtomic.load())
                {
                    break;
                }
            }
            
            if (m_isPauseAtomic.load())
            {
                {
                    std::scoped_lock sLock(m_listenerMutex);
                    if (m_listener)
                    {
                        m_listener->onPause(m_filePath);
                    }
                }
                
                std::unique_lock lock(m_pauseMutex);
                m_pauseCondition.wait(lock,
                    [&]()
                    {
                        return (m_isStopAtomic.load() || !m_isPauseAtomic.load());
                    }
                );
                
                {
                    std::scoped_lock sLock(m_listenerMutex);
                    if (m_listener)
                    {
                        m_listener->onResume(m_filePath);
                    }
                }
                continue;
            }
            
            if (chunkSize > m_dataBuffer->size() - pos)
            {
                chunkSize = m_dataBuffer->size() - pos;
            }
            
            file.write(m_dataBuffer->data() + pos, chunkSize);
            
            if (file.fail())
            {
                m_isWorkingAtomic = false;
                file.close();
                
                {
                    std::scoped_lock sLock(m_listenerMutex);
                    if (m_listener)
                    {
                        m_listener->onIOError(m_filePath, FileWriteError);
                    }
                }
                
                return;
            }
            
            pos += chunkSize;
            
            {
                std::scoped_lock sLock(m_listenerMutex);
                if (m_listener)
                {
                    m_listener->onProgress(static_cast<float>(pos) / m_dataBuffer->size());
                }
            }
        }
        
        file.close();
        m_isWorkingAtomic.store(false);
        
        if (m_isStopAtomic.load())
        {
            m_isStopAtomic.store(false);
            
            {
                std::scoped_lock sLock(m_listenerMutex);
                if (m_listener)
                {
                    m_listener->onStop(m_filePath);
                }
            }
            return;
        }
        if (m_isPauseAtomic.load())
        {
            m_isPauseAtomic.store(false);
            
            {
                std::scoped_lock sLock(m_listenerMutex);
                if (m_listener)
                {
                    m_listener->onIOError(m_filePath, PauseError);
                }
            }
        }
        
        if (m_listener)
        {
            m_listener->onSaveComplete(m_filePath);
        }
    }
};

// --------------------------------------------------------------------------

FileManager::FileManager(const size_t chunkSize)
{
    pimpl = std::make_unique<PImpl>(chunkSize);
}

FileManager::~FileManager()
{
    pimpl->setListener(nullptr);
    pimpl->stopWork();
}

void FileManager::setFilePath(const std::string& filePath)
{
    pimpl->setFilePath(filePath);
}

void FileManager::setDataBuffer(std::string& dataBuffer)
{
    pimpl->setDataBuffer(dataBuffer);
}

void FileManager::setListener(IFileIOListener* listener)
{
    pimpl->setListener(listener);
}

void FileManager::loadFile()
{
    pimpl->loadFile();
}

void FileManager::saveFile(const bool isRewrite)
{
    pimpl->saveFile(isRewrite);
}

void FileManager::pause()
{
    pimpl->pause();
}

void FileManager::resume()
{
    pimpl->resume();
}

void FileManager::stopWork()
{
    pimpl->stopWork();
}


}// namespace TextEditorCore
