#pragma once
#include <string>

class IFileIOListener
{
public:
    enum FileIOErrorsEnum
    {
        NoError,
        FileDNExist,
        FileUnavailable,
        FileWriteError,
        FileReWriteTaboo,
        FileReadError,
        PauseError,
        ResumeError,
        StopError,
    };

public:
    IFileIOListener() = default;
    virtual ~IFileIOListener(){};

    virtual void onIOStart(const std::string& filename) = 0;
    virtual void onIOError(const std::string& failedArguments, FileIOErrorsEnum) = 0;
    virtual void onProgress(const float percent) = 0;
    virtual void onPause(const std::string& fileName) = 0;
    virtual void onResume(const std::string& fileName) = 0;
    virtual void onStop(const std::string& fileName) = 0;
    virtual void onLoadComplete(const std::string& fileName, std::string& dataBuffer) = 0;
    virtual void onSaveComplete(const std::string& fileName) = 0;
};