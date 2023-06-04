#pragma once
#include <memory>
#include <string>
#include <TextEditorCore/IFileIOListener.h>

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
    
    void setFilePath(const std::string& filePath);
    void setDataBuffer(std::string& dataBuffer);
    void setListener(IFileIOListener* listener);
    
    void loadFile();
    void saveFile(const bool isRewrite);
    
    void pause();
    void resume();
    void stopWork();
};

}// namespace TextEditorCore
