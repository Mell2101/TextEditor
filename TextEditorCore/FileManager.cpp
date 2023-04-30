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
    onStartLoadCallback(filePath);

    if (std::filesystem::exists(filePath))
    {
        std::ifstream file(filePath);
        file.open(filePath);

        if (file.is_open())
        {
            getline(file, dataBuffer);

            if (file.fail())
            {
                onErrorLoadCallback(filePath, FileIOErrorsEnum::FileReadError);
                return;
            }
            
        }else
            onErrorLoadCallback(filePath, FileIOErrorsEnum::FileUnavailable);
    }else
        onErrorLoadCallback(filePath, FileIOErrorsEnum::FileDNExist);
        return;
    

    

    onFinishLoadCallback(filePath, dataBuffer);

    
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
                return;
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
