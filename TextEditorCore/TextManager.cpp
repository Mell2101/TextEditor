#include "TextManager.h"

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
    return std::vector<std::pair<size_t, size_t>> (1, std::pair<size_t, size_t>(0,0));
}

std::string TextManager::getSegment(const std::pair<size_t, size_t> &segment) const
{
    return std::string("0");
}

bool TextManager::eraseSegment(const std::pair<size_t, size_t> &segment)
{
    try
    {
        data.erase(segment.first, segment.second);
    }
    catch(const std::exception& e)
    {
        return false;
    }
    
        return true;
}

bool TextManager::insertSegment(const std::string &segmentData, const size_t pos)
{
    try
    {
        if (data.length() < pos)
        {
            size_t tempPosition = pos - data.length();

            for (size_t i = tempPosition; i < pos; i++)
            {
                data.insert(tempPosition, " ");
            }
            
            data.insert(pos,segmentData);
        }
        else if (data.length() >= pos)
        {
            data.insert(pos, segmentData);
        }
    }
    catch(const std::exception& e)
    {
        return false;
    }
    
    
    
    return true;
}




}// namespace TextEditorCore
