#pragma once

#include <TextEditorCore/FileManager.h>
#include <TextEditorCore/TextManager.h>
#include <TextEditorCore/IDocumentListener.h>


namespace TextEditorCore
{

class Document
{
    private:
        struct PImpl;
        std::unique_ptr<PImpl> m_pImpl;
        
    public:
        Document(const size_t index);
        
        Document(const Document& other) = delete;
        Document(Document&& other) = delete;
        Document& operator=(const Document& other) = delete;
        Document& operator=(Document&& other) = delete;
        
        virtual ~Document();
        
    public:
        void setListener(IDocumentListener& listener);
        void setFileName(const std::string& fileName);
        
        std::string& getTextData();
        void setText(const std::string& text);
        void modifyText(size_t pos, size_t length, const std::string& data);
        
        void load();
        void save(const bool isRewrite);
};

} //namespace TextEditorCore