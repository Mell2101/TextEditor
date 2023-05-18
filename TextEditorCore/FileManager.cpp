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

// class Worker{

//     std::condition_variable cv;
//     bool pauseFlag;
//     std::atomic<bool> stopFalag;
//     std::thread functionThread;
    
//     void pause();
//     void resume();
//     void stopWork();
//     void startWork();

// };

struct ThreadFlags
{
private:
    bool m_pauseFlag = false;
    bool m_stopWorkFlag = false;

public:
    bool getPauseFlag(){return m_pauseFlag;};
    bool getStopWorkFlag(){return m_stopWorkFlag;}

    void setPauseFlag(const bool a){m_pauseFlag = a;}
    void setStopWorkFlag(const bool a){m_stopWorkFlag = a;}
};


struct FileManager::PImpl
{
    // using pauseFlag = bool;
private:
    std::unordered_map<std::string, ThreadFlags> m_filePathPauseFlagMap;
    std::mutex m_filePathPauseFlagMapMutex;
    std::condition_variable m_pauseCondition;
    std::atomic<bool> m_isStopWork;
    // впорос что елает stopWork если отсанавливает все потоки то надо добавить флаг
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
                        const bool isRewrite,
                        FileIOListener& listener
                        );

public:
    void loadFile(const std::string& filePath,
                std::string& dataBuffer,
                FileIOListener& listner
                );

    void resume(const std::string& filePath, FileIOListener&);
    
    void pause(const std::string& filePath, FileIOListener&);

    void stopWork(const std::string& filePath, FileIOListener&);
    
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
    std::lock_guard lock(m_filePathPauseFlagMapMutex);
    if(m_filePathPauseFlagMap.emplace(filePath, ThreadFlags()).second == false)
    {
        // TODO: определиться с ошибкой для колбэка
        listner.onIOError("", FileIOListener::FileUnavailable);
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
    listner.onIOStart(filePath);
    
    if (std::filesystem::exists(filePath))
    {
        std::ifstream file(filePath);
        file.open(filePath);

        if (file.is_open())
        {
            // get length of file:
            file.seekg (0, file.end);
            int length = static_cast<int>(file.tellg());
            file.seekg (0, file.beg);
            const float percentage = static_cast<float>(m_chunkSize) * static_cast<float>(length) / 100.0;

            dataBuffer.clear();
            dataBuffer.reserve(length);

            float percentageRead = 0;
            while(!file.eof() && !m_isStopWork)
            {
                std::unique_lock lock(m_filePathPauseFlagMapMutex);
                
                m_pauseCondition.wait(lock,
                    [pauseFlag = m_filePathPauseFlagMap.at(filePath).getPauseFlag(),
                    stopFlag = m_filePathPauseFlagMap.at(filePath).getStopWorkFlag()]()
                    {return pauseFlag || stopFlag;}
                );
                lock.unlock();
                if(m_filePathPauseFlagMap.at(filePath).getStopWorkFlag())
                {
                    std::lock_guard lock(m_filePathPauseFlagMapMutex);
                    m_filePathPauseFlagMap.erase(filePath);
                    listner.onStop();
                    return;
                }

                file.read(dataBuffer.data() + file.tellg(), m_chunkSize);
                if(file.fail())
                {
                    listner.onIOError(filePath, FileIOListener::FileReadError);
                    std::lock_guard lock(m_filePathPauseFlagMapMutex);
                    m_filePathPauseFlagMap.erase(filePath);
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
        }
    }
    else
    {
        listner.onIOError(filePath, FileIOListener::FileDNExist);
        std::lock_guard lock(m_filePathPauseFlagMapMutex);
        m_filePathPauseFlagMap.erase(filePath);
        return;
    }
    if(m_filePathPauseFlagMap.at(filePath).getStopWorkFlag()){
        std::lock_guard lock(m_filePathPauseFlagMapMutex);
        m_filePathPauseFlagMap.erase(filePath);
        listner.onStop();
        return;
        // нам тут навыерное уже ничего не нужно 
    }
    std::lock_guard lock(m_filePathPauseFlagMapMutex);
    m_filePathPauseFlagMap.erase(filePath);
    listner.onLoadComplete(filePath, dataBuffer);
}

void FileManager::PImpl::pause(const std::string& filePath, FileIOListener& listener)
{
    std::lock_guard lock(m_filePathPauseFlagMapMutex);
    auto elmentToPouse = m_filePathPauseFlagMap.find(filePath);
    if(elmentToPouse == m_filePathPauseFlagMap.end())
    {
        //TODO: какая тут должна быть ошибка
        listener.onIOError("", FileIOListener::FileDNExist);
        return;
    }
    if((*elmentToPouse).second.getStopWorkFlag() == true || (*elmentToPouse).second.getPauseFlag() == true)
    {
        listener.onIOError("", FileIOListener::FileDNExist);
        return;
    }
    (*elmentToPouse).second.setPauseFlag(true);
}

void FileManager::PImpl::resume(const std::string& filePath, FileIOListener& listener)
{
    std::lock_guard lock(m_filePathPauseFlagMapMutex);
    auto elementToResume = m_filePathPauseFlagMap.find(filePath);

    if(elementToResume == m_filePathPauseFlagMap.end())
    {   
        listener.onIOError("", FileIOListener::FileDNExist);
        return;
    }
    if((*elementToResume).second.getStopWorkFlag() == true || (*elementToResume).second.getPauseFlag() == true)
    {
        listener.onIOError("", FileIOListener::FileDNExist);
        return;
    }
    (*elementToResume).second.setPauseFlag(false);
    m_pauseCondition.notify_one();
}

void FileManager::PImpl::stopWork(const std::string& filePath, FileIOListener&listener)
{
    std::lock_guard lock(m_filePathPauseFlagMapMutex);
    auto elmentToStop = m_filePathPauseFlagMap.find(filePath);
    if(elmentToStop == m_filePathPauseFlagMap.end())
    {
        //TODO: какая тут должна быть ошибка
        listener.onIOError("", FileIOListener::FileDNExist);
        return;
    }
    if((*elmentToStop).second.getStopWorkFlag() == true)
    {
        //TODO: какая тут должна быть ошибка
        listener.onIOError("", FileIOListener::FileDNExist);
        return;
    }
    (*elmentToStop).second.setStopWorkFlag(true);
}

void FileManager::PImpl::saveFile(const std::string& filePath,
                                const std::string& dataBuffer,
                                const bool isRewrite,
                                FileIOListener& listener
                                )
{
    // std::lock_guard lock(m_filePathPauseFlagMapMutex);
    // if(m_filePathPauseFlagMap.emplace(filePath, false).second == false)
    // {
    //     // TODO: определиться с ошибкой для колбэка
    //     listener.onIOError("", FileIOListener::FileUnavailable);
    //     return;
    // }
    
    // std::thread SaveThread(&FileManager::PImpl::saveFileFunction,
    //                         this,
    //                         std::ref(filePath),
    //                         std::ref(dataBuffer),
    //                         std::ref(isRewrite),
    //                         std::ref(listener)
    //                     );
    // SaveThread.detach();
}

void FileManager::PImpl::saveFileFunction(const std::string& filePath,
                                  const std::string& dataBuffer,
                                  const bool isRewrite,
                                  FileIOListener& listener
                                  )
{
            // listener.onIOStart(filePath);
            // // Saving file here
            // std::ofstream file(filePath);
        
            // file.close();
            // file.open(filePath);

            // if (!file.is_open())
            // {
            //     if (std::filesystem::exists(filePath))
            //     {
            //         listener.onIOError(filePath, FileIOListener::FileUnavailable);
            //         return;
            //     }

            // }


            // if(std::filesystem::exists(filePath) && isRewrite == false)
            // {
            //     listener.onIOError(filePath, FileIOListener::FileReWriteTaboo);
            //     return;
            // }

            // file << dataBuffer;
            
            // if (file.fail())
            // {
            //     listener.onIOError(filePath, FileIOListener::FileWriteError);
            //         return;
            // }
            
            // listener.onSaveComplete(filePath);
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

void FileManager::pause(const std::string& filePath, FileIOListener& listener)
{
    pimpl->pause(filePath, listener);
}

void FileManager::resume(const std::string& filePath, FileIOListener& listener)
{
    pimpl->resume(filePath, listener);
}

void FileManager::stopWork(const std::string& filePath, FileIOListener& listener)
{
    pimpl->stopWork(filePath, listener);
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
