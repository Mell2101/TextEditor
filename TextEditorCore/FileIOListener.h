#pragma once
#include <string>

class FileIOListener
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

public:
    FileIOListener(){};
    virtual ~FileIOListener(){};

    virtual void onIOStart(const std::string& filename) = 0;
    virtual void onIOError(const std::string& failedArguments, FileIOErrorsEnum) = 0;
    virtual void onProgress(const float percent) = 0;
    virtual void onPause() = 0;
    virtual void onResume() = 0;
    virtual void onStop() = 0;
    virtual void onLoadComplete(const std::string& fileName, std::string& dataBuffer) = 0;
    virtual void onSaveComplete(const std::string& fileName) = 0;
};