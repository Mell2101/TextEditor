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
    return;
}

void TextManager::clean()
{
    return;
}

std::vector<std::pair<size_t, size_t> > TextManager::find(const std::string &token)
{
    return std::vector<std::pair<size_t, size_t> >();
}

std::string TextManager::getSegment(const std::pair<size_t, size_t> &segment) const
{
    return std::string();
}

void TextManager::eraseSegment(const std::pair<size_t, size_t> &segment)
{
    return;
}

void TextManager::insertSegment(const std::string &segmentData, const size_t pos)
{
    return;
}




}// namespace TextEditorCore
