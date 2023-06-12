#include <TextEditorCore/TextManager.h>
#include <catch2/catch_all.hpp>

TEST_CASE("TextManager::TextManager()", "[TextManager::TextManager()]")
{
    TextEditorCore::TextManager textManager;
    const std::string& text = textManager.getTextData();
    REQUIRE(text.empty());
}

TEST_CASE("TextManager::TextManager(str)", "[TextManager::TextManager(str)]")
{
    TextEditorCore::TextManager textManager("blah blah!");
    const std::string& text = textManager.getTextData();
    REQUIRE(text == "blah blah!");
}

TEST_CASE("TextManager::clean()", "[TextManager::clean()]")
{
    {
        TextEditorCore::TextManager textManager("blah blah!");
        textManager.clean();
        const std::string& text = textManager.getTextData();
        REQUIRE(text.empty());
    }
    
    {
        TextEditorCore::TextManager textManager;
        textManager.clean();
        const std::string& text = textManager.getTextData();
        REQUIRE(text.empty());
    }
}

TEST_CASE("TextManager::insertSegment()", "[TextManager::insertSegment()]")
{
    // insert a segment at the begining
    {
        TextEditorCore::TextManager textManager("blah blah");
        bool result = textManager.insertSegment("new ", 0);
        const std::string& text = textManager.getTextData();
        REQUIRE(text == "new blah blah");
        REQUIRE(result == true);
    }
    
    // insert a segment at the end
    {
        TextEditorCore::TextManager textManager("blah blah");
        bool result = textManager.insertSegment(" new", textManager.getTextData().size());
        const std::string& text = textManager.getTextData();
        REQUIRE(text == "blah blah new");
        REQUIRE(result == true);
    }
    
    // insert a segment in hte midle
        {
        TextEditorCore::TextManager textManager("blah blah");
        bool result = textManager.insertSegment("new ", 5);
        const std::string& text = textManager.getTextData();
        REQUIRE(text == "blah new blah");
        REQUIRE(result == true);
    }
    
    // insert emty string
    {
        TextEditorCore::TextManager textManager("blah blah");
        bool result = textManager.insertSegment(std::string(), 0);
        const std::string& text = textManager.getTextData();
        REQUIRE(text == "blah blah");
        REQUIRE(result == true);
    }
    // insert segment in position higher then textManager size
    {
        TextEditorCore::TextManager textManager("blah blah");
        bool result = textManager.insertSegment("new ", 125);
        const std::string& text = textManager.getTextData();
        REQUIRE(text == "blah blah");
        REQUIRE(result == false);
    }
    // insert emty string in emty textManager
    {
        TextEditorCore::TextManager textManager;
        bool result = textManager.insertSegment(std::string(), 0);
        const std::string& text = textManager.getTextData();
        REQUIRE((text == "" && result == true));
        REQUIRE(result == true);
    }
}

TEST_CASE("TextManager::eraseSegment()", "[TextManager::eraseSegment()]")
{   
    {
        TextEditorCore::TextManager textManager("blah blah");
        bool result = textManager.eraseSegment(std::make_pair<size_t, size_t>(0,5));
        const std::string& text = textManager.getTextData();
        REQUIRE(text == "blah");
        REQUIRE(result == true);
    }

    // tests erasing segment of 0 length
    {
        TextEditorCore::TextManager textManager("blah blah");
        bool result = textManager.eraseSegment(std::make_pair<size_t, size_t>(0,0));
        const std::string& text = textManager.getTextData();
        REQUIRE(text == "blah blah");
        REQUIRE(result == true);
    }
    // check if the segment is removed outside of bounds
    {
        TextEditorCore::TextManager textManager;
        bool result = textManager.eraseSegment(std::make_pair<size_t, size_t>(156,5));
        const std::string& text = textManager.getTextData();
        REQUIRE(text == "");
        REQUIRE(result == false);
    }
    // test incorect eraseSegment range
    {
        TextEditorCore::TextManager textManager("blah blah");
        bool result = textManager.eraseSegment(std::make_pair<size_t, size_t>(5,0));
        const std::string& text = textManager.getTextData();
        REQUIRE(text == "blah blah");
        REQUIRE(result == true);
    }
    // test erasing rage greater then text
    {
        TextEditorCore::TextManager textManager("blah blah");
        bool result = textManager.eraseSegment(std::make_pair<size_t, size_t>(1,123));
        const std::string& text = textManager.getTextData();
        REQUIRE(text == "b");
        REQUIRE(result == true);
    }
    // erasing 0 length segment from emty TextManager
    {
        TextEditorCore::TextManager textManager;
        bool result = textManager.eraseSegment(std::make_pair<size_t, size_t>(0,0));
        const std::string& text = textManager.getTextData();
        REQUIRE(text == "" );
        REQUIRE(result == true);
    }
}