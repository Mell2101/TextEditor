#pragma once

#include <string>
#include <vector>

// =============================================================================

namespace TextEditorCore
{

class TextManager
{
private:
    std::string data;
    
public:
    TextManager();
    TextManager(const std::string& data);
    
    void setTextData(const std::string& content);
    std::string& getTextData() { return data; };
    
    void clean();
    
    std::vector<std::pair<size_t, size_t>> find(const std::string& token);
    
    std::string getSegment(const std::pair<size_t, size_t>& segment) const;
    bool eraseSegment(const std::pair<size_t, size_t>& segment);
    bool insertSegment(const std::string& segmentData, const size_t pos);
};

}// namespace TextEditorCore
