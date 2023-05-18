#pragma once
#include <memory>
#include <functional>


/* -------------------------------------------------------------------------- */



namespace TextEditorCore
{
class FileIOListener;


class FileManager //Signleton
{
public:
    enum FileIOErrorsEnum
    {
        NoError = 0,
        FileDNExist = 1,
        FileUnavailable = 2,
        FileWriteError = 3,
        FileReWriteTaboo = 4,
        FileReadError = 5
    };

    using pauseFlag = bool;
private:
    struct PImpl;
    std::unique_ptr<PImpl> pimpl;
    
public:
    // chunkSize - file part size we can read/write in singe iteration (Default 4 kB)
    FileManager(const size_t chunkSize = 4096);
    
    ~FileManager();

    void loadFile(
        const std::string& filePath,
        std::string& dataBuffer,
        FileIOListener& listner 
    );

    void saveFile(
        const std::string& filePath,
        const std::string& dataBuffer,
        const bool isRewrite,
        FileIOListener& listener
    );

    void pause(const std::string& filePath, FileIOListener&);

    void resume(const std::string& filePath, FileIOListener&);

    void stopWork();
    
};


class FileIOListener
{
public:
    virtual ~FileIOListener();

    virtual void onIOStart(const std::string& filename) = 0;
    virtual void onIOError(const std::string& failedArguments, FileManager::FileIOErrorsEnum) = 0;
    virtual void onProgress(const size_t percent) = 0;
    virtual void onPause() = 0;
    virtual void onResume() = 0;
    virtual void onStop() = 0;
    virtual void onLoadComplete(const std::string& fileName, std::string& dataBuffer) = 0;
    virtual void onSaveComplete(const std::string& fileName) = 0;
};

}// namespace TextEditorCore



    // //called when load is started, argument: fileName
    // using StartLoadCallback = std::function<void (const std::string&)>;
    // //called when load is finished, arguments: fileName, dataBuffer
    // using FinishLoadCallback = std::function<void (const std::string&, std::string&)>;
    // //called when load has failed, arguments: fileName, errorCode
    // using ErrorLoadCallback = std::function<void (const std::string&, FileIOErrorsEnum)>;

    // //called when save is started, argument: fileName
    // using StartSaveCallback = std::function<void (const std::string&)>;
    // //called when save is finished, argument: fileName
    // using FinishSaveCallback = std::function<void (const std::string&)>;
    // //called when save has failed, arguments: fileName, errorCode
    // using ErrorSaveCallback = std::function<void (const std::string&, FileIOErrorsEnum)>;


    // using BytesReadCallback = std::function<void (const size_t)>;
    // using FilePartLoadedCallback = std::function<void (const float)>;

    // using BytesSavedCallback = std::function<void (const size_t)>;