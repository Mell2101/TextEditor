#pragma once
#include <memory>
#include <string>
#include <TextEditorCore/FileIOListener.h>

/* -------------------------------------------------------------------------- */

namespace TextEditorCore
{

class FileManager
{



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
        IFileIOListener& listner 
    );
    
    void saveFile(
        const std::string& filePath,
        const std::string& dataBuffer,
        const bool isRewrite,
        IFileIOListener& listener
    );
    
    void pause(const std::string& fileName, IFileIOListener&);
    void resume(const std::string& fileName, IFileIOListener&);
    void stopWork(const std::string& fileName, IFileIOListener&);
};

}// namespace TextEditorCore
