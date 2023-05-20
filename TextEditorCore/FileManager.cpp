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
    // using pauseFlag = bool;
private:
    std::atomic<bool> m_pausFlag;
    std::atomic<bool> m_stopWorkFlag;
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
   
    if (std::filesystem::exists(filePath))
    {
        std::ifstream file(filePath);

        if (file.is_open())
        {
            listner.onIOStart(filePath);
            // get length of file:
            file.seekg (0, file.end);
            int length = static_cast<int>(file.tellg());
            file.seekg (0, file.beg);
            const float percentage = static_cast<float>(m_chunkSize) * static_cast<float>(length) / 100.0;

            dataBuffer.clear();
            dataBuffer.resize(length);

            float percentageRead = 0;
            while(!file.eof())
            {
                std::unique_lock lock(m_fileManagerMutex);
                m_pauseCondition.wait(lock, [a = &m_stopWorkFlag , b = &m_pausFlag ](){return (a || b);});
                lock.unlock();
                if(m_stopWorkFlag)
                    break;

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
    }
    else
    {
        listner.onIOError(filePath, FileIOListener::FileDNExist);
        return;
    }
    if(m_stopWorkFlag){
        listner.onStop();
        return;
        // нам тут навыерное уже ничего не нужно 
    }
    listner.onLoadComplete(filePath, dataBuffer);
}

void FileManager::PImpl::pause(const std::string& filePath, FileIOListener& listener)
{
    std::lock_guard lock(m_fileManagerMutex);
    if(m_pausFlag == true || m_stopWorkFlag)
    {
        listener.onIOError("", FileIOListener::FileWriteError);
        return;
    }
    m_pausFlag = true;
    listener.onPause();
}

void FileManager::PImpl::resume(const std::string& filePath, FileIOListener& listener)
{
    m_pausFlag = false;
    m_pauseCondition.notify_one();
    listener.onResume();
}

void FileManager::PImpl::stopWork(const std::string& filePath, FileIOListener&listener)
{
    m_pausFlag = true;
    m_pauseCondition.notify_one();
}

void FileManager::PImpl::saveFile(const std::string& filePath,
                                const std::string& dataBuffer,
                                const bool isRewrite,
                                FileIOListener& listener
                                )
{    
    std::thread SaveThread(&FileManager::PImpl::saveFileFunction,
                            this,
                            std::ref(filePath),
                            std::ref(dataBuffer),
                            std::ref(isRewrite),
                            std::ref(listener)
                        );
    SaveThread.detach();
}

void FileManager::PImpl::saveFileFunction(const std::string& filePath,
                                  const std::string& dataBuffer,
                                  const bool isRewrite,
                                  FileIOListener& listener
                                  )
{
            // Saving file here
            std::ofstream file(filePath);
        
            file.close();
            file.open(filePath);

            if (!file.is_open())
            {
                if (std::filesystem::exists(filePath))
                {
                    listener.onIOError(filePath, FileIOListener::FileUnavailable);
                    return;
                }

            }


            if(std::filesystem::exists(filePath) && isRewrite == false)
            {
                listener.onIOError(filePath, FileIOListener::FileReWriteTaboo);
                return;
            }

            const float percentage = static_cast<float>(m_chunkSize) * static_cast<float>(dataBuffer.size()) / 100.0;
            float percentageRead = 0;
            listener.onIOStart(filePath);

            for(size_t i = 0; i < dataBuffer.size(); i += m_chunkSize)
            {
                std::unique_lock lock(m_fileManagerMutex);
                m_pauseCondition.wait(lock, [a = &m_stopWorkFlag , b = &m_pausFlag ](){return (a || b);});
                lock.unlock();
                if(m_stopWorkFlag)
                    break;
                file.write(dataBuffer.data() + i, m_chunkSize);
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
