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
    // ~TextManager();
    
    void setTextData(const std::string& content);
    const std::string& getTextData() const  { return data; };
    
    void clean();
    
    // //returns pairs, containing pos and size
    std::vector<std::pair<size_t, size_t>> find(const std::string& token);
    
    std::string getSegment(const std::pair<size_t, size_t>& segment) const;
    void eraseSegment(const std::pair<size_t, size_t>& segment);
    void insertSegment(const std::string& segmentData, const size_t pos);
};

}// namespace TextEditorCore
