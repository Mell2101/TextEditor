#pragma once

#include <string>
#include <vector>

// =============================================================================

namespace TextEditorCore
{

class TextManager
{
private:
    std::string m_data;
    
public:
    TextManager();
    TextManager(const std::string& data);
    
    std::string& getTextData() { return m_data; };
    
    void clean();
    
    std::vector<std::pair<size_t, size_t>> find(const std::string& token);
    
    std::string getSegment(const std::pair<size_t, size_t>& segment) const;
    bool eraseSegment(const std::pair<size_t, size_t>& segment);
    bool insertSegment(const std::string& segmentData, const size_t pos);
    void replaceAll(const std::string& newData) { m_data = newData; }
};

}// namespace TextEditorCore
