#pragma once

#include <string>

namespace TextEditorCore
{

enum FileIOErrorsEnum
{
    NoError,
    FilePathIsEmpty,
    DataBufferIsNotSet,
    FileDNExist,
    FileUnavailable,
    FileWriteError,
    FileReWriteTaboo,
    FileReadError,
    PauseError,
    ResumeError,
    StopError,
    IOInProgress,
};

class IFileIOListener
{
public:
    virtual ~IFileIOListener() = default;
    
    virtual void onIOStart(const std::string& filename) = 0;
    virtual void onIOError(const std::string& filename, FileIOErrorsEnum) = 0;
    virtual void onProgress(const std::string& filename, const float percent) = 0;
    virtual void onPause(const std::string& fileName) = 0;
    virtual void onResume(const std::string& fileName) = 0;
    virtual void onStop(const std::string& fileName) = 0;
    virtual void onLoadComplete(const std::string& fileName, std::string& dataBuffer) = 0;
    virtual void onSaveComplete(const std::string& fileName) = 0;
};


} // namespace TextEditorCore