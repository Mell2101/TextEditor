#pragma once
#include <memory>
#include <functional>


/* -------------------------------------------------------------------------- */

namespace TextEditorCore
{

class FileManager //Signleton
{
public:
    enum FileIOErrorsEnum
    {
        NoError = 0,
        FileDNExist = 1,
        FileUnavailable = 2
    };

    //called when load is started, argument: fileName
    using StartLoadCallback = std::function<void (const std::string&)>;
    //called when load is finished, arguments: fileName, dataBuffer
    using FinishLoadCallback = std::function<void (const std::string&, std::string&)>;
    //called when load has failed, arguments: fileName, errorCode
    using ErrorLoadCallback = std::function<void (const std::string&, FileIOErrorsEnum)>;

    //called when save is started, argument: fileName
    using StartSaveCallback = std::function<void (const std::string&)>;
    //called when save is finished, argument: fileName
    using FinishSaveCallback = std::function<void (const std::string&)>;
    //called when save has failed, arguments: fileName, errorCode
    using ErrorSaveCallback = std::function<void (const std::string&, FileIOErrorsEnum)>;

private:
    struct PImpl;
    std::unique_ptr<PImpl> pimpl;
    
public:

    FileManager();
    ~FileManager();

    void loadFile(
        const std::string& filePath,
        std::string& dataBuffer,
        const StartLoadCallback onStartLoadCallback,
        const FinishLoadCallback onFinishLoadCallback,
        const ErrorLoadCallback onErrorLoadCallback
    );
    
    void stopWork();
    
    void saveFile(
        const std::string& filePath,
        const std::string& dataBuffer,
        const StartSaveCallback onStartSaveCallback,
        const FinishSaveCallback onFinishSaveCallback,
        const ErrorLoadCallback onErrorSaveCallback
    );
    
};

}// namespace TextEditorCore
