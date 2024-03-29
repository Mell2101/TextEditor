#include <TextEditorCore/Document.h>

namespace TextEditorCore
{

// ============================================================

struct Document::PImpl : public IFileIOListener
{
    size_t m_index;
    std::string m_fileName;
    
    FileManager m_fileManager;
    TextManager m_textManager;
    
    IDocumentListener* m_listener;
    
    PImpl()
    {
        m_fileManager.setListener(this);
    }
    
    void load()
    {
        m_fileManager.setDataBuffer(m_textManager.getTextData());
        m_fileManager.loadFile();
    }
    void save(const bool isRewrite)
    {
        m_fileManager.setDataBuffer(m_textManager.getTextData());
        m_fileManager.saveFile(isRewrite);
    }
    void stop()
    {
        m_fileManager.stopWork();
    }
    
    
    // IFileIOListener
    void onIOStart(const std::string& filename) override
    {
        if (m_listener)
        {
            m_listener->onStartLoading(m_index);
        }
    }
    void onIOError(const std::string& filename, FileIOErrorsEnum error) override
    {
        if (m_listener)
        {
            m_listener->onIOError(m_index, error);
        }
    }
    void onProgress(const std::string& filename, const float percent) override
    {
        if (m_listener)
        {
            m_listener->onProgress(m_index, percent);
        }
    }
    void onPause(const std::string& fileName) override
    {
        if (m_listener)
        {
            m_listener->onPause(m_index);
        }
    }
    void onResume(const std::string& fileName) override
    {
        if (m_listener)
        {
            m_listener->onResume(m_index);
        }
    }
    void onStop(const std::string& fileName) override
    {
        if (m_listener)
        {
            m_listener->onStop(m_index);
        }
    }
    void onLoadComplete(const std::string& fileName, std::string& dataBuffer) override
    {
        if (m_listener)
        {
            m_listener->onLoadComplete(m_index, dataBuffer);
        }
    }
    void onSaveComplete(const std::string& fileName) override
    {
        if (m_listener)
        {
            m_listener->onSaveComplete(m_index);
        }
    }
};

// ============================================================

Document::Document(const size_t index)
    :
        m_pImpl(std::make_unique<PImpl>())
{
    m_pImpl->m_index = index;
}

Document::~Document()
{
    m_pImpl->m_listener = nullptr;
}

void Document::setListener(IDocumentListener& listener)
{
    m_pImpl->m_listener = &listener;
}

void Document::setFileName(const std::string& fileName)
{
    m_pImpl->m_fileName = fileName;
    m_pImpl->m_fileManager.setFilePath(fileName);
}

std::string& Document::getTextData()
{
    return m_pImpl->m_textManager.getTextData();
}

void Document::setText(const std::string& text)
{
    m_pImpl->m_textManager.replaceAll(text);
    if (m_pImpl->m_listener)
    {
        m_pImpl->m_listener->onChanged(m_pImpl->m_index, m_pImpl->m_textManager.getTextData());
    }
}

void Document::modifyText(size_t pos, size_t length, const std::string& data)
{
    if (!m_pImpl->m_textManager.eraseSegment({pos, length}))
    {
        if (m_pImpl->m_listener)
        {
            m_pImpl->m_listener->onModifyError(m_pImpl->m_index);
        }
        return;
    }
    
    if (!m_pImpl->m_textManager.insertSegment(data, pos))
    {
        if (m_pImpl->m_listener)
        {
            m_pImpl->m_listener->onModifyError(m_pImpl->m_index);
        }
        return;
    }
    
    if (m_pImpl->m_listener)
    {
        m_pImpl->m_listener->onChanged(m_pImpl->m_index, m_pImpl->m_textManager.getTextData());
    }
}

void Document::load()
{
    m_pImpl->load();
}

void Document::save(const bool isRewrite)
{
    m_pImpl->save(isRewrite);
}

} // namespace TextEditorCore
