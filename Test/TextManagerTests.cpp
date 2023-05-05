#include <TextManager.h>
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

TEST_CASE("TextManager::setTextData()", "[TextManager::setTextData()]")
{
    {
        TextEditorCore::TextManager textManager("blah blah!");
        textManager.setTextData("new text!");
        const std::string& text = textManager.getTextData();
        REQUIRE(text == "new text");
    }

    {
        TextEditorCore::TextManager textManager;
        textManager.setTextData("new text!");
        const std::string& text = textManager.getTextData();
        REQUIRE(text == "new text!");
    }

    {
        TextEditorCore::TextManager textManager;
        textManager.setTextData(std::string("aboba"));
        const std::string& text = textManager.getTextData();
        REQUIRE(text == "aboba");
    }
    
    {
        TextEditorCore::TextManager textManager("blah blah!");
        textManager.setTextData(std::string());
        const std::string& text = textManager.getTextData();
        REQUIRE(text.empty());
    }

    {
        TextEditorCore::TextManager textManager;
        textManager.setTextData(std::string());
        const std::string& text = textManager.getTextData();
        REQUIRE(text.empty());
    }
}

TEST_CASE("TextManager::insertSegment()", "[TextManager::insertSegment()]")
{
    
    {
        TextEditorCore::TextManager textManager("blah blah");
        bool result = textManager.insertSegment("new ", 0);
        const std::string& text = textManager.getTextData();
        REQUIRE((text == "new blah blah" && result == true));
    }

    {
        TextEditorCore::TextManager textManager("blah blah");
        bool result = textManager.insertSegment(std::string(), 0);
        const std::string& text = textManager.getTextData();
        REQUIRE((text == "blah blah" && result == true));
    }

    {
        TextEditorCore::TextManager textManager;
        bool result = textManager.insertSegment("new ", 0);
        const std::string& text = textManager.getTextData();
        REQUIRE((text == "new " && result == true));
    }

    {
        TextEditorCore::TextManager textManager("blah blah");
        bool result = textManager.insertSegment("new ", 125);
        const std::string& text = textManager.getTextData();
        REQUIRE((text == "blah blah" && result == false));
    }

    {
        TextEditorCore::TextManager textManager;
        bool result = textManager.insertSegment(std::string(), 0);
        const std::string& text = textManager.getTextData();
        REQUIRE((text == "" && result == true));
    }
}

TEST_CASE("TextManager::eraseSegment()", "[TextManager::eraseSegment()]")
{
    
    {
        TextEditorCore::TextManager textManager("blah blah");
        bool result = textManager.eraseSegment(std::make_pair(0,5));
        const std::string& text = textManager.getTextData();
        REQUIRE((text == " blah" && result == true));
    }

    // tests erasing segment of 0 length
    {
        TextEditorCore::TextManager textManager("blah blah");
        bool result = textManager.eraseSegment(std::make_pair(0,0));
        const std::string& text = textManager.getTextData();
        REQUIRE((text == "blah blah" && result == true));
    }
    // test erasing of empty TextManager
    {
        TextEditorCore::TextManager textManager;
        bool result = textManager.eraseSegment(std::make_pair(0,5));
        const std::string& text = textManager.getTextData();
        REQUIRE((text == "" && result == false));
    }
    // test incorect eraseSegment range
    {
        TextEditorCore::TextManager textManager("blah blah");
        bool result = textManager.eraseSegment(std::make_pair(5,0));
        const std::string& text = textManager.getTextData();
        REQUIRE((text == "blah blah" && result == false));
    }
    // test erasing rage greater then text
    {
        TextEditorCore::TextManager textManager("blah blah");
        bool result = textManager.eraseSegment(std::make_pair(1,123));
        const std::string& text = textManager.getTextData();
        REQUIRE((text == "b" && result == true));
    }
    // erasing 0 length segment from emty TextManager
    {
        TextEditorCore::TextManager textManager;
        bool result = textManager.eraseSegment(std::make_pair(0,0));
        const std::string& text = textManager.getTextData();
        REQUIRE((text == "" && result == true));
    }
}