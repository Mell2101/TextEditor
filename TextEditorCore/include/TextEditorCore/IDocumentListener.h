#pragma once

#include <TextEditorCore/IFileIOListener.h>

namespace TextEditorCore
{

class IDocumentListener
{
    public:
        virtual ~IDocumentListener() = default;
        
        virtual void onCreated(const size_t index) = 0;
        virtual void onChanged(const size_t index, std::string& data) = 0;
        virtual void onModifyError(const size_t index) = 0;
        
        virtual void onStartLoading(const size_t index) = 0;
        virtual void onIOError(const size_t index, FileIOErrorsEnum) = 0;
        virtual void onProgress(const size_t index, const float percent) = 0;
        virtual void onPause(const size_t index) = 0;
        virtual void onResume(const size_t index) = 0;
        virtual void onStop(const size_t index) = 0;
        virtual void onLoadComplete(const size_t index, std::string& dataBuffer) = 0;
        virtual void onSaveComplete(const size_t index) = 0;
};

} // namespace TextEditorCore
