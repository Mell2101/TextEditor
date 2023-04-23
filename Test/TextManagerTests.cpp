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

