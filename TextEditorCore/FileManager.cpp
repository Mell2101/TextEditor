#include "FileManager.h"


/* -------------------------------------------------------------------------- */
//PIMPL
#include <iostream>
#include <fstream>
#include <filesystem>

namespace TextEditorCore
{

struct FileManager::PImpl
{
    void loadFile(const std::string& filePath,
                  std::string& dataBuffer,
                  const StartLoadCallback& onStartLoadCallback,
                  const FinishLoadCallback& onFinishLoadCallback,
                  const ErrorLoadCallback& onErrorLoadCallback);
    
    void stopWork();
    
    void saveFile(const std::string& filePath,
                  const std::string& dataBuffer,
                  const bool isRewrite,
                  const StartSaveCallback& onStartSaveCallback,
                  const FinishSaveCallback& onFinishSaveCallback,
                  const ErrorSaveCallback& onErrorSaveCallback);
                  
    FileManager::FileIOErrorsEnum checkPath(const std::string& filePath, bool isLoad, bool isExist);
    
};


void FileManager::PImpl::loadFile(const std::string& filePath,
                                  std::string& dataBuffer,
                                  const StartLoadCallback& onStartLoadCallback,
                                  const FinishLoadCallback& onFinishLoadCallback,
                                  const ErrorLoadCallback& onErrorLoadCallback)
{
    // Comment cuz checkPath is deleted
    // FileManager::FileIOErrorsEnum errorCode = checkPath(filePath, true, true);
    // switch (errorCode)
    // {
    //     case FileManager::FileIOErrorsEnum::NoError:
    //         onStartLoadCallback(filePath);
    //         //Loading file here
    //         dataBuffer = "dummyData";
    //         onFinishLoadCallback(filePath, dataBuffer);
    //         return;
    //     default:
    //         onErrorLoadCallback(filePath, errorCode);
    //         return;
    // }
}

void FileManager::PImpl::stopWork()
{

}

void FileManager::PImpl::saveFile(const std::string& filePath,
                                  const std::string& dataBuffer,
                                  const bool isRewrite,
                                  const StartSaveCallback& onStartSaveCallback,
                                  const FinishSaveCallback& onFinishSaveCallback,
                                  const ErrorSaveCallback& onErrorSaveCallback)
{
//    FileManager::FileIOErrorsEnum errorCode = checkPath(filePath, true, true);


            onStartSaveCallback(filePath);
            // Saving file here
            std::ofstream file(filePath);
        
            file.close();
            file.open(filePath);
            


            if (!file.is_open())
            {
                if (std::filesystem::exists(filePath))
                {
                    onErrorSaveCallback(filePath, FileIOErrorsEnum::FileUnavailable);
                    return;
                }

            }


            if(std::filesystem::exists(filePath) && isRewrite == false)
            {
                onErrorSaveCallback(filePath, FileIOErrorsEnum::FileReWriteTaboo);
            }

            file << dataBuffer;
            
            if (file.fail())
            {
                onErrorSaveCallback(filePath, FileIOErrorsEnum::FileWriteError);
                    return;
            }
            
            
            onFinishSaveCallback(filePath);
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
    const StartLoadCallback& onStartLoadCallback,
    const FinishLoadCallback& onFinishLoadCallback,
    const ErrorLoadCallback& onErrorLoadCallback
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
                           const bool isRewrite,
                           const StartSaveCallback& onStartSaveCallback,
                           const FinishSaveCallback& onFinishSaveCallback,
                           const ErrorSaveCallback& onErrorSaveCallback
)
{
    pimpl->saveFile(filePath, dataBuffer, isRewrite, onStartSaveCallback, onFinishSaveCallback, onErrorSaveCallback);
}

}// namespace TextEditorCore
