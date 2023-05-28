#include <TextEditorCore/TextManager.h>

namespace TextEditorCore
{

TextManager::TextManager()
{
}

TextManager::TextManager(const std::string& data) : data(data)
{
    
}

void TextManager::setTextData(const std::string &content)
{
    data = content;
}

void TextManager::clean()
{
    data.clear();
}

std::vector<std::pair<size_t, size_t> > TextManager::find(const std::string &token)
{
    std::vector<std::pair<size_t, size_t> > Result;
    
    while (!std::string::npos)
    {
        size_t pos = 0;
        

        Result.push_back(std::pair<size_t, size_t>(data.find(token,pos), token.length()));
        pos += token.length() + 1;
    }
    
    return Result;
}

std::string TextManager::getSegment(const std::pair<size_t, size_t> &segment) const
{
    if (data.length() < segment.first)
    {
        return std::string("");
    }
    else
    {
        return data.substr(segment.first, segment.second);
    }
}

bool TextManager::eraseSegment(const std::pair<size_t, size_t> &segment)
{
    if (data.length() < segment.first)
    {
        return false;
    }
    else
    {
        data.erase(segment.first, segment.second);
        return true;
    }
}

bool TextManager::insertSegment(const std::string &segmentData, const size_t pos)
{
    try
    {
        if (data.length() < pos)
        {
            return false;
        }
        else if(data.length() - 1 == pos)
        {
            data.append(segmentData);
            return true;
        }
        else if(0 == pos)
        {
            data.append(segmentData);
            return true;
        }
        else
        {
            data.insert(pos,segmentData);
            return true;
        }
    }

    catch(...)
    {
        return false;
    }
    
    
    
}




}// namespace TextEditorCore
