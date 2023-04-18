#pragma once
#include <memory>
#include <functional>


/* -------------------------------------------------------------------------- */

namespace TextEditorCore
{

class FileManager //Signleton
{
private:
    struct PImpl;
    std::unique_ptr<PImpl> pimpl;
    
public:
    void loadFile(
        const std::string& filePath,
        std::function<void ()> onStartLoadCallback,
        std::function<void ()> onFinishLoadCallback,
        std::function<void ()> onErrorLoadCallback
    );
    
    void stopWork();
    
    void saveFile(
        const std::string& filePath,
        const std::vector<uint8_t> data,
        std::function<void ()> onStartSaveCallback,
        std::function<void ()> onFinishSaveCallback,
        std::function<void ()> onErrorSaveCallback
    );
    
    //load file
    //save file
    //save file
};

}// namespace TextEditorCore
