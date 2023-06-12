#include <TextEditorCore/TextManager.h>

namespace TextEditorCore
{

TextManager::TextManager()
{
}

TextManager::TextManager(const std::string& data)
    : m_data(data)
{
    
}

void TextManager::clean()
{
    m_data.clear();
}

std::vector<std::pair<size_t, size_t> > TextManager::find(const std::string &token)
{
    std::vector<std::pair<size_t, size_t> > Result;
    
    while (!std::string::npos)
    {
        size_t pos = 0;
        
        Result.push_back(std::pair<size_t, size_t>(m_data.find(token,pos), token.length()));
        pos += token.length() + 1;
    }
    
    return Result;
}

std::string TextManager::getSegment(const std::pair<size_t, size_t> &segment) const
{
    if (m_data.length() < segment.first)
    {
        return std::string("");
    }
    else
    {
        return m_data.substr(segment.first, segment.second);
    }
}

bool TextManager::eraseSegment(const std::pair<size_t, size_t> &segment)
{
    if (m_data.length() < segment.first)
    {
        return false;
    }
    else
    {
        m_data.erase(segment.first, segment.second);
        return true;
    }
}

bool TextManager::insertSegment(const std::string &segmentData, const size_t pos)
{
    try
    {
        if (m_data.length() < pos)
        {
            return false;
        }
        else if (pos == m_data.length())
        {
            m_data.append(segmentData);
            return true;
        }
        else
        {
            m_data.insert(pos, segmentData);
            return true;
        }
    }
    
    catch(...)
    {
        return false;
    }
}

}// namespace TextEditorCore
