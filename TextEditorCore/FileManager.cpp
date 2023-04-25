#include "FileManager.h"


/* -------------------------------------------------------------------------- */
//PIMPL

#include <iostream>
#include <fstream>

namespace TextEditorCore
{

struct FileManager::PImpl
{
    void loadFile(const std::string& filePath,
                  std::string& dataBuffer,
                  StartLoadCallback onStartLoadCallback,
                  FinishLoadCallback onFinishLoadCallback,
                  ErrorLoadCallback onErrorLoadCallback);
    
    void stopWork();
    
    void saveFile(const std::string& filePath,
                  const std::string& dataBuffer,
                  StartSaveCallback onStartSaveCallback,
                  FinishSaveCallback onFinishSaveCallback,
                  ErrorSaveCallback onErrorSaveCallback);
                  
    FileManager::FileIOErrorsEnum checkPath(const std::string& filePath, bool isLoad);
    
};


void FileManager::PImpl::loadFile(const std::string& filePath,
                                  std::string& dataBuffer,
                                  StartLoadCallback onStartLoadCallback,
                                  FinishLoadCallback onFinishLoadCallback,
                                  ErrorLoadCallback onErrorLoadCallback)
{
    FileManager::FileIOErrorsEnum errorCode = checkPath(filePath, true);
    switch (errorCode)
    {
        case FileManager::FileIOErrorsEnum::NoError:
            onStartLoadCallback(filePath);
            //Loading file here
            dataBuffer = "dummyData";
            onFinishLoadCallback(filePath, dataBuffer);
            return;
        default:
            onErrorLoadCallback(filePath, errorCode);
            return;
    }
}

void FileManager::PImpl::stopWork()
{

}

void FileManager::PImpl::saveFile(const std::string& filePath,
                                  const std::string& dataBuffer,
                                  StartSaveCallback onStartSaveCallback,
                                  FinishSaveCallback onFinishSaveCallback,
                                  ErrorSaveCallback onErrorSaveCallback)
{
    FileManager::FileIOErrorsEnum errorCode = checkPath(filePath, true);
    switch (errorCode)
    {
        case FileManager::FileIOErrorsEnum::NoError:
            onStartSaveCallback(filePath);
            // Saving file here
            onFinishSaveCallback(filePath);
            return;
        default:
            onErrorSaveCallback(filePath, errorCode);
            return;
    }
}

FileManager::FileIOErrorsEnum FileManager::PImpl::checkPath(const std::string& filePath, bool isLoad)
{
    return FileManager::FileIOErrorsEnum::NoError;
}


// --------------------------------------------------------------------------

FileManager::FileManager()
{
    pimpl = std::make_unique<PImpl>();
}

FileManager::~FileManager()
{

}

void FileManager::loadFile(
    const std::string& filePath,
    std::string& dataBuffer,
    StartLoadCallback onStartLoadCallback,
    FinishLoadCallback onFinishLoadCallback,
    ErrorLoadCallback onErrorLoadCallback
)
{
    pimpl->loadFile(filePath, dataBuffer, onStartLoadCallback, onFinishLoadCallback, onErrorLoadCallback);
}

void FileManager::stopWork()
{
    pimpl->stopWork();
}

void FileManager::saveFile(const std::string& filePath,
                           const std::string& dataBuffer,
                           StartSaveCallback onStartSaveCallback,
                           FinishSaveCallback onFinishSaveCallback,
                           ErrorSaveCallback onErrorSaveCallback
)
{
    pimpl->saveFile(filePath, dataBuffer, onStartSaveCallback, onFinishSaveCallback, onErrorSaveCallback);
}

}// namespace TextEditorCore
