#include "FileManager.h"


/* -------------------------------------------------------------------------- */
//PIMPL

#include <iostream>
#include <fstream>

namespace TextEditorCore
{

struct FileManager::PImpl
{
    void loadFile(
        // const std::string& filePath,
        // std::function<void ()> onStartLoadCallback,
        // std::function<void ()> onFinishLoadCallback,
        // std::function<void ()> onErrorLoadCallback
    );
    
    void stopWork();
    
    void saveFile(
        // const std::string& filePath,
        // const std::vector<uint8_t> data,
        // std::function<void ()> onStartSaveCallback,
        // std::function<void ()> onFinishSaveCallback,
        // std::function<void ()> onErrorSaveCallback
    );
    
    
};


void FileManager::PImpl::loadFile(
    // const std::string& filePath,
    // std::function<void ()> onStartLoadCallback,
    // std::function<void ()> onFinishLoadCallback,
    // std::function<void ()> onErrorLoadCallback
)
{
    //dymmy implemetation
}

void FileManager::PImpl::stopWork()
{
    
}

void FileManager::PImpl::saveFile(
    // const std::string& filePath,
    // const std::vector<uint8_t> data,
    // std::function<void ()> onStartSaveCallback,
    // std::function<void ()> onFinishSaveCallback,
    // std::function<void ()> onErrorSaveCallback
)
{
    
}




/* -------------------------------------------------------------------------- */

void FileManager::loadFile(
    const std::string& filePath,
    std::function<void ()> onStartLoadCallback,
    std::function<void ()> onFinishLoadCallback,
    std::function<void ()> onErrorLoadCallback
)
{
    pimpl->loadFile();
}

void FileManager::stopWork()
{
    
}

void FileManager::saveFile(
    const std::string& filePath,
    const std::vector<uint8_t> data,
    std::function<void ()> onStartSaveCallback,
    std::function<void ()> onFinishSaveCallback,
    std::function<void ()> onErrorSaveCallback
)
{
    
}

}
