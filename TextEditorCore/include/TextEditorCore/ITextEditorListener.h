#pragma once

#include <string>


class ITextEditorListener
{
public:
    virtual ~ITextEditorListener();
    
    virtual void onTextModified(size_t pos, size_t length, const std::string& data) = 0;
    virtual void onFileLoaded(const std::string& fileName, std::string& data) = 0;
    virtual void onFileSaved(const std::string& fileName) = 0;
    virtual void onFileIOError(const std::string& fileName, const std::string& errorMessage) = 0;
};