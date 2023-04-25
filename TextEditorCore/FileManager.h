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
        FileUnavailable = 2,
        FileUnableToOpen = 3,
        FileWriteError = 4
    };

    using StartLoadCallback = std::function<void (const std::string&)>;
    using FinishLoadCallback = std::function<void (const std::string&, std::string&)>;
    using ErrorLoadCallback = std::function<void (const std::string&, FileIOErrorsEnum)>;

    using StartSaveCallback = std::function<void (const std::string&)>;
    using FinishSaveCallback = std::function<void (const std::string&)>;
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
        StartLoadCallback& onStartLoadCallback,
        FinishLoadCallback& onFinishLoadCallback,
        ErrorLoadCallback& onErrorLoadCallback
    );
    
    void stopWork();
    
    void saveFile(
        const std::string& filePath,
        const std::string& dataBuffer,
        const bool isRewrite,
        StartSaveCallback& onStartSaveCallback,
        FinishSaveCallback& onFinishSaveCallback,
        ErrorLoadCallback& onErrorSaveCallback
    );
    
};

}// namespace TextEditorCore
