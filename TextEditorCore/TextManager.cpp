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
    return;
}

void TextManager::clean()
{
    data = "";
    return;
}

std::vector<std::pair<size_t, size_t> > TextManager::find(const std::string &token)
{
    return std::vector<std::pair<size_t, size_t>> (1, std::pair<size_t, size_t>(0,0));
}

std::string TextManager::getSegment(const std::pair<size_t, size_t> &segment) const
{
    return std::string("0");
}

void TextManager::eraseSegment(const std::pair<size_t, size_t> &segment)
{
    size_t it = segment.first;

    while (it != segment.second)
    {     
        data.erase(it);
        it++;
    }
    
    return;
}

void TextManager::insertSegment(const std::string &segmentData, const size_t pos)
{
    if (data.back() == pos)
    {
        data.append(segmentData, 0, segmentData.back());
    }
    else if (data.back() != pos)
    {
        data.insert(pos,segmentData);
    }
    
    
    
    return;
}




}// namespace TextEditorCore
